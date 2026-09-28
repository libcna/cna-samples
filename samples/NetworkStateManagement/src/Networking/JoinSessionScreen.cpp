// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// JoinSessionScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Networking/JoinSessionScreen.hpp"

#include <exception>
#include <memory>
#include <utility>

#include "Microsoft/Xna/Framework/Net/AvailableNetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Networking/AvailableSessionMenuEntry.hpp"
#include "Networking/LobbyScreen.hpp"
#include "Networking/NetworkBusyScreen.hpp"
#include "Networking/NetworkErrorScreen.hpp"
#include "Networking/NetworkSessionComponent.hpp"
#include "Resources.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace NetworkStateManagement
{
    using Microsoft::Xna::Framework::Net::AvailableNetworkSession;
    using Microsoft::Xna::Framework::Net::AvailableNetworkSessionCollection;
    using Microsoft::Xna::Framework::Net::NetworkSession;

    JoinSessionScreen::JoinSessionScreen(AvailableNetworkSessionCollection availableSessions)
        : MenuScreen(Resources::JoinSession)
        , availableSessions(std::move(availableSessions))
    {
        auto& entries = getMenuEntriesProperty();
        for (const AvailableNetworkSession& availableSession : this->availableSessions)
        {
            // Create menu entries for each available session.
            auto menuEntry = std::make_shared<AvailableSessionMenuEntry>(availableSession);
            menuEntry->Selected += [this](System::Object* sender, const PlayerIndexEventArgs& e)
            {
                AvailableSessionMenuEntrySelected(sender, e);
            };
            entries.push_back(std::move(menuEntry));

            // Matchmaking can return up to 25 available sessions at a time, but
            // we don't have room to fit that many on the screen. In a perfect
            // world we should make the menu scroll if there are too many, but it
            // is easier to just not bother displaying more than we have room for.
            if (static_cast<int>(entries.size()) >= MaxSearchResults)
                break;
        }

        // Add the Back menu entry.
        auto backMenuEntry = std::make_shared<MenuEntry>(Resources::Back);
        backMenuEntry->Selected += [this](System::Object* sender, const PlayerIndexEventArgs& e)
        {
            BackMenuEntrySelected(sender, e);
        };
        entries.push_back(std::move(backMenuEntry));
    }

    void JoinSessionScreen::AvailableSessionMenuEntrySelected(System::Object* sender, const PlayerIndexEventArgs&)
    {
        // Which menu entry was selected?
        auto* menuEntry = dynamic_cast<AvailableSessionMenuEntry*>(sender);
        const AvailableNetworkSession& availableSession = menuEntry->getAvailableSessionProperty();

        try
        {
            // Begin an asynchronous join network session operation.
            System::IAsyncResult* asyncResult = NetworkSession::BeginJoin(&availableSession, {}, {});

            // Activate the network busy screen, which will display
            // an animation until this operation has completed.
            auto busyScreen = std::make_shared<NetworkBusyScreen>(asyncResult);
            busyScreen->OperationCompleted += [this](System::Object* busy, const OperationCompletedEventArgs& completed)
            {
                JoinSessionOperationCompleted(busy, completed);
            };

            getScreenManagerProperty().AddScreen(std::move(busyScreen), getControllingPlayerProperty());
        }
        catch (const std::exception& exception)
        {
            getScreenManagerProperty().AddScreen(std::make_shared<NetworkErrorScreen>(exception), getControllingPlayerProperty());
        }
    }

    void JoinSessionScreen::JoinSessionOperationCompleted(System::Object*, const OperationCompletedEventArgs& e)
    {
        // C# releases the finished IAsyncResult to the collector; here the End call's owner deletes it.
        std::unique_ptr<System::IAsyncResult> asyncResult(e.getAsyncResultProperty());
        try
        {
            // End the asynchronous join network session operation.
            std::shared_ptr<NetworkSession> networkSession(NetworkSession::EndJoin(asyncResult.get()));

            // Create a component that will manage the session we just joined.
            NetworkSessionComponent::Create(getScreenManagerProperty(), networkSession);

            // Go to the lobby screen. We pass null as the controlling player,
            // because the lobby screen accepts input from all local players
            // who are in the session, not just a single controlling player.
            getScreenManagerProperty().AddScreen(std::make_shared<LobbyScreen>(networkSession), std::nullopt);

            availableSessions.Dispose();
        }
        catch (const std::exception& exception)
        {
            getScreenManagerProperty().AddScreen(std::make_shared<NetworkErrorScreen>(exception), getControllingPlayerProperty());
        }
    }

    void JoinSessionScreen::BackMenuEntrySelected(System::Object*, const PlayerIndexEventArgs&)
    {
        availableSessions.Dispose();

        ExitScreen();
    }

    const std::string& JoinSessionScreen::GetTypeName() const
    {
        static const std::string name = "NetworkStateManagement.JoinSessionScreen";
        return name;
    }
}
