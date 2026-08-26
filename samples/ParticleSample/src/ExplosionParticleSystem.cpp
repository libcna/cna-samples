// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ExplosionParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "ExplosionParticleSystem.hpp"

#include "Microsoft/Xna/Framework/MathHelper.hpp"

namespace ParticleSample
{
    using Microsoft::Xna::Framework::MathHelper;

    ExplosionParticleSystem::ExplosionParticleSystem(ParticleSampleGame& game,
                                                     int howManyEffects)
        : ParticleSystem(game, howManyEffects)
    {
    }

    void ExplosionParticleSystem::InitializeConstants()
    {
        textureFilename = "explosion";

        // high initial speed with lots of variance.  make the values closer
        // together to have more consistently circular explosions.
        minInitialSpeed = 40;
        maxInitialSpeed = 500;

        // doesn't matter what these values are set to, acceleration is tweaked in
        // the override of InitializeParticle.
        minAcceleration = 0;
        maxAcceleration = 0;

        // explosions should be relatively short lived
        minLifetime = .5f;
        maxLifetime = 1.0f;

        minScale = .3f;
        maxScale = 1.0f;

        // we need to reduce the number of particles on Windows Phone in order to keep
        // a good framerate
#if defined(WINDOWS_PHONE)
        minNumParticles = 10;
        maxNumParticles = 12;
#else
        minNumParticles = 20;
        maxNumParticles = 25;
#endif

        minRotationSpeed = -MathHelper::PiOver4;
        maxRotationSpeed = MathHelper::PiOver4;

        // additive blending is very good at creating fiery effects.
        blendState = BlendState::Additive;

        setDrawOrderProperty(AdditiveDrawOrder);
    }

    void ExplosionParticleSystem::InitializeParticle(Particle& p, Vector2 where)
    {
        ParticleSystem::InitializeParticle(p, where);

        // The base works fine except for acceleration. Explosions move outwards,
        // then slow down and stop because of air resistance. Let's change
        // acceleration so that when the particle is at max lifetime, the velocity
        // will be zero.

        // We'll use the equation vt = v0 + (a0 * t). (If you're not familar with
        // this, it's one of the basic kinematics equations for constant
        // acceleration, and basically says:
        // velocity at time t = initial velocity + acceleration * t)
        // We'll solve the equation for a0, using t = p.Lifetime and vt = 0.
        p.Acceleration = -p.Velocity / p.getLifetimeProperty();
    }
}
