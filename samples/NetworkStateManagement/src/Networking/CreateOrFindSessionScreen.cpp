// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CreateOrFindSessionScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Networking/CreateOrFindSessionScreen.hpp"

#include <exception>
#include <memory>
#include <utility>
#include <vector>

#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/Net/AvailableNetworkSessionCollection.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionProperties.hpp"
#include "Networking/JoinSessionScreen.hpp"
#include "Networking/LobbyScreen.hpp"
#include "Networking/NetworkBusyScreen.hpp"
#include "Networking/NetworkErrorScreen.hpp"
#include "Networking/NetworkSessionComponent.hpp"
#include "Resources.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/MessageBoxScreen.hpp"
#include "System/NotSupportedException.hpp"

namespace NetworkStateManagement
{
    using Microsoft::Xna::Framework::GamerServices::SignedInGamer;
    using Microsoft::Xna::Framework::Net::AvailableNetworkSessionCollection;
    using Microsoft::Xna::Framework::Net::NetworkSession;
    using Microsoft::Xna::Framework::Net::NetworkSessionProperties;
    using Microsoft::Xna::Framework::Net::NetworkSessionType;

    CreateOrFindSessionScreen::CreateOrFindSessionScreen(NetworkSessionType sessionType)
        : MenuScreen(GetMenuTitle(sessionType))
        , sessionType(sessionType)
    {
        // Create our menu entries.
        auto createSessionMenuEntry = std::make_shared<MenuEntry>(Resources::CreateSession);
        auto findSessionsMenuEntry = std::make_shared<MenuEntry>(Resources::FindSessions);
        auto backMenuEntry = std::make_shared<MenuEntry>(Resources::Back);

        // Hook up menu event handlers.
        createSessionMenuEntry->Selected += [this](System::Object* sender, const PlayerIndexEventArgs& e)
        {
            CreateSessionMenuEntrySelected(sender, e);
        };
        findSessionsMenuEntry->Selected += [this](System::Object* sender, const PlayerIndexEventArgs& e)
        {
            FindSessionsMenuEntrySelected(sender, e);
        };
        backMenuEntry->Selected += [this](System::Object* sender, const PlayerIndexEventArgs& e)
        {
            MenuScreen::OnCancel(sender, e);
        };

        // Add entries to the menu.
        auto& entries = getMenuEntriesProperty();
        entries.push_back(std::move(createSessionMenuEntry));
        entries.push_back(std::move(findSessionsMenuEntry));
        entries.push_back(std::move(backMenuEntry));
    }

    // Helper chooses an appropriate menu title for the specified session type.
    std::string CreateOrFindSessionScreen::GetMenuTitle(NetworkSessionType sessionType)
    {
        switch (sessionType)
        {
        case NetworkSessionType::PlayerMatch:
            return Resources::PlayerMatch;
        case NetworkSessionType::SystemLink:
            return Resources::SystemLink;
        default:
            throw System::NotSupportedException();
        }
    }

    void CreateOrFindSessionScreen::CreateSessionMenuEntrySelected(System::Object*, const PlayerIndexEventArgs&)
    {
        try
        {
            // Which local profiles should we include in this session?
            const std::vector<SignedInGamer*> localGamers =
                NetworkSessionComponent::ChooseGamers(sessionType, getControllingPlayerProperty().value());

            // Begin an asynchronous create network session operation.
            System::IAsyncResult* asyncResult = NetworkSession::BeginCreate(
                sessionType, localGamers, NetworkSessionComponent::MaxGamers, 0, NetworkSessionProperties(), {}, {});

            // Activate the network busy screen, which will display
            // an animation until this operation has completed.
            auto busyScreen = std::make_shared<NetworkBusyScreen>(asyncResult);
            busyScreen->OperationCompleted += [this](System::Object* sender, const OperationCompletedEventArgs& e)
            {
                CreateSessionOperationCompleted(sender, e);
            };

            getScreenManagerProperty().AddScreen(std::move(busyScreen), getControllingPlayerProperty());
        }
        catch (const std::exception& exception)
        {
            getScreenManagerProperty().AddScreen(std::make_shared<NetworkErrorScreen>(exception), getControllingPlayerProperty());
        }
    }

    void CreateOrFindSessionScreen::CreateSessionOperationCompleted(System::Object*, const OperationCompletedEventArgs& e)
    {
        std::unique_ptr<System::IAsyncResult> asyncResult(e.getAsyncResultProperty());
        try
        {
            // End the asynchronous create network session operation.
            std::shared_ptr<NetworkSession> networkSession(NetworkSession::EndCreate(asyncResult.get()));

            // Create a component that will manage the session we just created.
            NetworkSessionComponent::Create(getScreenManagerProperty(), networkSession);

            // Go to the lobby screen. We pass null as the controlling player,
            // because the lobby screen accepts input from all local players
            // who are in the session, not just a single controlling player.
            getScreenManagerProperty().AddScreen(std::make_shared<LobbyScreen>(networkSession), std::nullopt);
        }
        catch (const std::exception& exception)
        {
            getScreenManagerProperty().AddScreen(std::make_shared<NetworkErrorScreen>(exception), getControllingPlayerProperty());
        }
    }

    void CreateOrFindSessionScreen::FindSessionsMenuEntrySelected(System::Object*, const PlayerIndexEventArgs&)
    {
        try
        {
            // Which local profiles should we include in this session?
            const std::vector<SignedInGamer*> localGamers =
                NetworkSessionComponent::ChooseGamers(sessionType, getControllingPlayerProperty().value());

            // Begin an asynchronous find network sessions operation.
            System::IAsyncResult* asyncResult =
                NetworkSession::BeginFind(sessionType, localGamers, NetworkSessionProperties(), {}, {});

            // Activate the network busy screen, which will display
            // an animation until this operation has completed.
            auto busyScreen = std::make_shared<NetworkBusyScreen>(asyncResult);
            busyScreen->OperationCompleted += [this](System::Object* sender, const OperationCompletedEventArgs& e)
            {
                FindSessionsOperationCompleted(sender, e);
            };

            getScreenManagerProperty().AddScreen(std::move(busyScreen), getControllingPlayerProperty());
        }
        catch (const std::exception& exception)
        {
            getScreenManagerProperty().AddScreen(std::make_shared<NetworkErrorScreen>(exception), getControllingPlayerProperty());
        }
    }

    void CreateOrFindSessionScreen::FindSessionsOperationCompleted(System::Object*, const OperationCompletedEventArgs& e)
    {
        std::unique_ptr<System::IAsyncResult> asyncResult(e.getAsyncResultProperty());
        std::shared_ptr<GameScreen> nextScreen;

        try
        {
            // End the asynchronous find network sessions operation.
            AvailableNetworkSessionCollection availableSessions = NetworkSession::EndFind(asyncResult.get());

            if (availableSessions.getCountProperty() == 0)
            {
                // If we didn't find any sessions, display an error.
                availableSessions.Dispose();

                nextScreen = std::make_shared<MessageBoxScreen>(Resources::NoSessionsFound, false);
            }
            else
            {
                // If we did find some sessions, proceed to the JoinSessionScreen.
                nextScreen = std::make_shared<JoinSessionScreen>(std::move(availableSessions));
            }
        }
        catch (const std::exception& exception)
        {
            nextScreen = std::make_shared<NetworkErrorScreen>(exception);
        }

        getScreenManagerProperty().AddScreen(std::move(nextScreen), getControllingPlayerProperty());
    }

    const std::string& CreateOrFindSessionScreen::GetTypeName() const
    {
        static const std::string name = "NetworkStateManagement.CreateOrFindSessionScreen";
        return name;
    }
}
