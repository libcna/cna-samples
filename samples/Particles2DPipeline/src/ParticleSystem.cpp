// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ParticleSystem.hpp"

#include <cmath>
#include <utility>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"

#include "ParticleHelpers.hpp"

namespace Particles2DPipelineSample
{
    using ParticlesSettings::AccelerationMode;
    using ParticlesSettings::ParticleSystemSettings;

    ParticleSystem::ParticleSystem(Game& game, std::string settingsAssetName)
        : ParticleSystem(game, std::move(settingsAssetName), 10)
    {
    }

    ParticleSystem::ParticleSystem(Game& game, std::string settingsAssetName,
                                   int initialParticleCount)
        : DrawableGameComponent(game), settingsAssetName(std::move(settingsAssetName))
    {
        for (int i = 0; i < initialParticleCount; i++)
        {
            particles.emplace_back();
            freeParticles.push_back(&particles[(std::size_t)i]);
        }
    }

    const std::string& ParticleSystem::GetTypeName() const
    {
        static const std::string typeName{"Particles2DPipelineSample.ParticleSystem"};
        return typeName;
    }

    void ParticleSystem::LoadContent()
    {
        settings = getGameProperty().getContentProperty()
                       .Load<ParticleSystemSettings>(settingsAssetName);
        texture.emplace(getGameProperty().getContentProperty()
                            .Load<Texture2D>(settings.TextureFilename));

        origin.X = (float)(texture->getWidthProperty() / 2);
        origin.Y = (float)(texture->getHeightProperty() / 2);

        spriteBatch.emplace(getGraphicsDeviceProperty());

        blendState = BlendState();
        blendState.setAlphaSourceBlendProperty(settings.SourceBlend);
        blendState.setColorSourceBlendProperty(settings.SourceBlend);
        blendState.setAlphaDestinationBlendProperty(settings.DestinationBlend);
        blendState.setColorDestinationBlendProperty(settings.DestinationBlend);

        DrawableGameComponent::LoadContent();
    }

    Vector2 ParticleSystem::PickRandomDirection()
    {
        float angle = ParticleHelpers::RandomBetween(settings.MinDirectionAngle,
                                                     settings.MaxDirectionAngle);
        angle = MathHelper::ToRadians(angle);
        return Vector2((float)std::cos(angle), (float)std::sin(angle));
    }

    void ParticleSystem::AddParticles(Vector2 where, Vector2 velocity)
    {
        const int numParticles =
            ParticleHelpers::Random().Next(settings.MinNumParticles, settings.MaxNumParticles);

        for (int i = 0; i < numParticles; i++)
        {
            if (freeParticles.empty())
            {
                for (int j = 0; j < 10; j++)
                {
                    particles.emplace_back();
                    freeParticles.push_back(&particles.back());
                }
            }

            Particle* p = freeParticles.front();
            freeParticles.pop_front();
            InitializeParticle(*p, where, velocity);
        }
    }

    void ParticleSystem::InitializeParticle(Particle& p, Vector2 where, Vector2 velocity)
    {
        velocity *= settings.EmitterVelocitySensitivity;

        const Vector2 direction = PickRandomDirection();

        const float speed = ParticleHelpers::RandomBetween(settings.MinInitialSpeed,
                                                           settings.MaxInitialSpeed);
        velocity += direction * speed;

        const float lifetime =
            ParticleHelpers::RandomBetween(settings.MinLifetime, settings.MaxLifetime);
        const float scale =
            ParticleHelpers::RandomBetween(settings.MinSize, settings.MaxSize);
        float rotationSpeed =
            ParticleHelpers::RandomBetween(settings.MinRotationSpeed, settings.MaxRotationSpeed);
        rotationSpeed = MathHelper::ToRadians(rotationSpeed);

        Vector2 acceleration = Vector2::Zero;

        switch (settings.AccelerationMode)
        {
            case AccelerationMode::Scalar:
            {
                const float accelerationScale = ParticleHelpers::RandomBetween(
                    settings.MinAccelerationScale, settings.MaxAccelerationScale);
                acceleration = direction * accelerationScale;
                break;
            }
            case AccelerationMode::EndVelocity:
                acceleration = (velocity * (settings.EndVelocity - 1)) / lifetime;
                break;
            case AccelerationMode::Vector:
                acceleration = Vector2(
                    ParticleHelpers::RandomBetween(settings.MinAccelerationVector.X,
                                                   settings.MaxAccelerationVector.X),
                    ParticleHelpers::RandomBetween(settings.MinAccelerationVector.Y,
                                                   settings.MaxAccelerationVector.Y));
                break;
            default:
                break;
        }

        p.Initialize(where, velocity, acceleration, lifetime, scale, rotationSpeed);
    }

    void ParticleSystem::Update(GameTime& gameTime)
    {
        const float dt = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        for (Particle& p : particles)
        {
            if (p.getActiveProperty())
            {
                p.Acceleration += settings.Gravity * dt;
                p.Update(dt);

                if (!p.getActiveProperty())
                {
                    freeParticles.push_back(&p);
                }
            }
        }

        DrawableGameComponent::Update(gameTime);
    }

    void ParticleSystem::Draw(const GameTime& gameTime)
    {
        spriteBatch->Begin(SpriteSortMode::Deferred, blendState);

        for (Particle& p : particles)
        {
            if (!p.getActiveProperty())
                continue;

            const float normalizedLifetime =
                p.getTimeSinceStartProperty() / p.getLifetimeProperty();

            const float alpha = 4 * normalizedLifetime * (1 - normalizedLifetime);
            const Color color = Color::White * alpha;

            const float scale = p.getScaleProperty() * (.75f + .25f * normalizedLifetime);

            spriteBatch->Draw(*texture, p.Position, std::nullopt, color,
                              p.getRotationProperty(), origin, scale, SpriteEffects::None, 0.0f);
        }

        spriteBatch->End();

        DrawableGameComponent::Draw(gameTime);
    }
}
