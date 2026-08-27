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
     *
     * Unlike the sibling Particles3D sample this class is concrete: every system is the same code
     * driven by a different settings asset, so there are no per-effect subclasses at all.
     */
    class ParticleSystem : public Microsoft::Xna::Framework::DrawableGameComponent
    {
    public:
        /**
         * @brief Constructor.
         * @param game         The game this component belongs to.
         * @param content      The content manager the settings, effect and texture come from.
         * @param settingsName Name of the XML settings asset describing this particle system.
         */
        ParticleSystem(Game& game, Content::ContentManager& content, std::string settingsName);

        /**
         * @brief Returns the fully qualified logical type name of this component.
         * @return "Particle3DSample.ParticleSystem".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

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
         * @brief Loads the settings and the graphics for the particle system.
         *
         * Unlike the sibling Particles3D sample there is no Initialize override: the pool size
         * comes from the settings asset, so the particle array cannot be allocated until this has
         * read it.
         */
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

        // Name of the XML settings file describing this particle system.
        std::string settingsName;

        // Settings class controls the appearance and animation of this particle system, loaded
        // whole from content rather than filled in by a subclass.
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
