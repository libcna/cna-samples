// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Cursor.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Ray.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace PickingSample
{
    using Microsoft::Xna::Framework::DrawableGameComponent;
    using Microsoft::Xna::Framework::Game;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Ray;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    /**
     * @brief Cursor is a DrawableGameComponent that draws a cursor on the screen.
     */
    class Cursor : public DrawableGameComponent
    {
    private:
        // this constant controls how fast the gamepad moves the cursor. this constant
        // is in pixels per second.
        static constexpr float CursorSpeed = 400.0f;

        // this spritebatch is created internally, and is used to draw the cursor.
        // XNA's SpriteBatch and Texture2D are reference types, so the original's fields are null
        // until LoadContent assigns them; std::optional is this port's established stand-in.
        std::optional<SpriteBatch> spriteBatch;

        // this is the sprite that is drawn at the current cursor position.
        // textureCenter is used to center the sprite when drawing.
        std::optional<Texture2D> cursorTexture;
        Vector2 textureCenter;

        // Position is the cursor position, and is in screen space.
        Vector2 position;

    public:
        /**
         * @brief Gets the cursor position, in screen space.
         * @return The cursor's screen-space position.
         */
        [[nodiscard]] const Vector2& getPositionProperty() const { return position; }

        /**
         * @brief Constructs the cursor.
         * @param game The game that owns this component.
         */
        explicit Cursor(Game& game);

        /**
         * @brief Returns the fully qualified logical type name of this component.
         * @return "PickingSample.Cursor".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads the cursor texture, finds its center and creates a SpriteBatch. */
        void LoadContent() override;

    public:
        /**
         * @brief Moves the cursor from whichever input the platform provides.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

    private:
        /**
         * @brief Handles input for Xbox 360.
         * @param gameTime Provides a snapshot of timing values.
         */
        void UpdateXboxInput(GameTime& gameTime);

        /** @brief Handles input for Windows. */
        void UpdateWindowsInput();

        /** @brief Handles input for Windows Phone. */
        void UpdateWindowsPhoneInput();

    public:
        /**
         * @brief Draws the cursor sprite centred on its position.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

        /**
         * @brief Calculates a world space ray starting at the camera's "eye" and pointing in the
         *        direction of the cursor.
         *
         * @param projectionMatrix The camera's projection matrix.
         * @param viewMatrix The camera's view matrix.
         * @return The ray under the cursor.
         */
        [[nodiscard]] Ray CalculateCursorRay(const Matrix& projectionMatrix,
                                             const Matrix& viewMatrix);
    };
}
