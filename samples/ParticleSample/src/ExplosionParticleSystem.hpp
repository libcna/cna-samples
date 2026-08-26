// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ExplosionParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "ParticleSystem.hpp"

namespace ParticleSample
{
    /**
     * @brief A specialization of ParticleSystem which creates a fiery explosion.
     *
     * It should be combined with ExplosionSmokeParticleSystem for best effect.
     */
    class ExplosionParticleSystem : public ParticleSystem
    {
    public:
        /**
         * @brief Constructs the system.
         * @param game The host game.
         * @param howManyEffects Maximum simultaneous effects.
         */
        ExplosionParticleSystem(ParticleSampleGame& game, int howManyEffects);

    protected:
        /**
         * @brief Sets up the constants that give this particle system its behavior and
         *        properties.
         */
        void InitializeConstants() override;

        /**
         * @brief Overriden so explosions slow to a stop by the end of their lifetime.
         * @param p The particle to initialize.
         * @param where Where the particle should be.
         */
        void InitializeParticle(Particle& p, Vector2 where) override;
    };
}
