// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PauseMenuScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Screens/PauseMenuScreen.hpp"

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionState.hpp"
#include "Networking/NetworkSessionComponent.hpp"
#include "Resources.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/BackgroundScreen.hpp"
#include "Screens/LoadingScreen.hpp"
#include "Screens/MainMenuScreen.hpp"
#include "Screens/MessageBoxScreen.hpp"

namespace NetworkStateManagement
{
    using Microsoft::Xna::Framework::Net::NetworkSession;
    using Microsoft::Xna::Framework::Net::NetworkSessionState;

    PauseMenuScreen::PauseMenuScreen(std::shared_ptr<NetworkSession> networkSession)
        : MenuScreen(Resources::Paused)
        , networkSession(std::move(networkSession))
    {
        auto& entries = getMenuEntriesProperty();

        // Add the Resume Game menu entry.
        auto resumeGameMenuEntry = std::make_shared<MenuEntry>(Resources::ResumeGame);
        resumeGameMenuEntry->Selected += [this](System::Object* sender, const PlayerIndexEventArgs& e)
        {
            MenuScreen::OnCancel(sender, e);
        };
        entries.push_back(std::move(resumeGameMenuEntry));

        if (this->networkSession == nullptr)
        {
            // If this is a single player game, add the Quit menu entry.
            auto quitGameMenuEntry = std::make_shared<MenuEntry>(Resources::QuitGame);
            quitGameMenuEntry->Selected += [this](System::Object* sender, const PlayerIndexEventArgs& e)
            {
                QuitGameMenuEntrySelected(sender, e);
            };
            entries.push_back(std::move(quitGameMenuEntry));
        }
        else
        {
            // If we are hosting a network game, add the Return to Lobby menu entry.
            if (this->networkSession->getIsHostProperty())
            {
                auto lobbyMenuEntry = std::make_shared<MenuEntry>(Resources::ReturnToLobby);
                lobbyMenuEntry->Selected += [this](System::Object* sender, const PlayerIndexEventArgs& e)
                {
                    ReturnToLobbyMenuEntrySelected(sender, e);
                };
                entries.push_back(std::move(lobbyMenuEntry));
            }

            // Add the End/Leave Session menu entry.
            const std::string leaveEntryText = this->networkSession->getIsHostProperty() ? Resources::EndSession
                                                                                  : Resources::LeaveSession;

            auto leaveSessionMenuEntry = std::make_shared<MenuEntry>(leaveEntryText);
            leaveSessionMenuEntry->Selected += [this](System::Object* sender, const PlayerIndexEventArgs& e)
            {
                LeaveSessionMenuEntrySelected(sender, e);
            };
            entries.push_back(std::move(leaveSessionMenuEntry));
        }
    }

    void PauseMenuScreen::QuitGameMenuEntrySelected(System::Object*, const PlayerIndexEventArgs&)
    {
        auto confirmQuitMessageBox = std::make_shared<MessageBoxScreen>(Resources::ConfirmQuitGame);
        confirmQuitMessageBox->Accepted += [this](System::Object* sender, const PlayerIndexEventArgs& e)
        {
            ConfirmQuitMessageBoxAccepted(sender, e);
        };
        getScreenManagerProperty().AddScreen(std::move(confirmQuitMessageBox), getControllingPlayerProperty());
    }

    void PauseMenuScreen::ConfirmQuitMessageBoxAccepted(System::Object*, const PlayerIndexEventArgs&)
    {
        std::vector<std::shared_ptr<GameScreen>> screens;
        screens.push_back(std::make_shared<BackgroundScreen>());
        screens.push_back(std::make_shared<MainMenuScreen>());
        LoadingScreen::Load(getScreenManagerProperty(), false, std::nullopt, std::move(screens));
    }

    void PauseMenuScreen::ReturnToLobbyMenuEntrySelected(System::Object*, const PlayerIndexEventArgs&)
    {
        if (networkSession->getSessionStateProperty() == NetworkSessionState::Playing)
        {
            networkSession->EndGame();
        }
    }

    void PauseMenuScreen::LeaveSessionMenuEntrySelected(System::Object*, const PlayerIndexEventArgs& e)
    {
        NetworkSessionComponent::LeaveSession(getScreenManagerProperty(), e.getPlayerIndexProperty());
    }

    const std::string& PauseMenuScreen::GetTypeName() const
    {
        static const std::string name = "NetworkStateManagement.PauseMenuScreen";
        return name;
    }
}
