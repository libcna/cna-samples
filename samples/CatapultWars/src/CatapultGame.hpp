// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/TimeSpan.hpp"

#include "ScreenManager/ScreenManager.hpp"
#include "Screens/BackgroundScreen.hpp"
#include "Screens/GameplayScreen.hpp"
#include "Screens/InstructionsScreen.hpp"
#include "Screens/MainMenuScreen.hpp"
#include "Utility/AudioManager.hpp"

namespace CatapultGame {

using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::GraphicsDeviceManager;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;
using GameStateManagement::ScreenManager;

class CatapultGame : public Game {
public:
    CatapultGame() {
        graphics_ = std::make_unique<GraphicsDeviceManager>(this);
        getContentProperty().setRootDirectoryProperty("Content");
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));

        CNAEXT ScreenManager::RegisterScreenType<BackgroundScreen>();
        CNAEXT ScreenManager::RegisterScreenType<GameplayScreen>();
        CNAEXT ScreenManager::RegisterScreenType<InstructionsScreen>();
        CNAEXT ScreenManager::RegisterScreenType<MainMenuScreen>();

        screenManager_ = std::make_unique<ScreenManager>(*this);
        getComponentsProperty().Add(screenManager_.get());

        graphics_->setIsFullScreenProperty(true);

        // CNAEXT: this remains a touch-only phone game; desktop mouse events enter the ordinary
        // TouchPanel gesture path instead of changing InputState. Owner-requested, see diff.md.
        CNAEXT TouchPanel::setMouseTouchEmulationEnabledEXT(true);

        screenManager_->AddScreen(std::make_shared<BackgroundScreen>(), std::nullopt);
        screenManager_->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);

        AudioManager::Initialize(*this);
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CatapultGame.CatapultGame";
        return name;
    }

protected:
    void Initialize() override { Game::Initialize(); }

    void LoadContent() override {
        AudioManager::LoadSounds();
        Game::LoadContent();
    }

private:
    std::unique_ptr<GraphicsDeviceManager> graphics_;
    std::unique_ptr<ScreenManager> screenManager_;
};

} // namespace CatapultGame
