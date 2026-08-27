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

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"

#include "ParticleEmitter.hpp"
#include "ParticleSystem.hpp"

namespace Particles2DPipelineSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief Sample showing four particle systems whose every setting comes from content.
     */
    class ParticleSampleGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game and its four particle systems. */
        ParticleSampleGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "Particles2DPipelineSample.ParticleSampleGame".
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
        /** Which of the three effects is running. */
        enum class State
        {
            Explosions,
            SmokePlume,
            Emitter
        };

        /** Moves the emitter to follow the mouse and emits along the way. */
        void UpdateEmitter(const GameTime& gameTime);

        /** Puffs one burst of smoke at the bottom of the screen every half second. */
        void UpdateSmokePlume(float dt);

        /** Fires one explosion, and its smoke, every two seconds. */
        void UpdateExplosions(float dt);

        /** Handles quitting and cycling through the three effects. */
        void HandleInput();

        static constexpr int NumStates = 3;
        static constexpr float TimeBetweenExplosions = 2.0f;
        static constexpr float TimeBetweenSmokePlumePuffs = .5f;

        GraphicsDeviceManager graphics;

        std::optional<SpriteBatch> spriteBatch;
        std::optional<SpriteFont> font;
        std::optional<Texture2D> emitterSprite;

        // The game owns the four systems; Components.Add stores borrowed pointers.
        std::unique_ptr<ParticleSystem> explosion;
        std::unique_ptr<ParticleSystem> smoke;
        std::unique_ptr<ParticleSystem> smokePlume;
        std::unique_ptr<ParticleSystem> emitterSystem;
        std::unique_ptr<ParticleEmitter> emitter;

        State currentState = State::Explosions;

        float timeTillExplosion = 0.0f;
        float timeTillPuff = 0.0f;

        Input::KeyboardState lastKeyboardState;
        Input::GamePadState lastGamepadState;
    };
}
