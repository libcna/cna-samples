// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// FrameRateCounter.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "FrameRateCounter.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "System/Int32.hpp"
#include "System/TimeSpan.hpp"

namespace FrameRateCounterComponent
{
    using Microsoft::Xna::Framework::Color;

    FrameRateCounter::FrameRateCounter(Game& game)
        : DrawableGameComponent(game), content_(&game.getServicesProperty())
    {
    }

    const std::string& FrameRateCounter::GetTypeName() const
    {
        static const std::string name = "FrameRateCounterComponent.FrameRateCounter";
        return name;
    }

    void FrameRateCounter::LoadContent()
    {
        spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        spriteFont_.emplace(content_.Load<SpriteFont>("content\\Font"));
        fpsScreenLocation_ = Vector2(320.0f, 32.0f);
    }

    void FrameRateCounter::UnloadContent()
    {
        content_.Unload();
    }

    void FrameRateCounter::Update(GameTime& gameTime)
    {
        elapsedTime_ += gameTime.getElapsedGameTimeProperty().getTicksProperty();
        if (elapsedTime_ > System::TimeSpan::TicksPerSecond)
        {
            elapsedTime_ -= System::TimeSpan::TicksPerSecond;
            frameRate_ = frameCounter_;
            frameCounter_ = 0;
        }
    }

    void FrameRateCounter::Draw(const GameTime&)
    {
        frameCounter_++;
        const std::string fps = "fps: " + System::Int32::ToString(frameRate_);
        spriteBatch_->Begin();
        spriteBatch_->DrawString(*spriteFont_, fps, fpsScreenLocation_, Color::White);
        spriteBatch_->End();
    }
}
