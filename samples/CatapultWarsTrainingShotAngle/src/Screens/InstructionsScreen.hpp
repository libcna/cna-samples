// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "System/Threading/Thread.hpp"
#include "System/Threading/ThreadState.hpp"
#include "System/TimeSpan.hpp"

#include "../ScreenManager/ScreenManager.hpp"
#include "GameplayScreen.hpp"

namespace CatapultGame {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Input::Touch::GestureType;
using GameStateManagement::GameScreen;
using GameStateManagement::InputState;

class InstructionsScreen : public GameScreen {
public:
    InstructionsScreen() {
        setEnabledGestures(GestureType::Tap);
        setTransitionOnTime(System::TimeSpan::FromSeconds(0.0));
        setTransitionOffTime(System::TimeSpan::FromSeconds(0.5));
    }

    void LoadContent() override {
        background_.emplace(Load<Texture2D>("Textures/Backgrounds/instructions"));
        font_.emplace(Load<SpriteFont>("Fonts/MenuFont"));
    }

    void Update(GameTime& gameTime, bool otherScreenHasFocus,
                bool coveredByOtherScreen) override {
        if (thread_ &&
            thread_->getThreadStateProperty() == System::Threading::ThreadState::Stopped &&
            !IsExiting()) {
            isLoading_ = false;
            ExitScreen();
            GetScreenManager()->AddScreen(gameplayScreen_, std::nullopt);
        }

        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
    }

    void HandleInput(InputState& input) override {
        if (isLoading_) {
            GameScreen::HandleInput(input);
            return;
        }

        for (const auto& gesture : input.Gestures) {
            if (gesture.getGestureTypeProperty() != GestureType::Tap) continue;

            gameplayScreen_ = std::make_shared<GameplayScreen>();
            gameplayScreen_->setScreenManager(GetScreenManager());
            thread_ = std::make_unique<System::Threading::Thread>(
                [this] { gameplayScreen_->LoadAssets(); });
            isLoading_ = true;
            thread_->Start();
        }

        GameScreen::HandleInput(input);
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        spriteBatch.Begin();
        spriteBatch.Draw(
            *background_, Vector2(0.0f, 0.0f),
            Color(static_cast<SharpRuntime::bytecs>(255),
                  static_cast<SharpRuntime::bytecs>(255),
                  static_cast<SharpRuntime::bytecs>(255),
                  static_cast<SharpRuntime::bytecs>(TransitionAlpha() * 255.0f)));

        if (isLoading_) {
            const std::string text = "Loading...";
            const Vector2 size = font_->MeasureString(text);
            const auto viewport =
                GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
            const Vector2 position((viewport.getWidthProperty() - size.X) / 2.0f,
                                   (viewport.getHeightProperty() - size.Y) / 2.0f);
            spriteBatch.DrawString(*font_, text, position, Color::Black);
            spriteBatch.DrawString(*font_, text, position - Vector2(-4.0f, 4.0f),
                                   Color(255.0f, 150.0f, 0.0f));
        }
        spriteBatch.End();
    }

private:
    std::optional<Texture2D> background_;
    std::optional<SpriteFont> font_;
    bool isLoading_ = false;
    std::shared_ptr<GameplayScreen> gameplayScreen_;
    std::unique_ptr<System::Threading::Thread> thread_;
};

} // namespace CatapultGame
