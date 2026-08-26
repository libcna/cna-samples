// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ExplosionSmokeParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "ExplosionSmokeParticleSystem.hpp"

#include "Microsoft/Xna/Framework/MathHelper.hpp"

namespace ParticleSample
{
    using Microsoft::Xna::Framework::MathHelper;

    ExplosionSmokeParticleSystem::ExplosionSmokeParticleSystem(ParticleSampleGame& game,
                                                               int howManyEffects)
        : ParticleSystem(game, howManyEffects)
    {
    }

    void ExplosionSmokeParticleSystem::InitializeConstants()
    {
        textureFilename = "smoke";

        // less initial speed than the explosion itself
        minInitialSpeed = 20;
        maxInitialSpeed = 200;

        // acceleration is negative, so particles will accelerate away from the
        // initial velocity.  this will make them slow down, as if from wind
        // resistance. we want the smoke to linger a bit and feel wispy, though,
        // so we don't stop them completely like we do ExplosionParticleSystem
        // particles.
        minAcceleration = -10;
        maxAcceleration = -50;

        // explosion smoke lasts for longer than the explosion itself, but not
        // as long as the plumes do.
        minLifetime = 1.0f;
        maxLifetime = 2.5f;

        minScale = 1.0f;
        maxScale = 2.0f;

        // we need to reduce the number of particles on Windows Phone in order to keep
        // a good framerate
#if defined(WINDOWS_PHONE)
        minNumParticles = 5;
        maxNumParticles = 10;
#else
        minNumParticles = 10;
        maxNumParticles = 20;
#endif

        minRotationSpeed = -MathHelper::PiOver4;
        maxRotationSpeed = MathHelper::PiOver4;

        blendState = BlendState::AlphaBlend;

        setDrawOrderProperty(AlphaBlendDrawOrder);
    }
}
