// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Cat.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Animals/Animal.hpp"

namespace Flocking
{
    class InputState;

    /**
     * @brief The player-controlled animal the birds flee from.
     */
    class Cat : public Animal
    {
    protected:
        Vector2 center;

    public:
        /**
         * @brief Constructs the cat in the middle of its boundary.
         * @param tex The texture to draw it with.
         * @param screenWidth The width it clamps at.
         * @param screenHeight The height it clamps at.
         */
        Cat(Texture2D tex, int screenWidth, int screenHeight);

        /**
         * @brief Moves the cat, clamped to its boundary.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(const GameTime& gameTime) override;

        /**
         * @brief Draws the cat, unrotated.
         * @param spriteBatch An already-begun sprite batch.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(SpriteBatch& spriteBatch, const GameTime& gameTime) override;

        /**
         * @brief Takes this frame's movement from the player's input.
         * @param input The input state.
         */
        void HandleInput(const InputState& input);
    };
}
