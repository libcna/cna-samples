// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "ParticleSettings.hpp"
#include "ParticleVertex.hpp"

namespace Particle3DSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief The main component in charge of displaying particles.
     */
    class ParticleSystem : public Microsoft::Xna::Framework::DrawableGameComponent
    {
    public:
        /**
         * @brief Initializes the component.
         *
         * Allocates the particle array and fills in the corner fields, which never change.
         */
        void Initialize() override;

        /**
         * @brief Updates the particle system.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief Draws the particle system.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

        /**
         * @brief Sets the camera view and projection matrices that will be used to draw this
         *        particle system.
         * @param view       The camera's view matrix.
         * @param projection The camera's projection matrix.
         */
        void SetCamera(const Matrix& view, const Matrix& projection);

        /**
         * @brief Adds a new particle to the system.
         * @param position Where the particle starts.
         * @param velocity The velocity of whatever created it.
         */
        void AddParticle(Vector3 position, Vector3 velocity);

    protected:
        /**
         * @brief Constructor.
         * @param game    The game this component belongs to.
         * @param content The content manager the effect and texture are loaded from.
         */
        ParticleSystem(Game& game, Content::ContentManager& content);

        /**
         * @brief Derived particle system classes should override this method and use it to
         *        initalize their tweakable settings.
         * @param settings The settings object to fill in.
         */
        virtual void InitializeSettings(ParticleSettings& settings) = 0;

        /** @brief Loads graphics for the particle system. */
        void LoadContent() override;

    private:
        /** Helper for loading and initializing the particle effect. */
        void LoadParticleEffect();

        /**
         * Helper for checking when active particles have reached the end of their life. It moves
         * old particles from the active area of the queue to the retired section.
         */
        void RetireActiveParticles();

        /**
         * Helper for checking when retired particles have been kept around long enough that we can
         * be sure the GPU is no longer using them.
         */
        void FreeRetiredParticles();

        /** Helper for uploading new particles from our managed array to the GPU vertex buffer. */
        void AddNewParticlesToVertexBuffer();

        // Settings class controls the appearance and animation of this particle system.
        ParticleSettings settings;

        // For loading the effect and particle texture.
        Content::ContentManager* content;

        // Custom effect for drawing particles. This computes the particle
        // animation entirely in the vertex shader: no per-particle CPU work required!
        Effect* particleEffect = nullptr;

        // Shortcuts for accessing frequently changed effect parameters.
        EffectParameter* effectViewParameter = nullptr;
        EffectParameter* effectProjectionParameter = nullptr;
        EffectParameter* effectViewportScaleParameter = nullptr;
        EffectParameter* effectTimeParameter = nullptr;

        // An array of particles, treated as a circular queue.
        std::vector<ParticleVertex> particles;

        // A vertex buffer holding our particles. This contains the same data as
        // the particles array, but copied across to where the GPU can access it.
        std::unique_ptr<DynamicVertexBuffer> vertexBuffer;

        // Index buffer turns sets of four vertices into particle quads (pairs of triangles).
        std::unique_ptr<IndexBuffer> indexBuffer;

        // The particle texture, held so it outlives the effect parameter pointing at it.
        std::optional<Texture2D> texture;

        // The queue's four regions: active, new, free and retired. See ParticleSystem.cs for the
        // full explanation of why a newly created particle cannot be uploaded or reused at once.
        int firstActiveParticle = 0;
        int firstNewParticle = 0;
        int firstFreeParticle = 0;
        int firstRetiredParticle = 0;

        // Store the current time, in seconds.
        float currentTime = 0;

        // Count how many times Draw has been called. This is used to know
        // when it is safe to retire old particles back into the free list.
        int drawCounter = 0;
    };
}
