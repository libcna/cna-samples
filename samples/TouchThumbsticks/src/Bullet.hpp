// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Bullet.cs
//
// Microsoft Advanced Technology Group
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <cmath>
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace TouchThumbsticks
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteEffects;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    /** @brief A single bullet in the game. */
    class Bullet
    {
    private:
        const float rotation;
        Vector2 velocity;
        Color color;

    public:
        /** @brief Texture shared by all bullets and assigned by the game. */
        inline static Texture2D Texture;

        /** @brief Position of the bullet in world coordinates. */
        Vector2 Position;

        /**
         * @brief Creates a bullet at the supplied position and velocity.
         *
         * @param pos Starting position of the bullet.
         * @param vel Velocity of the bullet.
         * @param col Color used to draw the bullet.
         */
        Bullet(const Vector2& pos, const Vector2& vel, const Color& col)
            : rotation(std::atan2(vel.Y, vel.X)),
              velocity(vel),
              color(col),
              Position(pos)
        {
        }

        /** @brief Moves the bullet along its velocity. */
        void Update()
        {
            Position += velocity;
        }

        /**
         * @brief Draws the bullet.
         *
         * @param spriteBatch Sprite batch used for drawing.
         */
        void Draw(SpriteBatch& spriteBatch) const
        {
            spriteBatch.Draw(
                Texture,
                Position,
                std::optional<Rectangle>(std::nullopt),
                color,
                rotation,
                Vector2(
                    static_cast<float>(Texture.getWidthProperty()) / 2.0f,
                    static_cast<float>(Texture.getHeightProperty()) / 2.0f),
                1.0f,
                SpriteEffects::None,
                0.0f);
        }
    };
}
