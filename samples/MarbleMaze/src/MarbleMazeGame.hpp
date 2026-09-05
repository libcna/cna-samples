// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/DisplayOrientation.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/TimeSpan.hpp"

#include "Misc/AudioManager.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/BackgroundScreen.hpp"
#include "Screens/GameplayScreen.hpp"
#include "Screens/HighScoreScreen.hpp"
#include "Screens/LoadingAndInstructionScreen.hpp"
#include "Screens/MainMenuScreen.hpp"
#include "Screens/PauseScreen.hpp"

namespace MarbleMazeGame
{
    using GameStateManagement::ScreenManager;
    using namespace Microsoft::Xna::Framework;

    class MarbleMazeGame : public Game
    {
    public:
        MarbleMazeGame()
            : graphics_(this)
        {
            CNAEXT ScreenManager::RegisterScreenType<BackgroundScreen>();
            CNAEXT ScreenManager::RegisterScreenType<GameplayScreen>();
            CNAEXT ScreenManager::RegisterScreenType<HighScoreScreen>();
            CNAEXT ScreenManager::RegisterScreenType<LoadingAndInstructionScreen>();
            CNAEXT ScreenManager::RegisterScreenType<MainMenuScreen>();
            CNAEXT ScreenManager::RegisterScreenType<PauseScreen>();

            getContentProperty().setRootDirectoryProperty("Content");
            setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));

            screenManager_ = std::make_shared<ScreenManager>(*this);
            getComponentsProperty().Add(screenManager_.get());

            graphics_.setIsFullScreenProperty(true);
            graphics_.setSupportedOrientationsProperty(DisplayOrientation::LandscapeLeft);

            // CNAEXT — owner-approved pointer support for this otherwise touch-only Phone sample.
            // Mouse input enters the unchanged TouchPanel and gesture paths; see ../diff.md.
            CNAEXT Microsoft::Xna::Framework::Input::Touch::TouchPanel::
                setMouseTouchEmulationEnabledEXT(true);

            screenManager_->AddScreen(std::make_shared<BackgroundScreen>(), std::nullopt);
            screenManager_->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);

            AudioManager::Initialize(*this);
        }

        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name = "MarbleMazeGame.MarbleMazeGame";
            return name;
        }

    protected:
        void LoadContent() override
        {
            AudioManager::LoadSounds();
            HighScoreScreen::LoadHighscore();
            Game::LoadContent();
        }

    private:
        GraphicsDeviceManager graphics_;
        std::shared_ptr<ScreenManager> screenManager_;
    };
}
