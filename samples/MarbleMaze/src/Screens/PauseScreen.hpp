// SPDX-License-Identifier: MS-PL
#pragma once

// PauseScreen.hpp — C++ port of Screens/PauseScreen.cs (XNA 4.0 MarbleMaze
// sample).

#include <memory>

#include "../ScreenManager/MenuScreen.hpp"
#include "../Misc/AudioManager.hpp"
#include "BackgroundScreen.hpp"

namespace MarbleMazeGame {

using GameStateManagement::MenuEntry;
using GameStateManagement::MenuScreen;
using GameStateManagement::PlayerIndexEventArgs;
using Microsoft::Xna::Framework::PlayerIndex;

class PauseScreen : public MenuScreen {
public:
    PauseScreen() : MenuScreen("Game Paused") {
        auto returnGameMenuEntry = std::make_shared<MenuEntry>("Return");
        auto restartGameMenuEntry = std::make_shared<MenuEntry>("Restart");
        auto exitMenuEntry = std::make_shared<MenuEntry>("Quit Game");

        returnGameMenuEntry->Selected +=
            [this](System::Object*, const PlayerIndexEventArgs&) { ReturnGameMenuEntrySelected(); };
        restartGameMenuEntry->Selected +=
            [this](System::Object*, const PlayerIndexEventArgs&) { RestartGameMenuEntrySelected(); };
        exitMenuEntry->Selected +=
            [this](System::Object* sender, const PlayerIndexEventArgs& e) {
                MenuScreen::OnCancel(sender, e);
            };

        MenuEntries().push_back(returnGameMenuEntry);
        MenuEntries().push_back(restartGameMenuEntry);
        MenuEntries().push_back(exitMenuEntry);
    }

protected:
    void OnCancel(PlayerIndex playerIndex) override;

private:
    void ReturnGameMenuEntrySelected();
    void RestartGameMenuEntrySelected();
};

} // namespace MarbleMazeGame
