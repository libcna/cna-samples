// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>

#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "../Players/Human.hpp"
#include "../ScreenManager/MenuScreen.hpp"
#include "../Utility/AudioManager.hpp"

namespace CatapultGame {

using Microsoft::Xna::Framework::PlayerIndex;
using Microsoft::Xna::Framework::Vector2;
using GameStateManagement::GameScreen;
using GameStateManagement::MenuEntry;
using GameStateManagement::MenuScreen;
using GameStateManagement::PlayerIndexEventArgs;

class PauseScreen : public MenuScreen {
public:
    PauseScreen(std::shared_ptr<GameScreen> backgroundScreen, Player* human, Player* computer)
        : MenuScreen(std::string()), backgroundScreen_(std::move(backgroundScreen)),
          human_(human), computer_(computer) {
        setIsPopup(true);

        auto startGameMenuEntry = std::make_shared<MenuEntry>("Return");
        auto exitMenuEntry = std::make_shared<MenuEntry>("Quit Game");
        startGameMenuEntry->Selected +=
            [this](System::Object* sender, const PlayerIndexEventArgs& args) {
                StartGameMenuEntrySelected(sender, args);
            };
        exitMenuEntry->Selected +=
            [this](System::Object* sender, const PlayerIndexEventArgs& args) {
                (void)sender;
                OnCancel(args.getPlayerIndexProperty());
            };
        MenuEntries().push_back(startGameMenuEntry);
        MenuEntries().push_back(exitMenuEntry);

        prevHumanIsActive_ = human_->getCatapultProperty()->getIsActiveProperty();
        prevComputerIsActive_ = computer_->getCatapultProperty()->getIsActiveProperty();
        human_->getCatapultProperty()->setIsActiveProperty(false);
        computer_->getCatapultProperty()->setIsActiveProperty(false);
        AudioManager::PauseResumeSounds(false);
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

    void OnCancel(PlayerIndex playerIndex) override;

private:
    void StartGameMenuEntrySelected(System::Object* sender,
                                    const PlayerIndexEventArgs& args) {
        (void)sender;
        (void)args;
        human_->getCatapultProperty()->setIsActiveProperty(prevHumanIsActive_);
        computer_->getCatapultProperty()->setIsActiveProperty(prevComputerIsActive_);

        auto* humanPlayer = dynamic_cast<Human*>(human_);
        if (!humanPlayer->getIsDraggingProperty()) {
            AudioManager::PauseResumeSounds(true);
        } else {
            humanPlayer->ResetDragState();
            AudioManager::StopSounds();
        }

        backgroundScreen_->ExitScreen();
        ExitScreen();
    }

    std::shared_ptr<GameScreen> backgroundScreen_;
    Player* human_ = nullptr;
    Player* computer_ = nullptr;
    bool prevHumanIsActive_ = false;
    bool prevComputerIsActive_ = false;
};

} // namespace CatapultGame
