// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleEmitter.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ParticleEmitter.hpp"

#include "ParticleSystem.hpp"

namespace Particles2DPipelineSample
{
    ParticleEmitter::ParticleEmitter(ParticleSystem& particleSystem, float particlesPerSecond,
                                     Vector2 initialPosition)
        : particleSystem(&particleSystem),
          timeBetweenParticles(1.0f / particlesPerSecond),
          position(initialPosition)
    {
    }

    void ParticleEmitter::Update(const GameTime& gameTime, Vector2 newPosition)
    {
        const float elapsedTime =
            (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        if (elapsedTime > 0)
        {
            const Vector2 velocity = (newPosition - position) / elapsedTime;

            float timeToSpend = timeLeftOver + elapsedTime;
            float currentTime = -timeLeftOver;

            while (timeToSpend > timeBetweenParticles)
            {
                currentTime += timeBetweenParticles;
                timeToSpend -= timeBetweenParticles;

                const float mu = currentTime / elapsedTime;

                const Vector2 particlePosition = Vector2::Lerp(position, newPosition, mu);

                particleSystem->AddParticles(particlePosition, velocity);
            }

            timeLeftOver = timeToSpend;
        }

        position = newPosition;
    }
}
