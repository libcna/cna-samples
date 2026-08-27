// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleEmitter.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace Particle3DSample
{
    using namespace Microsoft::Xna::Framework;

    class ParticleSystem;

    /**
     * @brief Helper for objects that want to leave particles behind them as they move around the
     *        world.
     *
     * It keeps track of a moving object, remembering its previous position so it can calculate the
     * object's velocity, and works out the right locations for creating particles at any frequency
     * -- whether that is faster or slower than the game's update rate.
     */
    class ParticleEmitter
    {
    public:
        /**
         * @brief Constructs a new particle emitter object.
         * @param particleSystem    The system the emitted particles are added to.
         * @param particlesPerSecond How many particles to emit each second.
         * @param initialPosition   Where the emitting object starts.
         */
        ParticleEmitter(ParticleSystem& particleSystem,
                        float particlesPerSecond, Vector3 initialPosition);

        /**
         * @brief Updates the emitter, creating the appropriate number of particles in the
         *        appropriate positions.
         * @param gameTime    Provides a snapshot of timing values.
         * @param newPosition Where the emitting object is now.
         */
        void Update(const GameTime& gameTime, Vector3 newPosition);

    private:
        ParticleSystem* particleSystem;
        float timeBetweenParticles;
        Vector3 previousPosition;
        float timeLeftOver = 0;
    };
}
