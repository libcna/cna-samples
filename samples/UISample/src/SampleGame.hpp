// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "System/TimeSpan.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/BackgroundScreen.hpp"
#include "Screens/HighScoreScreen.hpp"
#include "Screens/LevelSelectScreen.hpp"
#include "Screens/MainMenuScreen.hpp"
#include "Screens/SingleControlScreen.hpp"

namespace UserInterfaceSample {

class SampleGame final : public Microsoft::Xna::Framework::Game {
public:
    SampleGame() : graphics_(this) {
        getContentProperty().setRootDirectoryProperty("Content");
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
        graphics_.setIsFullScreenProperty(true);

        CNAEXT ScreenManager::RegisterScreenType<BackgroundScreen>();
        CNAEXT ScreenManager::RegisterScreenType<HighScoreScreen>();
        CNAEXT ScreenManager::RegisterScreenType<LevelSelectScreen>();
        CNAEXT ScreenManager::RegisterScreenType<MainMenuScreen>();
        CNAEXT ScreenManager::RegisterScreenType<SingleControlScreen>();

        screenManager_ = std::make_unique<ScreenManager>(*this);
        getComponentsProperty().Add(screenManager_.get());

        if (!screenManager_->DeserializeState()) {
            screenManager_->AddScreen(std::make_shared<BackgroundScreen>(), std::nullopt);
            screenManager_->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
        }
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "UserInterfaceSample.SampleGame";
        return name;
    }

protected:
    void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override {
        getGraphicsDeviceProperty().Clear(Microsoft::Xna::Framework::Color::Black);
        Game::Draw(gameTime);
    }

private:
    Microsoft::Xna::Framework::GraphicsDeviceManager graphics_;
    std::unique_ptr<ScreenManager> screenManager_;
};

} // namespace UserInterfaceSample
