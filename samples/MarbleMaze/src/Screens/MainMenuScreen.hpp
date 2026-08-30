// SPDX-License-Identifier: MS-PL
#pragma once

// MainMenuScreen.hpp — C++ port of Screens/MainMenuScreen.cs (XNA 4.0
// MarbleMaze sample).

#include <memory>

#include "../ScreenManager/MenuScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"
#include "../Misc/AudioManager.hpp"
#include "BackgroundScreen.hpp"

namespace MarbleMazeGame {

using GameStateManagement::MenuEntry;
using GameStateManagement::MenuScreen;
using GameStateManagement::PlayerIndexEventArgs;
using Microsoft::Xna::Framework::PlayerIndex;

class MainMenuScreen : public MenuScreen {
public:
    MainMenuScreen() : MenuScreen("") {
        auto startGameMenuEntry = std::make_shared<MenuEntry>("Play");
        auto highScoreMenuEntry = std::make_shared<MenuEntry>("High Score");
        auto exitMenuEntry = std::make_shared<MenuEntry>("Exit");

        startGameMenuEntry->Selected +=
            [this](System::Object*, const PlayerIndexEventArgs&) { StartGameMenuEntrySelected(); };
        highScoreMenuEntry->Selected +=
            [this](System::Object*, const PlayerIndexEventArgs&) { HighScoreMenuEntrySelected(); };
        exitMenuEntry->Selected +=
            [this](System::Object* sender, const PlayerIndexEventArgs& e) {
                MenuScreen::OnCancel(sender, e);
            };

        MenuEntries().push_back(startGameMenuEntry);
        MenuEntries().push_back(highScoreMenuEntry);
        MenuEntries().push_back(exitMenuEntry);
    }

protected:
    void OnCancel(PlayerIndex playerIndex) override;

private:
    void HighScoreMenuEntrySelected();
    void StartGameMenuEntrySelected();
};

} // namespace MarbleMazeGame
