// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Ship.cs
//
// Microsoft Advanced Technology Group
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace TouchThumbsticks
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteEffects;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    /** @brief Base class for ships in the game. */
    class Ship
    {
    private:
        Texture2D texture;

    public:
        /** @brief Position of the ship in world coordinates. */
        Vector2 Position;

        /** @brief Velocity of the ship. */
        Vector2 Velocity;

        /** @brief Rotation of the ship in radians. */
        float Rotation = 0.0f;

        /**
         * @brief Creates a ship using the supplied texture.
         *
         * @param textureValue Texture used to draw the ship.
         */
        explicit Ship(const Texture2D& textureValue)
            : texture(textureValue)
        {
        }

        /** @brief Destroys the polymorphic ship. */
        virtual ~Ship() = 0;

        /**
         * @brief Tests whether the ship contains a point.
         *
         * @param point Point to test.
         * @return `true` when the ship contains the point; otherwise `false`.
         */
        [[nodiscard]] virtual bool ContainsPoint(const Vector2& point) const
        {
            static_cast<void>(point);
            return false;
        }

        /**
         * @brief Allows the ship to update.
         *
         * @param gameTime Current game timing snapshot.
         */
        virtual void Update(const GameTime& gameTime)
        {
            static_cast<void>(gameTime);
        }

        /**
         * @brief Draws the ship.
         *
         * @param spriteBatch Sprite batch used for drawing.
         */
        virtual void Draw(SpriteBatch& spriteBatch) const
        {
            spriteBatch.Draw(
                texture,
                Position,
                std::optional<Rectangle>(std::nullopt),
                Color::White,
                Rotation,
                Vector2(
                    static_cast<float>(texture.getWidthProperty()) / 2.0f,
                    static_cast<float>(texture.getHeightProperty()) / 2.0f),
                1.0f,
                SpriteEffects::None,
                0.0f);
        }
    };

    inline Ship::~Ship() = default;
}
