// SPDX-License-Identifier: MS-PL

#include "Screens/MainMenuScreen.hpp"

#include <memory>
#include <vector>

#include "ScreenManager/ScreenManager.hpp"
#include "Screens/GameplayScreen.hpp"
#include "Screens/LoadingScreen.hpp"
#include "Screens/MessageBoxScreen.hpp"
#include "Screens/OptionsMenuScreen.hpp"

namespace GameStateManagement
{
    MainMenuScreen::MainMenuScreen()
        : MenuScreen("Main Menu")
    {
        auto playGameMenuEntry = std::make_shared<MenuEntry>("Play Game");
        auto optionsMenuEntry = std::make_shared<MenuEntry>("Options");
        auto exitMenuEntry = std::make_shared<MenuEntry>("Exit");

        playGameMenuEntry->Selected += [this](System::Object* sender,
                                               const PlayerIndexEventArgs& e)
        {
            PlayGameMenuEntrySelected(sender, e);
        };
        optionsMenuEntry->Selected += [this](System::Object* sender,
                                              const PlayerIndexEventArgs& e)
        {
            OptionsMenuEntrySelected(sender, e);
        };
        exitMenuEntry->Selected += [this](System::Object* sender,
                                           const PlayerIndexEventArgs& e)
        {
            MenuScreen::OnCancel(sender, e);
        };

        auto& entries = getMenuEntriesProperty();
        entries.push_back(std::move(playGameMenuEntry));
        entries.push_back(std::move(optionsMenuEntry));
        entries.push_back(std::move(exitMenuEntry));
    }

    void MainMenuScreen::PlayGameMenuEntrySelected(System::Object*,
                                                    const PlayerIndexEventArgs& e)
    {
        std::vector<std::shared_ptr<GameScreen>> screens;
        screens.push_back(std::make_shared<GameplayScreen>());
        LoadingScreen::Load(getScreenManagerProperty(), true, e.getPlayerIndexProperty(),
                            std::move(screens));
    }

    void MainMenuScreen::OptionsMenuEntrySelected(System::Object*,
                                                   const PlayerIndexEventArgs& e)
    {
        getScreenManagerProperty().AddScreen(std::make_shared<OptionsMenuScreen>(),
                                             e.getPlayerIndexProperty());
    }

    void MainMenuScreen::OnCancel(Microsoft::Xna::Framework::PlayerIndex playerIndex)
    {
        auto confirmExitMessageBox = std::make_shared<MessageBoxScreen>(
            "Are you sure you want to exit this sample?");
        confirmExitMessageBox->Accepted += [this](System::Object* sender,
                                                   const PlayerIndexEventArgs& e)
        {
            ConfirmExitMessageBoxAccepted(sender, e);
        };
        getScreenManagerProperty().AddScreen(std::move(confirmExitMessageBox), playerIndex);
    }

    void MainMenuScreen::ConfirmExitMessageBoxAccepted(System::Object*,
                                                        const PlayerIndexEventArgs&)
    {
        getScreenManagerProperty().getGameProperty().Exit();
    }

    const std::string& MainMenuScreen::GetTypeName() const
    {
        static const std::string name = "GameStateManagement.MainMenuScreen";
        return name;
    }
}
