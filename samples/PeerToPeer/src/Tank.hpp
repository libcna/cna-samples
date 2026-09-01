// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Microsoft::Xna::Framework::Content
{
    class ContentManager;
}

namespace Microsoft::Xna::Framework::Graphics
{
    class SpriteBatch;
}

namespace PeerToPeer
{
    /**
     * @brief Moves and draws a tank whose state is synchronized by the game class.
     */
    class Tank
    {
    public:
        /** @brief Current position of the tank. */
        Microsoft::Xna::Framework::Vector2 Position;

        /** @brief Current linear velocity of the tank. */
        Microsoft::Xna::Framework::Vector2 Velocity;

        /** @brief Current tank-body rotation in radians. */
        float TankRotation = 0.0f;

        /** @brief Current turret rotation in radians. */
        float TurretRotation = 0.0f;

        /** @brief Requested movement direction. */
        Microsoft::Xna::Framework::Vector2 TankInput;

        /** @brief Requested turret direction. */
        Microsoft::Xna::Framework::Vector2 TurretInput;

        /**
         * @brief Creates a tank at the starting position selected by its gamer index.
         *
         * @param gamerIndex Index of the owning gamer in the session roster.
         * @param content Content manager used to load the body and turret textures.
         * @param screenWidth Width of the movement area.
         * @param screenHeight Height of the movement area.
         */
        Tank(
            int gamerIndex,
            Microsoft::Xna::Framework::Content::ContentManager& content,
            int screenWidth,
            int screenHeight);

        /** @brief Advances position, velocity, and rotations from the latest input. */
        void Update();

        /**
         * @brief Draws the tank body and turret.
         *
         * @param spriteBatch Active sprite batch.
         */
        void Draw(Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch);

    private:
        static constexpr float TankTurnRate = 0.01f;
        static constexpr float TurretTurnRate = 0.03f;
        static constexpr float TankSpeed = 0.3f;
        static constexpr float TankFriction = 0.9f;

        Microsoft::Xna::Framework::Graphics::Texture2D tankTexture;
        Microsoft::Xna::Framework::Graphics::Texture2D turretTexture;
        Microsoft::Xna::Framework::Vector2 screenSize;

        [[nodiscard]] static float TurnToFace(
            float rotation,
            Microsoft::Xna::Framework::Vector2 target,
            float turnRate);
    };
}
