// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Projectile.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Projectile.hpp"

#include "ParticleSystem.hpp"
#include "System/Random.hpp"

namespace Particle3DSample
{
    namespace
    {
        // The original's `static Random random = new Random()`, shared by every projectile.
        System::Random& SharedRandom()
        {
            static System::Random random;
            return random;
        }

        // The C# constructor initialises `position` and `velocity` before handing `position` to
        // the emitter, and a C++ member initialiser list cannot both randomise and forward it.
        Vector3 MakeVelocity()
        {
            Vector3 velocity;
            velocity.X = (float)(SharedRandom().NextDouble() - 0.5) * 60.0f;
            velocity.Y = (float)(SharedRandom().NextDouble() + 0.5) * 40.0f;
            velocity.Z = (float)(SharedRandom().NextDouble() - 0.5) * 60.0f;
            return velocity;
        }
    }

    Projectile::Projectile(ParticleSystem& explosionParticles,
                           ParticleSystem& explosionSmokeParticles,
                           ParticleSystem& projectileTrailParticles)
        : explosionParticles(&explosionParticles),
          explosionSmokeParticles(&explosionSmokeParticles),
          // Use the particle emitter helper to output our trail particles.
          // Start at the origin, firing in a random (but roughly upward) direction.
          trailEmitter(projectileTrailParticles, trailParticlesPerSecond, Vector3::Zero),
          position(Vector3::Zero),
          velocity(MakeVelocity())
    {
        static_assert(sidewaysVelocityRange == 60 && verticalVelocityRange == 40,
                      "MakeVelocity duplicates these ranges");
    }

    bool Projectile::Update(const GameTime& gameTime)
    {
        const float elapsedTime =
            (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        // Simple projectile physics.
        position += velocity * elapsedTime;
        velocity.Y -= elapsedTime * gravity;
        age += elapsedTime;

        // Update the particle emitter, which will create our particle trail.
        trailEmitter.Update(gameTime, position);

        // If enough time has passed, explode! Note how we pass our velocity
        // in to the AddParticle method: this lets the explosion be influenced
        // by the speed and direction of the projectile which created it.
        if (age > projectileLifespan)
        {
            for (int i = 0; i < numExplosionParticles; i++)
                explosionParticles->AddParticle(position, velocity);

            for (int i = 0; i < numExplosionSmokeParticles; i++)
                explosionSmokeParticles->AddParticle(position, velocity);

            return false;
        }

        return true;
    }
}
