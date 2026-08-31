// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/TimeSpan.hpp"

#include "../ScreenManager/ScreenManager.hpp"

namespace CatapultGame {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using GameStateManagement::GameScreen;

class BackgroundScreen : public GameScreen {
public:
    BackgroundScreen() {
        setTransitionOnTime(System::TimeSpan::FromSeconds(0.0));
        setTransitionOffTime(System::TimeSpan::FromSeconds(0.5));
    }

    void LoadContent() override {
        background_.emplace(Load<Texture2D>("Textures/Backgrounds/title_screen"));
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
        spriteBatch.End();
    }

private:
    std::optional<Texture2D> background_;
};

} // namespace CatapultGame
