// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleEmitter.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Particles2DPipelineSample
{
    using namespace Microsoft::Xna::Framework;

    class ParticleSystem;

    /**
     * @brief Helper for objects that want to leave particles behind them as they move around.
     *
     * It works out the right locations for creating particles at any frequency, whether that is
     * faster or slower than the game's update rate.
     */
    class ParticleEmitter
    {
    public:
        /**
         * @brief Constructs a new particle emitter object.
         * @param particleSystem     The system the emitted particles are added to.
         * @param particlesPerSecond How many particles to emit each second.
         * @param initialPosition    Where the emitting object starts.
         */
        ParticleEmitter(ParticleSystem& particleSystem, float particlesPerSecond,
                        Vector2 initialPosition);

        /**
         * @brief Gets where the emitter currently is.
         * @return The emitter's position.
         */
        [[nodiscard]] Vector2 getPositionProperty() const { return position; }

        /**
         * @brief Updates the emitter, creating the appropriate number of particles.
         * @param gameTime    Provides a snapshot of timing values.
         * @param newPosition Where the emitting object is now.
         */
        void Update(const GameTime& gameTime, Vector2 newPosition);

    private:
        ParticleSystem* particleSystem;
        float timeBetweenParticles;
        Vector2 position;
        float timeLeftOver = 0;
    };
}
