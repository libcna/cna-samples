// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// NetworkSessionComponent.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Networking/NetworkSessionComponent.hpp"

#include <cstdio>
#include <exception>
#include <optional>
#include <utility>

#include "IMessageDisplay.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameComponentCollection.hpp"
#include "Microsoft/Xna/Framework/GameServiceContainer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Gamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerPrivilegeException.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerPrivileges.hpp"
#include "Microsoft/Xna/Framework/GamerServices/InviteAcceptedEventArgs.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamerCollection.hpp"
#include "Microsoft/Xna/Framework/Net/GamerJoinedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/GamerLeftEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionEndReason.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionEndedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionState.hpp"
#include "Networking/LobbyScreen.hpp"
#include "Networking/NetworkBusyScreen.hpp"
#include "Networking/NetworkErrorScreen.hpp"
#include "Resources.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/BackgroundScreen.hpp"
#include "Screens/LoadingScreen.hpp"
#include "Screens/MainMenuScreen.hpp"
#include "Screens/MessageBoxScreen.hpp"
#include "System/NotSupportedException.hpp"

namespace NetworkStateManagement
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::GamerServices;
    using namespace Microsoft::Xna::Framework::Net;

    namespace
    {
        // Components that left their session this frame; see ReleaseRetired.
        std::vector<std::shared_ptr<NetworkSessionComponent>>& Retired()
        {
            static std::vector<std::shared_ptr<NetworkSessionComponent>> retired;
            return retired;
        }
    }

    // The constructor is private: external callers should use the Create method.
    NetworkSessionComponent::NetworkSessionComponent(ScreenManager& screenManager,
                                                     std::shared_ptr<NetworkSession> networkSession)
        : GameComponent(screenManager.getGameProperty())
        , screenManager(screenManager)
        , networkSession(std::move(networkSession))
    {
        // Hook up our session event handlers.
        this->networkSession->GamerJoined += [this](System::Object* sender, const GamerJoinedEventArgs& e) { GamerJoined(sender, e); };
        this->networkSession->GamerLeft += [this](System::Object* sender, const GamerLeftEventArgs& e) { GamerLeft(sender, e); };
        this->networkSession->SessionEnded += [this](System::Object* sender, const NetworkSessionEndedEventArgs& e)
        {
            NetworkSessionEnded(sender, e);
        };
    }

    void NetworkSessionComponent::Create(ScreenManager& screenManager, std::shared_ptr<NetworkSession> networkSession)
    {
        Game& game = screenManager.getGameProperty();

        // Register this network session as a service.
        game.getServicesProperty().AddService<NetworkSession>(networkSession.get());

        // Create a NetworkSessionComponent, and add it to the Game.
        std::shared_ptr<NetworkSessionComponent> component(new NetworkSessionComponent(screenManager, std::move(networkSession)));
        game.getComponentsProperty().Add(std::shared_ptr<IGameComponent>(component));
    }

    void NetworkSessionComponent::Initialize()
    {
        GameComponent::Initialize();

        // Look up the IMessageDisplay service, which will
        // be used to report gamer join/leave notifications.
        messageDisplay = getGameProperty().getServicesProperty().GetService<IMessageDisplay>();

        if (messageDisplay != nullptr)
            notifyWhenPlayersJoinOrLeave = true;
    }

    void NetworkSessionComponent::Dispose(bool disposing)
    {
        if (disposing)
        {
            // The game may still hold this component's pointer for the rest of the frame.
            Retired().push_back(shared_from_this());

            // Remove the NetworkSessionComponent.
            (void)getGameProperty().getComponentsProperty().Remove(this);

            // Remove the NetworkSession service.
            getGameProperty().getServicesProperty().RemoveService<NetworkSession>();

            // Dispose the NetworkSession.
            if (networkSession != nullptr)
            {
                networkSession->Dispose();
                networkSession = nullptr;
            }
        }

        GameComponent::Dispose(disposing);
    }

    void NetworkSessionComponent::ReleaseRetired() { Retired().clear(); }

    void NetworkSessionComponent::Update(GameTime&)
    {
        if (networkSession == nullptr)
            return;

        try
        {
            networkSession->Update();

            // Has the session ended?
            if (networkSession->getSessionStateProperty() == NetworkSessionState::Ended)
            {
                LeaveSession();
            }
        }
        catch (const std::exception& exception)
        {
            // Handle any errors from the network session update.
            std::fprintf(stderr, "NetworkSession.Update threw %s\n", exception.what());

            sessionEndMessage = Resources::ErrorNetwork;

            LeaveSession();
        }
    }

    // Event handler called when a gamer joins the session. Displays a notification message.
    void NetworkSessionComponent::GamerJoined(System::Object*, const GamerJoinedEventArgs& e)
    {
        if (notifyWhenPlayersJoinOrLeave)
        {
            messageDisplay->ShowMessage(Resources::MessageGamerJoined, {e.getGamerProperty()->getGamertagProperty()});
        }
    }

    // Event handler called when a gamer leaves the session. Displays a notification message.
    void NetworkSessionComponent::GamerLeft(System::Object*, const GamerLeftEventArgs& e)
    {
        if (notifyWhenPlayersJoinOrLeave)
        {
            messageDisplay->ShowMessage(Resources::MessageGamerLeft, {e.getGamerProperty()->getGamertagProperty()});
        }
    }

    // Event handler called when the network session ends. Stores the end reason, so this can later
    // be displayed to the user.
    void NetworkSessionComponent::NetworkSessionEnded(System::Object*, const NetworkSessionEndedEventArgs& e)
    {
        switch (e.getEndReasonProperty())
        {
        case NetworkSessionEndReason::ClientSignedOut:
            sessionEndMessage.clear();
            break;

        case NetworkSessionEndReason::HostEndedSession:
            sessionEndMessage = Resources::ErrorHostEndedSession;
            break;

        case NetworkSessionEndReason::RemovedByHost:
            sessionEndMessage = Resources::ErrorRemovedByHost;
            break;

        case NetworkSessionEndReason::Disconnected:
        default:
            sessionEndMessage = Resources::ErrorDisconnected;
            break;
        }

        notifyWhenPlayersJoinOrLeave = false;
    }

    // Event handler called when the system delivers an invite notification. This can occur when
    // the user accepts an invite that was sent to them by a friend (pull mode), or if they choose
    // the "Join Session In Progress" option in their friends screen (push mode). The handler leaves
    // the current session (if any), then joins the session referred to by the invite. It is not
    // necessary to prompt the user before doing this, as the Guide will already have taken care of
    // the necessary confirmations before the invite was delivered to you.
    void NetworkSessionComponent::InviteAccepted(ScreenManager& screenManager, const InviteAcceptedEventArgs& e)
    {
        // If we are already in a network session, leave it now.
        NetworkSessionComponent* self = FindSessionComponent(screenManager.getGameProperty());

        if (self != nullptr)
            self->Dispose();

        try
        {
            // Which local profiles should we include in this session?
            const std::vector<SignedInGamer*> localGamers =
                ChooseGamers(NetworkSessionType::PlayerMatch, e.getGamerProperty()->getPlayerIndexProperty());

            // Begin an asynchronous join-from-invite operation.
            System::IAsyncResult* asyncResult = NetworkSession::BeginJoinInvited(localGamers, {}, {});

            // Use the loading screen to replace whatever screens were previously
            // active. This will completely reset the screen state, regardless of
            // whether we were in the menus or playing a game when the invite was
            // delivered. When the loading screen finishes, it will activate the
            // network busy screen, which displays an animation as it waits for
            // the join operation to complete.
            auto busyScreen = std::make_shared<NetworkBusyScreen>(asyncResult);

            busyScreen->OperationCompleted += [](System::Object* sender, const OperationCompletedEventArgs& completed)
            {
                JoinInvitedOperationCompleted(sender, completed);
            };

            std::vector<std::shared_ptr<GameScreen>> screens;
            screens.push_back(std::make_shared<BackgroundScreen>());
            screens.push_back(std::move(busyScreen));
            LoadingScreen::Load(screenManager, false, std::nullopt, std::move(screens));
        }
        catch (const std::exception& exception)
        {
            std::vector<std::shared_ptr<GameScreen>> screens;
            screens.push_back(std::make_shared<BackgroundScreen>());
            screens.push_back(std::make_shared<MainMenuScreen>());
            screens.push_back(std::make_shared<NetworkErrorScreen>(exception));
            LoadingScreen::Load(screenManager, false, std::nullopt, std::move(screens));
        }
    }

    // Event handler for when the asynchronous join-from-invite operation has completed.
    void NetworkSessionComponent::JoinInvitedOperationCompleted(System::Object* sender, const OperationCompletedEventArgs& e)
    {
        ScreenManager& screenManager = dynamic_cast<GameScreen*>(sender)->getScreenManagerProperty();
        std::unique_ptr<System::IAsyncResult> asyncResult(e.getAsyncResultProperty());

        try
        {
            // End the asynchronous join-from-invite operation.
            std::shared_ptr<NetworkSession> networkSession(NetworkSession::EndJoinInvited(asyncResult.get()));

            // Create a component that will manage the session we just created.
            NetworkSessionComponent::Create(screenManager, networkSession);

            // Go to the lobby screen.
            screenManager.AddScreen(std::make_shared<LobbyScreen>(networkSession), std::nullopt);
        }
        catch (const std::exception& exception)
        {
            screenManager.AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
            screenManager.AddScreen(std::make_shared<NetworkErrorScreen>(exception), std::nullopt);
        }
    }

    bool NetworkSessionComponent::IsOnlineSessionType(NetworkSessionType sessionType)
    {
        switch (sessionType)
        {
        case NetworkSessionType::Local:
        case NetworkSessionType::SystemLink:
            return false;

        case NetworkSessionType::PlayerMatch:
        case NetworkSessionType::Ranked:
            return true;

        default:
            throw System::NotSupportedException();
        }
    }

    // Decides which local player profiles should be included in a network session. This is passed
    // the index of the primary player (the one who selected the relevant menu option, or who is
    // responding to an invite). We must always include the primary player, but we can also include
    // other local profiles who are signed in to the appropriate kind of network.
    std::vector<SignedInGamer*> NetworkSessionComponent::ChooseGamers(NetworkSessionType sessionType, PlayerIndex playerIndex)
    {
        std::vector<SignedInGamer*> gamers;

        // Look up the primary gamer, and make sure they are signed in.
        SignedInGamer* primaryGamer = (*Gamer::getSignedInGamersProperty())[playerIndex];

        if (primaryGamer == nullptr)
            throw GamerPrivilegeException();

        gamers.push_back(primaryGamer);

        // Check whether any other profiles should also be included.
        for (SignedInGamer* gamer : *Gamer::getSignedInGamersProperty())
        {
            // Never include more profiles than the MaxLocalGamers constant.
            if (static_cast<int>(gamers.size()) >= MaxLocalGamers)
                break;

            // Don't want two copies of the primary gamer!
            if (gamer == primaryGamer)
                continue;

            // If this is an online session, make sure the profile is signed
            // in to Live, and that it has the privilege for online gameplay.
            if (IsOnlineSessionType(sessionType))
            {
                if (!gamer->getIsSignedInToLiveProperty())
                    continue;

                if (!gamer->getPrivilegesProperty().getAllowOnlineSessionsProperty())
                    continue;
            }

            if (primaryGamer->getIsGuestProperty() && !gamer->getIsGuestProperty() && gamers[0] == primaryGamer)
            {
                // Special case: if the primary gamer is a guest profile,
                // we should insert some other non-guest at the start of the
                // output list, because guests aren't allowed to host sessions.
                gamers.insert(gamers.begin(), gamer);
            }
            else
            {
                gamers.push_back(gamer);
            }
        }

        return gamers;
    }

    void NetworkSessionComponent::LeaveSession(ScreenManager& screenManager, PlayerIndex playerIndex)
    {
        NetworkSessionComponent* self = FindSessionComponent(screenManager.getGameProperty());

        if (self != nullptr)
        {
            // Display a message box to confirm the user really wants to leave.
            const std::string& message = self->networkSession->getIsHostProperty() ? Resources::ConfirmEndSession
                                                                                   : Resources::ConfirmLeaveSession;

            auto confirmMessageBox = std::make_shared<MessageBoxScreen>(message);

            // Hook the messge box ok event to actually leave the session.
            std::weak_ptr<NetworkSessionComponent> weakSelf = self->shared_from_this();
            confirmMessageBox->Accepted += [weakSelf](System::Object*, const PlayerIndexEventArgs&)
            {
                if (auto component = weakSelf.lock())
                    component->LeaveSession();
            };

            screenManager.AddScreen(std::move(confirmMessageBox), playerIndex);
        }
    }

    // Internal method for leaving the network session. This disposes the session, removes the
    // NetworkSessionComponent, and returns the user to the main menu screen.
    void NetworkSessionComponent::LeaveSession()
    {
        // Destroy this NetworkSessionComponent.
        Dispose();

        // If we have a sessionEndMessage string explaining why the session has
        // ended (maybe this was a network disconnect, or perhaps the host kicked
        // us out?) create a message box to display this reason to the user.
        std::shared_ptr<MessageBoxScreen> messageBox;

        if (!sessionEndMessage.empty())
            messageBox = std::make_shared<MessageBoxScreen>(sessionEndMessage, false);

        // At this point we want to return the user all the way to the main menu screen. If a
        // MainMenuScreen is still on the stack we pop back to it; otherwise (after gameplay the
        // stack has been emptied) we reset everything via the LoadingScreen.
        const std::vector<std::shared_ptr<GameScreen>> screens = screenManager.GetScreens();

        // Look for the MainMenuScreen.
        for (std::size_t i = 0; i < screens.size(); i++)
        {
            if (dynamic_cast<MainMenuScreen*>(screens[i].get()) != nullptr)
            {
                // If we found one, pop everything since then to return back to it.
                for (std::size_t j = i + 1; j < screens.size(); j++)
                    screens[j]->ExitScreen();

                // Display the why-did-the-session-end message box.
                if (messageBox != nullptr)
                    screenManager.AddScreen(std::move(messageBox), std::nullopt);

                return;
            }
        }

        // If we didn't find an existing MainMenuScreen, reload everything.
        // The why-did-the-session-end message box will be displayed after
        // the loading screen has completed.
        std::vector<std::shared_ptr<GameScreen>> screensToLoad;
        screensToLoad.push_back(std::make_shared<BackgroundScreen>());
        screensToLoad.push_back(std::make_shared<MainMenuScreen>());
        screensToLoad.push_back(std::move(messageBox));
        LoadingScreen::Load(screenManager, false, std::nullopt, std::move(screensToLoad));
    }

    // Searches through the Game.Components collection to find the NetworkSessionComponent (if any).
    NetworkSessionComponent* NetworkSessionComponent::FindSessionComponent(Game& game)
    {
        for (IGameComponent* component : game.getComponentsProperty())
        {
            if (auto* session = dynamic_cast<NetworkSessionComponent*>(component))
                return session;
        }
        return nullptr;
    }

    const std::string& NetworkSessionComponent::GetTypeName() const
    {
        static const std::string name = "NetworkStateManagement.NetworkSessionComponent";
        return name;
    }
}
