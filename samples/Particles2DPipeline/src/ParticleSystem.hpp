// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <deque>
#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Particle.hpp"
#include "ParticleSystemSettings.hpp"

namespace Particles2DPipelineSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief Draws and updates one pool of particles, configured entirely from content.
     */
    class ParticleSystem : public Microsoft::Xna::Framework::DrawableGameComponent
    {
    public:
        /** @brief Draw order for the systems that blend normally. */
        static constexpr int AlphaBlendDrawOrder = 100;

        /** @brief Draw order for the systems that blend additively, so they sit on top. */
        static constexpr int AdditiveDrawOrder = 200;

        /**
         * @brief Constructs the system with the default pool size.
         * @param game             The game this component belongs to.
         * @param settingsAssetName Name of the settings asset the pipeline built.
         */
        ParticleSystem(Game& game, std::string settingsAssetName);

        /**
         * @brief Constructs the system.
         * @param game                 The game this component belongs to.
         * @param settingsAssetName    Name of the settings asset the pipeline built.
         * @param initialParticleCount How many particles to allocate up front.
         */
        ParticleSystem(Game& game, std::string settingsAssetName, int initialParticleCount);

        /**
         * @brief Returns the fully qualified logical type name of this component.
         * @return "Particles2DPipelineSample.ParticleSystem".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Gets how many particles are currently free.
         * @return The size of the free queue.
         */
        [[nodiscard]] int getFreeParticleCountProperty() const { return (int)freeParticles.size(); }

        /**
         * @brief Adds a burst of particles.
         * @param where    Where the burst happens.
         * @param velocity The velocity of whatever created it.
         */
        void AddParticles(Vector2 where, Vector2 velocity);

        /**
         * @brief Updates every active particle.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief Draws every active particle.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    protected:
        /** @brief Loads the settings and the texture they name. */
        void LoadContent() override;

    private:
        /** Picks a random direction inside the settings' angle range. */
        Vector2 PickRandomDirection();

        /** Gives one recycled particle its starting state. */
        void InitializeParticle(Particle& p, Vector2 where, Vector2 velocity);

        std::optional<SpriteBatch> spriteBatch;
        std::optional<Texture2D> texture;
        Vector2 origin;

        // The pool owns the particles; the free queue borrows them, exactly as the original's
        // List<Particle> and Queue<Particle> do. std::deque keeps the pointers stable as it grows.
        std::deque<Particle> particles;
        std::deque<Particle*> freeParticles;

        ParticlesSettings::ParticleSystemSettings settings;
        std::string settingsAssetName;
        BlendState blendState;
    };
}
