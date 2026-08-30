// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// FrameRateCounter.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"

namespace CpuSkinningDemo
{
    /** @brief Drawable component that displays the current and average frame length. */
    class FrameRateCounter : public Microsoft::Xna::Framework::DrawableGameComponent
    {
    public:
        /**
         * @brief Constructs a frame-rate counter with its own Content manager.
         * @param game Game that owns the component.
         */
        explicit FrameRateCounter(Microsoft::Xna::Framework::Game& game);

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `CpuSkinningDemo.FrameRateCounter`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Counts one rendered frame, updates the once-per-second text, and draws it.
         * @param gameTime Timing information for the current frame.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    protected:
        /** @brief Creates the SpriteBatch and loads the sample font. */
        void LoadContent() override;

        /** @brief Unloads content owned by this component. */
        void UnloadContent() override;

    private:
        Microsoft::Xna::Framework::Content::ContentManager content;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> spriteFont;

        Microsoft::Xna::Framework::Vector2 fpsScreenLocation{32.0f, 32.0f};
        int frameRate = 0;
        int frameCounter = 0;
        float elapsedTime = 0.0f;
        std::string fpsString = "fps: ??";
    };
}
