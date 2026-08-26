// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "ParticleSystem.hpp"

#include "ParticleSampleGame.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "System/InvalidOperationException.hpp"
#include "System/Math.hpp"

#include <optional>

namespace ParticleSample
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Graphics::SpriteEffects;
    using Microsoft::Xna::Framework::Graphics::SpriteSortMode;

    ParticleSystem::ParticleSystem(ParticleSampleGame& game, int howManyEffects)
        : DrawableGameComponent(game)
        , game(game)
        , howManyEffects(howManyEffects)
    {
    }

    void ParticleSystem::Initialize()
    {
        InitializeConstants();

        // calculate the total number of particles we will ever need, using the
        // max number of effects and the max number of particles per effect.
        // once these particles are allocated, they will be reused, so that
        // we don't put any pressure on the garbage collector.
        particles.resize(static_cast<std::size_t>(howManyEffects) *
                         static_cast<std::size_t>(maxNumParticles));
        for (Particle& particle : particles)
        {
            freeParticles.Enqueue(&particle);
        }
        DrawableGameComponent::Initialize();
    }

    void ParticleSystem::LoadContent()
    {
        // make sure sub classes properly set textureFilename.
        if (textureFilename.empty())
        {
            const String message =
                "textureFilename wasn't set properly, so the "
                "particle system doesn't know what texture to load. Make "
                "sure your particle system's InitializeConstants function "
                "properly sets textureFilename.";
            throw System::InvalidOperationException(message);
        }
        // load the texture....
        texture = game.getContentProperty().Load<Texture2D>(textureFilename);

        // ... and calculate the center. this'll be used in the draw call, we
        // always want to rotate and scale around this point.
        origin.X = (float)(texture->getWidthProperty() / 2);
        origin.Y = (float)(texture->getHeightProperty() / 2);

        DrawableGameComponent::LoadContent();
    }

    void ParticleSystem::AddParticles(Vector2 where)
    {
        // the number of particles we want for this effect is a random number
        // somewhere between the two constants specified by the subclasses.
        const int numParticles =
            ParticleSampleGame::getRandomProperty().Next(minNumParticles, maxNumParticles);

        // create that many particles, if you can.
        for (int i = 0; i < numParticles && freeParticles.getCountProperty() > 0; i++)
        {
            // grab a particle from the freeParticles queue, and Initialize it.
            Particle* p = freeParticles.Dequeue();
            InitializeParticle(*p, where);
        }
    }

    void ParticleSystem::InitializeParticle(Particle& p, Vector2 where)
    {
        // first, call PickRandomDirection to figure out which way the particle
        // will be moving. velocity and acceleration's values will come from this.
        const Vector2 direction = PickRandomDirection();

        // pick some random values for our particle
        const float velocity =
            ParticleSampleGame::RandomBetween(minInitialSpeed, maxInitialSpeed);
        const float acceleration =
            ParticleSampleGame::RandomBetween(minAcceleration, maxAcceleration);
        const float lifetime =
            ParticleSampleGame::RandomBetween(minLifetime, maxLifetime);
        const float scale =
            ParticleSampleGame::RandomBetween(minScale, maxScale);
        const float rotationSpeed =
            ParticleSampleGame::RandomBetween(minRotationSpeed, maxRotationSpeed);

        // then initialize it with those random values. initialize will save those,
        // and make sure it is marked as active.
        p.Initialize(where, velocity * direction, acceleration * direction,
                     lifetime, scale, rotationSpeed);
    }

    Vector2 ParticleSystem::PickRandomDirection()
    {
        const float angle = ParticleSampleGame::RandomBetween(0, MathHelper::TwoPi);
        return Vector2((float)System::Math::Cos(angle), (float)System::Math::Sin(angle));
    }

    void ParticleSystem::Update(GameTime& gameTime)
    {
        // calculate dt, the change in the since the last frame. the particle
        // updates will use this value.
        const float dt =
            (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        // go through all of the particles...
        for (Particle& p : particles)
        {
            if (p.getActiveProperty())
            {
                // ... and if they're active, update them.
                p.Update(dt);
                // if that update finishes them, put them onto the free particles
                // queue.
                if (!p.getActiveProperty())
                {
                    freeParticles.Enqueue(&p);
                }
            }
        }

        DrawableGameComponent::Update(gameTime);
    }

    void ParticleSystem::Draw(const GameTime& gameTime)
    {
        // tell sprite batch to begin, using the spriteBlendMode specified in
        // initializeConstants
        game.getSpriteBatchProperty().Begin(SpriteSortMode::Deferred, blendState);

        for (const Particle& p : particles)
        {
            // skip inactive particles
            if (!p.getActiveProperty())
                continue;

            // normalized lifetime is a value from 0 to 1 and represents how far
            // a particle is through its life. 0 means it just started, .5 is half
            // way through, and 1.0 means it's just about to be finished.
            // this value will be used to calculate alpha and scale, to avoid
            // having particles suddenly appear or disappear.
            const float normalizedLifetime =
                p.getTimeSinceStartProperty() / p.getLifetimeProperty();

            // we want particles to fade in and fade out, so we'll calculate alpha
            // to be (normalizedLifetime) * (1-normalizedLifetime). this way, when
            // normalizedLifetime is 0 or 1, alpha is 0. the maximum value is at
            // normalizedLifetime = .5, and is
            // (normalizedLifetime) * (1-normalizedLifetime)
            // (.5)                 * (1-.5)
            // .25
            // since we want the maximum alpha to be 1, not .25, we'll scale the
            // entire equation by 4.
            const float alpha = 4 * normalizedLifetime * (1 - normalizedLifetime);
            const Color color = Color::White * alpha;

            // make particles grow as they age. they'll start at 75% of their size,
            // and increase to 100% once they're finished.
            const float scale = p.getScaleProperty() * (.75f + .25f * normalizedLifetime);

            game.getSpriteBatchProperty().Draw(*texture, p.Position, std::nullopt, color,
                p.getRotationProperty(), origin, scale, SpriteEffects::None, 0.0f);
        }

        game.getSpriteBatchProperty().End();

        DrawableGameComponent::Draw(gameTime);
    }
}
