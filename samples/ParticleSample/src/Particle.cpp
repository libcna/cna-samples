// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Particle.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Particle.hpp"

#include "ParticleSampleGame.hpp"

#include "Microsoft/Xna/Framework/MathHelper.hpp"

namespace ParticleSample
{
    using Microsoft::Xna::Framework::MathHelper;

    void Particle::Initialize(Vector2 position, Vector2 velocity, Vector2 acceleration,
                              float lifetime, float scale, float rotationSpeed)
    {
        // set the values to the requested values
        this->Position = position;
        this->Velocity = velocity;
        this->Acceleration = acceleration;
        this->setLifetimeProperty(lifetime);
        this->setScaleProperty(scale);
        this->setRotationSpeedProperty(rotationSpeed);

        // reset TimeSinceStart - we have to do this because particles will be
        // reused.
        this->setTimeSinceStartProperty(0.0f);

        // set rotation to some random value between 0 and 360 degrees.
        this->setRotationProperty(
            ParticleSampleGame::RandomBetween(0, MathHelper::TwoPi));
    }

    void Particle::Update(float dt)
    {
        Velocity += Acceleration * dt;
        Position += Velocity * dt;

        setRotationProperty(getRotationProperty() + getRotationSpeedProperty() * dt);

        setTimeSinceStartProperty(getTimeSinceStartProperty() + dt);
    }
}
