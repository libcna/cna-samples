// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "System/TimeSpan.hpp"
#include "System/Threading/Thread.hpp"

#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"

namespace MarbleMazeGame {

using GameStateManagement::GameScreen;
using GameStateManagement::InputState;
using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Input::Touch::GestureType;

class GameplayScreen;

class LoadingAndInstructionScreen : public GameScreen {
public:
    LoadingAndInstructionScreen() {
        setTransitionOnTime(System::TimeSpan::FromSeconds(0));
        setTransitionOffTime(System::TimeSpan::FromSeconds(0.5));
        setEnabledGestures(GestureType::Tap);
    }

    void LoadContent() override;
    void HandleInput(InputState& input) override;
    void Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen) override;

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        spriteBatch.Begin();

        spriteBatch.Draw(*background_, Vector2(0, 0), Color::White * TransitionAlpha());

        if (isLoading_) {
            std::string text = "Loading...";
            Vector2 size = font_->MeasureString(text);
            auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
            Vector2 position((viewport.getWidthProperty() - size.X) / 2.0f, (viewport.getHeightProperty() - size.Y) / 2.0f);
            spriteBatch.DrawString(*font_, text, position, Color::White);
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

} // namespace MarbleMazeGame
