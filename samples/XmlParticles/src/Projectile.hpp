// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Projectile.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "ParticleEmitter.hpp"

namespace Particle3DSample
{
    using namespace Microsoft::Xna::Framework;

    class ParticleSystem;

    /**
     * @brief Combines several different particle systems to build up a more sophisticated
     *        composite effect.
     *
     * It implements a rocket projectile, which arcs up into the sky using a ParticleEmitter to
     * leave a steady stream of trail particles behind it, and after a while explodes into a burst
     * of explosion and smoke particles.
     */
    class Projectile
    {
    public:
        /**
         * @brief Constructs a new projectile.
         * @param explosionParticles       System that draws the explosion flames.
         * @param explosionSmokeParticles  System that draws the explosion smoke.
         * @param projectileTrailParticles System that draws the trail behind the projectile.
         */
        Projectile(ParticleSystem& explosionParticles,
                   ParticleSystem& explosionSmokeParticles,
                   ParticleSystem& projectileTrailParticles);

        /**
         * @brief Updates the projectile.
         * @param gameTime Provides a snapshot of timing values.
         * @return false once the projectile has exploded and should be removed.
         */
        bool Update(const GameTime& gameTime);

    private:
        static constexpr float trailParticlesPerSecond = 200;
        static constexpr int numExplosionParticles = 30;
        static constexpr int numExplosionSmokeParticles = 50;
        static constexpr float projectileLifespan = 1.5f;
        static constexpr float sidewaysVelocityRange = 60;
        static constexpr float verticalVelocityRange = 40;
        static constexpr float gravity = 15;

        ParticleSystem* explosionParticles;
        ParticleSystem* explosionSmokeParticles;
        ParticleEmitter trailEmitter;

        Vector3 position;
        Vector3 velocity;
        float age = 0;
    };
}
