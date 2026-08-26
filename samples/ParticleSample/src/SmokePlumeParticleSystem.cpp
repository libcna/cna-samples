// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SmokePlumeParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "SmokePlumeParticleSystem.hpp"

#include "ParticleSampleGame.hpp"

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "System/Math.hpp"

namespace ParticleSample
{
    using Microsoft::Xna::Framework::MathHelper;

    SmokePlumeParticleSystem::SmokePlumeParticleSystem(ParticleSampleGame& game,
                                                       int howManyEffects)
        : ParticleSystem(game, howManyEffects)
    {
    }

    void SmokePlumeParticleSystem::InitializeConstants()
    {
        textureFilename = "smoke";

        minInitialSpeed = 20;
        maxInitialSpeed = 100;

        // we don't want the particles to accelerate at all, aside from what we
        // do in our overriden InitializeParticle.
        minAcceleration = 0;
        maxAcceleration = 0;

        // long lifetime, this can be changed to create thinner or thicker smoke.
        // tweak minNumParticles and maxNumParticles to complement the effect.
        minLifetime = 5.0f;
        maxLifetime = 7.0f;

        minScale = .5f;
        maxScale = 1.0f;

        // we need to reduce the number of particles on Windows Phone in order to keep
        // a good framerate
#if defined(WINDOWS_PHONE)
        minNumParticles = 3;
        maxNumParticles = 8;
#else
        minNumParticles = 7;
        maxNumParticles = 15;
#endif

        // rotate slowly, we want a fairly relaxed effect
        minRotationSpeed = -MathHelper::PiOver4 / 2.0f;
        maxRotationSpeed = MathHelper::PiOver4 / 2.0f;

        blendState = BlendState::AlphaBlend;

        setDrawOrderProperty(AlphaBlendDrawOrder);
    }

    Vector2 SmokePlumeParticleSystem::PickRandomDirection()
    {
        // Point the particles somewhere between 80 and 100 degrees.
        // tweak this to make the smoke have more or less spread.
        const float radians = ParticleSampleGame::RandomBetween(
            MathHelper::ToRadians(80), MathHelper::ToRadians(100));

        Vector2 direction = Vector2::Zero;
        // from the unit circle, cosine is the x coordinate and sine is the
        // y coordinate. We're negating y because on the screen increasing y moves
        // down the monitor.
        direction.X = (float)System::Math::Cos(radians);
        direction.Y = -(float)System::Math::Sin(radians);
        return direction;
    }

    void SmokePlumeParticleSystem::InitializeParticle(Particle& p, Vector2 where)
    {
        ParticleSystem::InitializeParticle(p, where);

        // the base is mostly good, but we want to simulate a little bit of wind
        // heading to the right.
        p.Acceleration.X += ParticleSampleGame::RandomBetween(10, 50);
    }
}
