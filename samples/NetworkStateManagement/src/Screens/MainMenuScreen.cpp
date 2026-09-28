// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// MainMenuScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Screens/MainMenuScreen.hpp"

#include <memory>
#include <vector>

#include "Networking/CreateOrFindSessionScreen.hpp"
#include "Networking/ProfileSignInScreen.hpp"
#include "Resources.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/GameplayScreen.hpp"
#include "Screens/LoadingScreen.hpp"
#include "Screens/MessageBoxScreen.hpp"

namespace NetworkStateManagement
{
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Net::NetworkSessionType;

    MainMenuScreen::MainMenuScreen()
        : MenuScreen(Resources::MainMenu)
    {
        // Create our menu entries.
        auto singlePlayerMenuEntry = std::make_shared<MenuEntry>(Resources::SinglePlayer);
        auto liveMenuEntry = std::make_shared<MenuEntry>(Resources::PlayerMatch);
        auto systemLinkMenuEntry = std::make_shared<MenuEntry>(Resources::SystemLink);
        auto exitMenuEntry = std::make_shared<MenuEntry>(Resources::Exit);

        // Hook up menu event handlers.
        singlePlayerMenuEntry->Selected += [this](System::Object* sender, const PlayerIndexEventArgs& e)
        {
            SinglePlayerMenuEntrySelected(sender, e);
        };
        liveMenuEntry->Selected += [this](System::Object* sender, const PlayerIndexEventArgs& e)
        {
            LiveMenuEntrySelected(sender, e);
        };
        systemLinkMenuEntry->Selected += [this](System::Object* sender, const PlayerIndexEventArgs& e)
        {
            SystemLinkMenuEntrySelected(sender, e);
        };
        exitMenuEntry->Selected += [this](System::Object* sender, const PlayerIndexEventArgs& e)
        {
            MenuScreen::OnCancel(sender, e);
        };

        // Add entries to the menu.
        auto& entries = getMenuEntriesProperty();
        entries.push_back(std::move(singlePlayerMenuEntry));
        entries.push_back(std::move(liveMenuEntry));
        entries.push_back(std::move(systemLinkMenuEntry));
        entries.push_back(std::move(exitMenuEntry));
    }

    void MainMenuScreen::SinglePlayerMenuEntrySelected(System::Object*, const PlayerIndexEventArgs& e)
    {
        std::vector<std::shared_ptr<GameScreen>> screens;
        screens.push_back(std::make_shared<GameplayScreen>(nullptr));
        LoadingScreen::Load(getScreenManagerProperty(), true, e.getPlayerIndexProperty(), std::move(screens));
    }

    void MainMenuScreen::LiveMenuEntrySelected(System::Object*, const PlayerIndexEventArgs& e)
    {
        CreateOrFindSession(NetworkSessionType::PlayerMatch, e.getPlayerIndexProperty());
    }

    void MainMenuScreen::SystemLinkMenuEntrySelected(System::Object*, const PlayerIndexEventArgs& e)
    {
        CreateOrFindSession(NetworkSessionType::SystemLink, e.getPlayerIndexProperty());
    }

    // Helper method shared by the Live and System Link menu event handlers.
    void MainMenuScreen::CreateOrFindSession(NetworkSessionType sessionType, PlayerIndex playerIndex)
    {
        // First, we need to make sure a suitable gamer profile is signed in.
        auto profileSignIn = std::make_shared<ProfileSignInScreen>(sessionType);

        // Hook up an event so once the ProfileSignInScreen is happy,
        // it will activate the CreateOrFindSessionScreen.
        profileSignIn->ProfileSignedIn += [this, sessionType, playerIndex](System::Object*, const System::EventArgs&)
        {
            getScreenManagerProperty().AddScreen(std::make_shared<CreateOrFindSessionScreen>(sessionType), playerIndex);
        };

        // Activate the ProfileSignInScreen.
        getScreenManagerProperty().AddScreen(std::move(profileSignIn), playerIndex);
    }

    void MainMenuScreen::OnCancel(PlayerIndex playerIndex)
    {
        auto confirmExitMessageBox = std::make_shared<MessageBoxScreen>(Resources::ConfirmExitSample);
        confirmExitMessageBox->Accepted += [this](System::Object* sender, const PlayerIndexEventArgs& e)
        {
            ConfirmExitMessageBoxAccepted(sender, e);
        };
        getScreenManagerProperty().AddScreen(std::move(confirmExitMessageBox), playerIndex);
    }

    void MainMenuScreen::ConfirmExitMessageBoxAccepted(System::Object*, const PlayerIndexEventArgs&)
    {
        getScreenManagerProperty().getGameProperty().Exit();
    }

    const std::string& MainMenuScreen::GetTypeName() const
    {
        static const std::string name = "NetworkStateManagement.MainMenuScreen";
        return name;
    }
}
