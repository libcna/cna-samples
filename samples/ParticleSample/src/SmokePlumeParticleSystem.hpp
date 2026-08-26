// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SmokePlumeParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "ParticleSystem.hpp"

namespace ParticleSample
{
    /**
     * @brief A specialization of ParticleSystem which sends up a plume of smoke.
     *
     * The smoke is blown to the right by the wind.
     */
    class SmokePlumeParticleSystem : public ParticleSystem
    {
    public:
        /**
         * @brief Constructs the system.
         * @param game The host game.
         * @param howManyEffects Maximum simultaneous effects.
         */
        SmokePlumeParticleSystem(ParticleSampleGame& game, int howManyEffects);

    protected:
        /**
         * @brief Sets up the constants that give this particle system its behavior and
         *        properties.
         */
        void InitializeConstants() override;

        /**
         * @brief Overriden so that particles always have an initial velocity pointing up.
         * @return A random direction which points basically up.
         */
        [[nodiscard]] Vector2 PickRandomDirection() override;

        /**
         * @brief Overriden to add the appearance of wind.
         * @param p The particle to set up.
         * @param where Where the particle should be placed.
         */
        void InitializeParticle(Particle& p, Vector2 where) override;
    };
}
