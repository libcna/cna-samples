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
#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace FrameRateCounterComponent
{
    using Microsoft::Xna::Framework::DrawableGameComponent;
    using Microsoft::Xna::Framework::Game;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteFont;
    using SharpRuntime::intcs;
    using SharpRuntime::longcs;

    /** @brief Drawable game component that displays the current frame rate. */
    class FrameRateCounter final : public DrawableGameComponent
    {
    public:
        /**
         * @brief Creates the frame-rate component and its independent content manager.
         *
         * @param game Game that owns the component.
         */
        explicit FrameRateCounter(Game& game);

        /** @brief Returns the fully-qualified .NET type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Updates the one-second frame-rate sampling window.
         *
         * @param gameTime Timing state for the current update.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief Increments and draws the frame counter.
         *
         * @param gameTime Timing state for the current draw.
         */
        void Draw(const GameTime& gameTime) override;

    protected:
        /** @brief Creates the sprite batch and loads the display font. */
        void LoadContent() override;

        /** @brief Unloads content owned by this component. */
        void UnloadContent() override;

    private:
        ContentManager content_;
        std::unique_ptr<SpriteBatch> spriteBatch_;
        std::optional<SpriteFont> spriteFont_;
        Vector2 fpsScreenLocation_;
        intcs frameRate_ = 0;
        intcs frameCounter_ = 0;
        longcs elapsedTime_ = 0;
    };
}
