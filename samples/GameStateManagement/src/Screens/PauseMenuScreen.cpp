// SPDX-License-Identifier: MS-PL

#include "Screens/PauseMenuScreen.hpp"

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "ScreenManager/ScreenManager.hpp"
#include "Screens/BackgroundScreen.hpp"
#include "Screens/LoadingScreen.hpp"
#include "Screens/MainMenuScreen.hpp"
#include "Screens/MessageBoxScreen.hpp"

namespace GameStateManagement
{
    PauseMenuScreen::PauseMenuScreen()
        : MenuScreen("Paused")
    {
        auto resumeGameMenuEntry = std::make_shared<MenuEntry>("Resume Game");
        auto quitGameMenuEntry = std::make_shared<MenuEntry>("Quit Game");

        resumeGameMenuEntry->Selected += [this](System::Object* sender,
                                                 const PlayerIndexEventArgs& e)
        {
            MenuScreen::OnCancel(sender, e);
        };
        quitGameMenuEntry->Selected += [this](System::Object* sender,
                                               const PlayerIndexEventArgs& e)
        {
            QuitGameMenuEntrySelected(sender, e);
        };

        auto& entries = getMenuEntriesProperty();
        entries.push_back(std::move(resumeGameMenuEntry));
        entries.push_back(std::move(quitGameMenuEntry));
    }

    void PauseMenuScreen::QuitGameMenuEntrySelected(System::Object*,
                                                     const PlayerIndexEventArgs&)
    {
        auto confirmQuitMessageBox = std::make_shared<MessageBoxScreen>(
            "Are you sure you want to quit this game?");
        confirmQuitMessageBox->Accepted += [this](System::Object* sender,
                                                   const PlayerIndexEventArgs& e)
        {
            ConfirmQuitMessageBoxAccepted(sender, e);
        };
        getScreenManagerProperty().AddScreen(std::move(confirmQuitMessageBox),
                                             getControllingPlayerProperty());
    }

    void PauseMenuScreen::ConfirmQuitMessageBoxAccepted(System::Object*,
                                                         const PlayerIndexEventArgs&)
    {
        std::vector<std::shared_ptr<GameScreen>> screens;
        screens.push_back(std::make_shared<BackgroundScreen>());
        screens.push_back(std::make_shared<MainMenuScreen>());
        LoadingScreen::Load(getScreenManagerProperty(), false, std::nullopt,
                            std::move(screens));
    }

    const std::string& PauseMenuScreen::GetTypeName() const
    {
        static const std::string name = "GameStateManagement.PauseMenuScreen";
        return name;
    }
}
