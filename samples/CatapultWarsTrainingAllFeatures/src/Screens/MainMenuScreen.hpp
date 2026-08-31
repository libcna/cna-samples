// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>

#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "../ScreenManager/MenuScreen.hpp"
#include "InstructionsScreen.hpp"

namespace CatapultGame {

using Microsoft::Xna::Framework::PlayerIndex;
using Microsoft::Xna::Framework::Vector2;
using GameStateManagement::MenuEntry;
using GameStateManagement::MenuScreen;
using GameStateManagement::PlayerIndexEventArgs;

class MainMenuScreen : public MenuScreen {
public:
    MainMenuScreen() : MenuScreen(std::string()) {
        setIsPopup(true);

        auto startSinglePlayerGameMenuEntry = std::make_shared<MenuEntry>("Play VS Phone");
        auto startTwoPlayersGameMenuEntry = std::make_shared<MenuEntry>("Play VS Human");
        auto exitMenuEntry = std::make_shared<MenuEntry>("Exit");
        startSinglePlayerGameMenuEntry->Selected +=
            [this](System::Object* sender, const PlayerIndexEventArgs& args) {
                StartSinglePlayerGameMenuEntrySelected(sender, args);
            };
        startTwoPlayersGameMenuEntry->Selected +=
            [this](System::Object* sender, const PlayerIndexEventArgs& args) {
                StartTwoPlayerGameMenuEntrySelected(sender, args);
            };
        exitMenuEntry->Selected +=
            [this](System::Object* sender, const PlayerIndexEventArgs& args) {
                MenuScreen::OnCancel(sender, args);
            };
        MenuEntries().push_back(startSinglePlayerGameMenuEntry);
        MenuEntries().push_back(startTwoPlayersGameMenuEntry);
        MenuEntries().push_back(exitMenuEntry);
    }

protected:
    void UpdateMenuEntryLocations() override {
        MenuScreen::UpdateMenuEntryLocations();
        for (const auto& entry : MenuEntries()) {
            Vector2 position = entry->Position();
            position.Y += 60.0f;
            entry->setPosition(position);
        }
    }

    void OnCancel(PlayerIndex playerIndex) override {
        (void)playerIndex;
        GetScreenManager()->getGameProperty().Exit();
    }

private:
    void StartSinglePlayerGameMenuEntrySelected(System::Object* sender,
                                                const PlayerIndexEventArgs& args) {
        (void)sender;
        (void)args;
        GetScreenManager()->AddScreen(std::make_shared<InstructionsScreen>(false), std::nullopt);
    }

    void StartTwoPlayerGameMenuEntrySelected(System::Object* sender,
                                             const PlayerIndexEventArgs& args) {
        (void)sender;
        (void)args;
        GetScreenManager()->AddScreen(std::make_shared<InstructionsScreen>(true), std::nullopt);
    }
};

} // namespace CatapultGame
