// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"

namespace ColorReplacement
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief Sample showing how to change the color of select areas on a model.
     */
    class ColorReplacementGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game and builds the view matrix. */
        ColorReplacementGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "ColorReplacement.ColorReplacementGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Creates the projection matrix once the graphics device exists. */
        void Initialize() override;

        /** @brief Load your graphics content. */
        void LoadContent() override;

        /**
         * @brief Allows the game to run logic.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        /** Draws a model that uses BasicEffect or ReplaceColor.fx on its parts. */
        void DrawModel(const Matrix& world);

        /**
         * Draws the text overlay including instructions and the current target color.
         */
        void DrawOverlayText();

        /** Handles input for quitting the game. */
        void HandleInput();

        GraphicsDeviceManager graphics;

        std::unique_ptr<SpriteBatch> spriteBatch;
        // XNA's SpriteFont and Model are reference types, so the original's fields are null
        // until LoadContent runs.
        std::optional<SpriteFont> spriteFont;

        Matrix view;
        Matrix projection;

        std::optional<Model> model;

        /**
         * Desired color parts of the model will have after color replacement
         */
        Vector3 targetColor;

        /**
         * Maximum rate at which selected channels of the
         * target color are changed based on user input
         */
        static constexpr float ColorChangeRate = 0.01f;
    };
}
