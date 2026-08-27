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
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "System/TimeSpan.hpp"

#include "ParticleSystem.hpp"
#include "Projectile.hpp"

namespace Particle3DSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief Sample showing how to implement a particle system entirely on the GPU, using the
     *        vertex shader to animate particles.
     */
    class Particle3DSampleGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructor. */
        Particle3DSampleGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "Particle3DSample.Particle3DSampleGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
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
        /** The sample can switch between three different visual effects. */
        enum class ParticleState
        {
            Explosions,
            SmokePlume,
            RingOfFire,
        };

        /** Helper for updating the explosions effect. */
        void UpdateExplosions(const GameTime& gameTime);

        /** Helper for updating the list of active projectiles. */
        void UpdateProjectiles(const GameTime& gameTime);

        /** Helper for updating the smoke plume effect. */
        void UpdateSmokePlume();

        /** Helper for updating the fire effect. */
        void UpdateFire();

        /**
         * Helper used by the UpdateFire method. Chooses a random location around a circle, at
         * which a fire particle will be created.
         */
        Vector3 RandomPointOnCircle();

        /** Helper for drawing the background grid model. */
        void DrawGrid(const Matrix& view, const Matrix& projection);

        /** Helper for drawing our message text. */
        void DrawMessage();

        /** Handles input for quitting the game and cycling through the different effects. */
        void HandleInput();

        /** Handles input for moving the camera. */
        void UpdateCamera(const GameTime& gameTime);

        GraphicsDeviceManager graphics;

        std::optional<SpriteBatch> spriteBatch;
        std::optional<SpriteFont> font;
        std::optional<Model> grid;

        // This sample uses five different particle systems. The game owns them; Components.Add
        // stores borrowed pointers.
        std::unique_ptr<ParticleSystem> explosionParticles;
        std::unique_ptr<ParticleSystem> explosionSmokeParticles;
        std::unique_ptr<ParticleSystem> projectileTrailParticles;
        std::unique_ptr<ParticleSystem> smokePlumeParticles;
        std::unique_ptr<ParticleSystem> fireParticles;

        ParticleState currentState = ParticleState::Explosions;

        // The explosions effect works by firing projectiles up into the
        // air, so we need to keep track of all the active projectiles.
        std::vector<Projectile> projectiles;

        System::TimeSpan timeToNextProjectile = System::TimeSpan::Zero;

        // Input state.
        Input::KeyboardState currentKeyboardState;
        Input::GamePadState currentGamePadState;

        Input::KeyboardState lastKeyboardState;
        Input::GamePadState lastGamePadState;

        // Camera state.
        float cameraArc = -5;
        float cameraRotation = 0;
        float cameraDistance = 200;
    };
}
