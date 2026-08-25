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
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/Random.hpp"

namespace ChaseAndEvade
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief This is the main type for your game.
     */
    class ChaseAndEvadeGame : public Microsoft::Xna::Framework::Game
    {
        /** What the tank is currently doing. */
        enum class TankAiState
        {
            /** chasing the cat */
            Chasing,
            /** the tank has gotten close enough that the cat that it can stop chasing it */
            Caught,
            /** the tank can't "see" the cat, and is wandering around. */
            Wander
        };

        /** What the mouse is currently doing. */
        enum class MouseAiState
        {
            /** evading the cat */
            Evading,
            /** the mouse can't see the "cat", and it's wandering around. */
            Wander
        };

        // The following values control the different characteristics of the characters
        // in this sample, including their speed, turning rates. distances are specified
        // in pixels, angles are specified in radians.

        /** how fast can the cat move? */
        static constexpr float MaxCatSpeed = 7.5f;

        /** how fast can the tank move? */
        static constexpr float MaxTankSpeed = 5.0f;
        /** how fast can he turn? */
        static constexpr float TankTurnSpeed = 0.10f;
        // this value controls the distance at which the tank will start to chase the
        // cat.
        static constexpr float TankChaseDistance = 250.0f;
        // TankCaughtDistance controls the distance at which the tank will stop because
        // he has "caught" the cat.
        static constexpr float TankCaughtDistance = 60.0f;
        // this constant is used to avoid hysteresis, which is common in ai programming.
        // see the doc for more details.
        static constexpr float TankHysteresis = 15.0f;

        /** how fast can the mouse move? */
        static constexpr float MaxMouseSpeed = 8.5f;
        /** and how fast can it turn? */
        static constexpr float MouseTurnSpeed = 0.20f;
        // MouseEvadeDistance controls the distance at which the mouse will flee from
        // cat. If the mouse is further than "MouseEvadeDistance" pixels away, he will
        // consider himself safe.
        static constexpr float MouseEvadeDistance = 200.0f;
        // this constant is similar to TankHysteresis. The value is larger than the
        // tank's hysteresis value because the mouse is faster than the tank: with a
        // higher velocity, small fluctuations are much more visible.
        static constexpr float MouseHysteresis = 60.0f;

        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;
        // XNA's SpriteFont is a reference type, so the original's field is null until
        // LoadContent assigns it; std::optional is this port's established stand-in.
        std::optional<SpriteFont> spriteFont;

        Texture2D tankTexture;
        Vector2 tankTextureCenter;
        Vector2 tankPosition;
        TankAiState tankState = TankAiState::Wander;
        float tankOrientation = 0.0f;
        Vector2 tankWanderDirection;

        Texture2D catTexture;
        Vector2 catTextureCenter;
        Vector2 catPosition;

        Texture2D mouseTexture;
        Vector2 mouseTextureCenter;
        Vector2 mousePosition;
        MouseAiState mouseState = MouseAiState::Wander;
        float mouseOrientation = 0.0f;
        Vector2 mouseWanderDirection;

        System::Random random;

    public:
        /** @brief Constructs the game and selects the back buffer for this platform. */
        ChaseAndEvadeGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "ChaseAndEvade.ChaseAndEvadeGame".
         */
        [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Spreads the tank, the cat and the mouse across the viewport. */
        void Initialize() override;

        /** @brief Loads the three textures and the HUD font. */
        void LoadContent() override;

        /**
         * @brief Reads input and runs the tank and mouse AI.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        [[nodiscard]] Vector2 ClampToViewport(Vector2 vector);
        void UpdateMouse();
        void UpdateTank();
        void Wander(Vector2 position, Vector2& wanderDirection,
                    float& orientation, float turnSpeed);
        [[nodiscard]] static float TurnToFace(Vector2 position, Vector2 faceThis,
                                              float currentAngle, float turnSpeed);
        [[nodiscard]] static float WrapAngle(float radians);
        void HandleInput();

        /**
         * The member names C#'s Enum.ToString() renders, which the HUD prints.
         */
        [[nodiscard]] static std::string ToString(TankAiState state);
        [[nodiscard]] static std::string ToString(MouseAiState state);
    };
}
