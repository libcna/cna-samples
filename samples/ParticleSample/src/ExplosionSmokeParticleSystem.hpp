// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ExplosionSmokeParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "ParticleSystem.hpp"

namespace ParticleSample
{
    /**
     * @brief A specialization of ParticleSystem which creates a circular pattern of
     *        smoke.
     *
     * It should be combined with ExplosionParticleSystem for best effect.
     */
    class ExplosionSmokeParticleSystem : public ParticleSystem
    {
    public:
        /**
         * @brief Constructs the system.
         * @param game The host game.
         * @param howManyEffects Maximum simultaneous effects.
         */
        ExplosionSmokeParticleSystem(ParticleSampleGame& game, int howManyEffects);

    protected:
        /**
         * @brief Sets up the constants that give this particle system its behavior and
         *        properties.
         */
        void InitializeConstants() override;
    };
}
