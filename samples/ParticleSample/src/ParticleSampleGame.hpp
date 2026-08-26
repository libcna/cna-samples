// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleSampleGame.cs
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
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/Random.hpp"

namespace ParticleSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;
    using SharpRuntime::String;

    class ExplosionParticleSystem;
    class ExplosionSmokeParticleSystem;
    class SmokePlumeParticleSystem;

    /**
     * @brief This is the main type for the ParticleSample.
     *
     * It creates three different kinds of ParticleSystems, and then adds them to its
     * components collection. It also keeps a random number generator, a SpriteBatch,
     * and a ContentManager that the different classes in this sample can share.
     */
    class ParticleSampleGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game and its three particle systems. */
        ParticleSampleGame();

        /** @brief Releases the game. */
        CNAEXT ~ParticleSampleGame() override;

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "ParticleSample.ParticleSampleGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Gets the sprite batch the particle systems share.
         * @return The sprite batch.
         */
        [[nodiscard]] SpriteBatch& getSpriteBatchProperty() { return *spriteBatch; }

        /**
         * @brief Gets the random number generator the whole sample shares.
         * @return The generator.
         */
        [[nodiscard]] static System::Random& getRandomProperty() { return random; }

        /**
         * @brief A handy little function that gives a random float between two values.
         *
         * This is used in several places in the sample, in particular in
         * ParticleSystem::InitializeParticle.
         *
         * @param min The lower bound.
         * @param max The upper bound.
         * @return A random value between @p min and @p max.
         */
        [[nodiscard]] static float RandomBetween(float min, float max);

    protected:
        /** @brief Load your graphics content. */
        void LoadContent() override;

        /**
         * @brief Allows the game to run logic such as updating the world, checking for
         *        collisions, gathering input and playing audio.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        /**
         * Called when we want to demo the smoke plume effect. It updates the timeTillPuff
         * timer, and adds more particles to the plume when necessary.
         */
        void UpdateSmokePlume(float dt);

        /**
         * Called when we want to demo the explosion effect. It updates the
         * timeTillExplosion timer, and starts another explosion effect when the timer
         * reaches zero.
         */
        void UpdateExplosions(float dt);

        /**
         * Checks to see if the user has just pushed the A button, the space bar, or
         * tapped the screen. If so, we should go to the next effect.
         */
        void HandleInput();

        GraphicsDeviceManager graphics;

        // The particle systems will all need a SpriteBatch to draw their particles,
        // so let's make one they can share. We'll use this to draw our SpriteFont too.
        std::unique_ptr<SpriteBatch> spriteBatch;

        // Used to draw the instructions on the screen.
        std::optional<SpriteFont> font;

        // a random number generator that the whole sample can share.
        static System::Random random;

        // Here's the really fun part of the sample, the particle systems! These are
        // drawable game components, so we can just add them to the components
        // collection.
        std::unique_ptr<ExplosionParticleSystem> explosion;
        std::unique_ptr<ExplosionSmokeParticleSystem> smoke;
        std::unique_ptr<SmokePlumeParticleSystem> smokePlume;

        // State is an enum that represents which effect we're currently demoing.
        enum class State
        {
            Explosions,
            SmokePlume
        };
        // the number of values in the "State" enum.
        static constexpr int NumStates = 2;
        State currentState = State::Explosions;

        /** The name `string.Format("{0}", currentState)` prints for a State value. */
        [[nodiscard]] static String ToString(State state);

        // a timer that will tell us when it's time to trigger another explosion.
        static constexpr float TimeBetweenExplosions = 2.0f;
        float timeTillExplosion = 0.0f;

        // keep a timer that will tell us when it's time to add more particles to the
        // smoke plume.
        static constexpr float TimeBetweenSmokePlumePuffs = .5f;
        float timeTillPuff = 0.0f;

        // keep track of the last frame's keyboard and gamepad state, so that we know
        // if the user has pressed a button.
        KeyboardState lastKeyboardState;
        GamePadState lastGamepadState;
    };
}
