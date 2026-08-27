// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Particle.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Particle.hpp"

#include "Microsoft/Xna/Framework/MathHelper.hpp"

#include "ParticleHelpers.hpp"

namespace Particles2DPipelineSample
{
    void Particle::Initialize(Vector2 position, Vector2 velocity, Vector2 acceleration,
                              float lifetimeValue, float scaleValue, float rotationSpeedValue)
    {
        // set the values to the requested values
        Position = position;
        Velocity = velocity;
        Acceleration = acceleration;
        lifetime = lifetimeValue;
        scale = scaleValue;
        rotationSpeed = rotationSpeedValue;

        // reset TimeSinceStart - we have to do this because particles will be reused.
        timeSinceStart = 0.0f;

        // set rotation to some random value between 0 and 360 degrees.
        rotation = ParticleHelpers::RandomBetween(0, MathHelper::TwoPi);
    }

    void Particle::Update(float dt)
    {
        Velocity += Acceleration * dt;
        Position += Velocity * dt;
        rotation += rotationSpeed * dt;
        timeSinceStart += dt;
    }
}
