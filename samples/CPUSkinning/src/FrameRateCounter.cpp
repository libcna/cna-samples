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
#include "System/String.hpp"

namespace CpuSkinningDemo
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Game;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteFont;

    FrameRateCounter::FrameRateCounter(Game& game)
        : DrawableGameComponent(game)
        , content(&game.getServicesProperty(), "Content")
    {
    }

    const std::string& FrameRateCounter::GetTypeName() const
    {
        static const std::string name = "CpuSkinningDemo.FrameRateCounter";
        return name;
    }

    void FrameRateCounter::LoadContent()
    {
        spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        spriteFont.emplace(content.Load<SpriteFont>("font"));
    }

    void FrameRateCounter::UnloadContent()
    {
        content.Unload();
    }

    void FrameRateCounter::Draw(const GameTime& gameTime)
    {
        ++frameCounter;
        elapsedTime += static_cast<float>(
            gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());
        if (elapsedTime >= 1.0f)
        {
            elapsedTime -= 1.0f;
            frameRate = frameCounter;
            frameCounter = 0;

            const float averageFrameLength = 1000.0f / static_cast<float>(frameRate);
            fpsString = System::String::Format(
                "fps: {0} ({1} ms)", frameRate, averageFrameLength);
        }

        spriteBatch->Begin();
        spriteBatch->DrawString(*spriteFont, fpsString, fpsScreenLocation, Color::White);
        spriteBatch->End();
    }
}
