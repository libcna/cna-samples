// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PlayerShip.cs
//
// Microsoft Advanced Technology Group
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <cmath>
#include <cstddef>
#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/TimeSpan.hpp"

#include "Bullet.hpp"
#include "Ship.hpp"
#include "VirtualThumbsticks.hpp"

namespace TouchThumbsticks
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    /** @brief Player ship controlled by the two virtual thumbsticks. */
    class PlayerShip : public Ship
    {
    private:
        static constexpr float acceleration = 0.75f;
        static constexpr float bulletSpeed = 20.0f;
        inline static const System::TimeSpan cooldown =
            System::TimeSpan::FromSeconds(0.15);

        System::TimeSpan fireTimer;

        void ClampPlayerShip()
        {
            if (Position.X < -WorldWidth / 2.0f)
            {
                Position.X = -WorldWidth / 2.0f;
                if (Velocity.X < 0.0f)
                    Velocity.X = 0.0f;
            }

            if (Position.X > WorldWidth / 2.0f)
            {
                Position.X = WorldWidth / 2.0f;
                if (Velocity.X > 0.0f)
                    Velocity.X = 0.0f;
            }

            if (Position.Y < -WorldHeight / 2.0f)
            {
                Position.Y = -WorldHeight / 2.0f;
                if (Velocity.Y < 0.0f)
                    Velocity.Y = 0.0f;
            }

            if (Position.Y > WorldHeight / 2.0f)
            {
                Position.Y = WorldHeight / 2.0f;
                if (Velocity.Y > 0.0f)
                    Velocity.Y = 0.0f;
            }
        }

    public:
        /** @brief Active bullets fired by the player. */
        std::vector<std::unique_ptr<Bullet>> Bullets;

        /** @brief Width of the world used to constrain the player and bullets. */
        int WorldWidth = 0;

        /** @brief Height of the world used to constrain the player and bullets. */
        int WorldHeight = 0;

        /**
         * @brief Creates the player ship.
         *
         * @param texture Texture used to draw the player.
         */
        explicit PlayerShip(const Texture2D& texture)
            : Ship(texture)
        {
        }

        /**
         * @brief Applies movement, aiming, firing and world constraints.
         *
         * @param gameTime Current game timing snapshot.
         */
        void Update(const GameTime& gameTime) override
        {
            Velocity += VirtualThumbsticks::getLeftThumbstickProperty() * acceleration;
            Position += Velocity;
            Velocity *= 0.98f;

            fireTimer -= gameTime.getElapsedGameTimeProperty();

            if (VirtualThumbsticks::getRightThumbstickProperty().Length() > 0.3f)
            {
                Rotation = -std::atan2(
                    -VirtualThumbsticks::getRightThumbstickProperty().Y,
                    VirtualThumbsticks::getRightThumbstickProperty().X);

                if (fireTimer <= System::TimeSpan::Zero)
                {
                    const Vector2 bulletVelocity = Vector2::Normalize(
                        VirtualThumbsticks::getRightThumbstickProperty()) * bulletSpeed;
                    Bullets.push_back(std::make_unique<Bullet>(
                        Position, bulletVelocity, Color::Red));
                    fireTimer = cooldown;
                }
            }
            else if (VirtualThumbsticks::getLeftThumbstickProperty().Length() > 0.2f)
            {
                Rotation = -std::atan2(
                    -VirtualThumbsticks::getLeftThumbstickProperty().Y,
                    VirtualThumbsticks::getLeftThumbstickProperty().X);
            }

            for (const auto& bullet : Bullets)
                bullet->Update();

            for (std::size_t i = Bullets.size(); i-- > 0;)
            {
                const Bullet& bullet = *Bullets[i];
                if (bullet.Position.X < -WorldWidth / 2.0f ||
                    bullet.Position.X > WorldWidth / 2.0f ||
                    bullet.Position.Y < -WorldHeight / 2.0f ||
                    bullet.Position.Y > WorldHeight / 2.0f)
                {
                    Bullets.erase(Bullets.begin() + static_cast<std::ptrdiff_t>(i));
                }
            }

            ClampPlayerShip();
        }

        /**
         * @brief Draws all bullets followed by the player ship.
         *
         * @param spriteBatch Sprite batch used for drawing.
         */
        void Draw(SpriteBatch& spriteBatch) const override
        {
            for (const auto& bullet : Bullets)
                bullet->Draw(spriteBatch);
            Ship::Draw(spriteBatch);
        }
    };
}
