// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleEmitter.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ParticleEmitter.hpp"

#include "ParticleSystem.hpp"

namespace Particle3DSample
{
    ParticleEmitter::ParticleEmitter(ParticleSystem& particleSystem,
                                     float particlesPerSecond, Vector3 initialPosition)
        : particleSystem(&particleSystem),
          timeBetweenParticles(1.0f / particlesPerSecond),
          previousPosition(initialPosition)
    {
    }

    void ParticleEmitter::Update(const GameTime& gameTime, Vector3 newPosition)
    {
        // Work out how much time has passed since the previous update.
        const float elapsedTime =
            (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        if (elapsedTime > 0)
        {
            // Work out how fast we are moving.
            const Vector3 velocity = (newPosition - previousPosition) / elapsedTime;

            // If we had any time left over that we didn't use during the
            // previous update, add that to the current elapsed time.
            float timeToSpend = timeLeftOver + elapsedTime;

            // Counter for looping over the time interval.
            float currentTime = -timeLeftOver;

            // Create particles as long as we have a big enough time interval.
            while (timeToSpend > timeBetweenParticles)
            {
                currentTime += timeBetweenParticles;
                timeToSpend -= timeBetweenParticles;

                // Work out the optimal position for this particle. This will produce
                // evenly spaced particles regardless of the object speed, particle
                // creation frequency, or game update rate.
                const float mu = currentTime / elapsedTime;

                const Vector3 position = Vector3::Lerp(previousPosition, newPosition, mu);

                // Create the particle.
                particleSystem->AddParticle(position, velocity);
            }

            // Store any time we didn't use, so it can be part of the next update.
            timeLeftOver = timeToSpend;
        }

        previousPosition = newPosition;
    }
}
