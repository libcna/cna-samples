// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "System/TimeSpan.hpp"

#include "Misc/AudioManager.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/BackgroundScreen.hpp"
#include "Screens/HighScoreScreen.hpp"
#include "Screens/MainMenuScreen.hpp"
#include "Screens/GameplayScreen.hpp"
#include "Screens/LoadingAndInstructionScreen.hpp"
#include "Screens/PauseScreen.hpp"

namespace HoneycombRush {

using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GraphicsDeviceManager;
using Microsoft::Xna::Framework::PlayerIndex;

class HoneycombRush final : public Game {
public:
    HoneycombRush() {
        AudioManager::Initialize(*this);
        graphics_ = std::make_unique<GraphicsDeviceManager>(this);
        getContentProperty().setRootDirectoryProperty("Content");
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
        graphics_->setIsFullScreenProperty(true);

        ScreenManager::RegisterScreenType<MainMenuScreen>();
        ScreenManager::RegisterScreenType<HighScoreScreen>();
        ScreenManager::RegisterScreenType<LoadingAndInstructionScreen>();
        ScreenManager::RegisterScreenType<PauseScreen>();

        screenManager_ = std::make_shared<ScreenManager>(*this);
        screenManager_->AddScreen(std::make_shared<BackgroundScreen>("titleScreen"), std::nullopt);
        screenManager_->AddScreen(std::make_shared<MainMenuScreen>(), PlayerIndex::One);
        getComponentsProperty().Add(screenManager_.get());
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "HoneycombRush.HoneycombRush";
        return name;
    }

protected:
    void LoadContent() override {
        HighScoreScreen::LoadHighscores();
        Game::LoadContent();
    }

private:
    std::unique_ptr<GraphicsDeviceManager> graphics_;
    std::shared_ptr<ScreenManager> screenManager_;
};

} // namespace HoneycombRush
