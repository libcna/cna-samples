// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// EnemyShip.cs
//
// Microsoft Advanced Technology Group
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <cmath>

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Ship.hpp"

namespace TouchThumbsticks
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    /** @brief Enemy ship that flies toward the player. */
    class EnemyShip : public Ship
    {
    private:
        float radius;

    public:
        /** @brief Non-owning player ship targeted by this enemy. */
        Ship* Player = nullptr;

        /**
         * @brief Creates an enemy ship.
         *
         * @param texture Texture used to draw the enemy.
         */
        explicit EnemyShip(const Texture2D& texture)
            : Ship(texture),
              radius(std::sqrt(static_cast<float>(
                         texture.getWidthProperty() * texture.getWidthProperty() +
                         texture.getHeightProperty() * texture.getHeightProperty())) *
                     0.75f)
        {
        }

        /**
         * @brief Turns and moves toward the player at constant speed.
         *
         * @param gameTime Current game timing snapshot.
         */
        void Update(const GameTime& gameTime) override
        {
            static_cast<void>(gameTime);
            const Vector2 d = Vector2::Normalize(Player->Position - Position);
            Rotation = std::atan2(d.Y, d.X);
            Position += d * 4.0f;
        }

        /**
         * @brief Tests a point against the enemy's collision radius.
         *
         * @param point Point to test.
         * @return `true` when the point lies within the collision radius.
         */
        [[nodiscard]] bool ContainsPoint(const Vector2& point) const override
        {
            return Vector2::Distance(Position, point) < radius;
        }
    };
}
