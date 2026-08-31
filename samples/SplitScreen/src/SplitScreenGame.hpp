// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SplitScreenGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Tank.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"

namespace SplitScreenSample
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GraphicsDeviceManager;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::Texture2D;
    using Microsoft::Xna::Framework::Graphics::Viewport;

    /** @brief Demonstrates rendering one animated scene through two independent viewports. */
    class SplitScreenGame : public Microsoft::Xna::Framework::Game
    {
    private:
        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;
        std::optional<Texture2D> blank;

        Viewport playerOneViewport;
        Viewport playerTwoViewport;

        Matrix playerOneView;
        Matrix playerOneProjection;
        Matrix playerTwoView;
        Matrix playerTwoProjection;

        Tank tank;

    public:
        /** @brief Creates the graphics manager and selects the original Content root. */
        SplitScreenGame();

        /**
         * @brief Returns the fully qualified logical type name of the sample game.
         * @return `SplitScreenSample.SplitScreenGame`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads the tank and creates both viewports, cameras and divider texture. */
        void LoadContent() override;

        /**
         * @brief Animates the tank and rotates the second camera.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief Draws the scene in both viewports, then overlays their borders.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        /**
         * @brief Draws the full scene through one viewport and restores the previous viewport.
         * @param gameTime Provides a snapshot of timing values.
         * @param viewport The viewport to activate for this scene pass.
         * @param view The view matrix for this pass.
         * @param projection The projection matrix for this pass.
         */
        void DrawScene(const GameTime& gameTime, const Viewport& viewport,
                       const Matrix& view, const Matrix& projection);

        /**
         * @brief Draws the four two-pixel black edges of a viewport.
         * @param viewport The viewport whose boundary is drawn.
         */
        void DrawViewportEdges(const Viewport& viewport);
    };
}
