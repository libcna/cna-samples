// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Particle.hpp"

#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/Collections/Generic/Queue.hpp"

namespace ParticleSample
{
    using Microsoft::Xna::Framework::DrawableGameComponent;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Graphics::BlendState;
    using Microsoft::Xna::Framework::Graphics::Texture2D;
    using SharpRuntime::String;
    using System::Collections::Generic::Queue;

    class ParticleSampleGame;

    /**
     * @brief ParticleSystem is an abstract class that provides the basic functionality
     *        to create a particle effect.
     *
     * Different subclasses will have different effects, such as fire, explosions, and
     * plumes of smoke. To use these subclasses, simply call AddParticles, and pass in
     * where the particles should exist.
     */
    class ParticleSystem : public DrawableGameComponent
    {
    public:
        // these two values control the order that particle systems are drawn in.
        // typically, particles that use additive blending should be drawn on top of
        // particles that use regular alpha blending. ParticleSystems should therefore
        // set their DrawOrder to the appropriate value in InitializeConstants, though
        // it is possible to use other values for more advanced effects.
        /** @brief Draw order for systems that use alpha blending. */
        static constexpr int AlphaBlendDrawOrder = 100;
        /** @brief Draw order for systems that use additive blending. */
        static constexpr int AdditiveDrawOrder = 200;

        /**
         * @brief Returns the number of particles that are available for a new effect.
         * @return The free particle count.
         */
        [[nodiscard]] int getFreeParticleCountProperty() const
        {
            return static_cast<int>(freeParticles.getCountProperty());
        }

        /**
         * @brief Adds an effect somewhere on the screen.
         *
         * If there aren't enough particles in the freeParticles queue, it will use as
         * many as it can. This means that if there are not enough particles available,
         * calling AddParticles will have no effect.
         *
         * @param where Where the particle effect should be created.
         */
        void AddParticles(Vector2 where);

        /**
         * @brief Overriden to allocate the particle pool once the constants are known.
         */
        void Initialize() override;

        /**
         * @brief Overriden from DrawableGameComponent; updates all active particles.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief Overriden from DrawableGameComponent; renders all active particles
         *        through the game's shared sprite batch.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    protected:
        /**
         * @brief Constructs a new ParticleSystem.
         *
         * @param game The host for this particle system. The game keeps the content
         *        manager and sprite batch for us.
         * @param howManyEffects The maximum number of particle effects that are
         *        expected on screen at once.
         *
         * @remark It is tempting to set the value of howManyEffects very high. However,
         *         this value should be set to the minimum possible, because it has a
         *         large impact on the amount of memory required, and slows down the
         *         Update and Draw functions.
         */
        ParticleSystem(ParticleSampleGame& game, int howManyEffects);

        /**
         * @brief Must be overriden by subclasses to set the constants below.
         *
         * It's here that they should set all the constants marked "constants to be set
         * by subclasses", which give each ParticleSystem its specific flavor.
         */
        virtual void InitializeConstants() = 0;

        /** @brief Overriden to load the texture and calculate its origin. */
        void LoadContent() override;

        /**
         * @brief Randomizes some properties for a particle, then calls Initialize on it.
         *
         * It can be overriden by subclasses if they want to modify the way particles are
         * created. For example, SmokePlumeParticleSystem overrides this function to make
         * all particles accelerate to the right, simulating wind.
         *
         * @param p The particle to initialize.
         * @param where The position on the screen that the particle should be.
         */
        virtual void InitializeParticle(Particle& p, Vector2 where);

        /**
         * @brief Decides which direction particles will move.
         *
         * The default implementation is a random vector in a circular pattern.
         *
         * @return The direction.
         */
        [[nodiscard]] virtual Vector2 PickRandomDirection();

        // This region of values control the "look" of the particle system, and should
        // be set by deriving particle systems in the InitializeConstants method.

        /** minNumParticles and maxNumParticles control the number of particles that are
            added when AddParticles is called. The number of particles will be a random
            number between minNumParticles and maxNumParticles. */
        int minNumParticles = 0;
        int maxNumParticles = 0;

        /** this controls the texture that the particle system uses. It will be used as
            an argument to ContentManager.Load. */
        String textureFilename;

        /** minInitialSpeed and maxInitialSpeed are used to control the initial velocity
            of the particles. The particle's initial speed will be a random number
            between these two. The direction is determined by the function
            PickRandomDirection, which can be overriden. */
        float minInitialSpeed = 0.0f;
        float maxInitialSpeed = 0.0f;

        /** minAcceleration and maxAcceleration are used to control the acceleration of
            the particles. The particle's acceleration will be a random number between
            these two. By default, the direction of acceleration is the same as the
            direction of the initial velocity. */
        float minAcceleration = 0.0f;
        float maxAcceleration = 0.0f;

        /** minRotationSpeed and maxRotationSpeed control the particles' angular
            velocity. Use smaller numbers to make particle systems look calm and wispy,
            and large numbers for more violent effects. */
        float minRotationSpeed = 0.0f;
        float maxRotationSpeed = 0.0f;

        /** minLifetime and maxLifetime are used to control the lifetime. Lifetime is
            also used in Draw to calculate alpha and scale values, to avoid particles
            suddenly "popping" into view. */
        float minLifetime = 0.0f;
        float maxLifetime = 0.0f;

        /** to get some additional variance in the appearance of the particles, we give
            them all random scales, additionally affected by the particle's lifetime. */
        float minScale = 0.0f;
        float maxScale = 0.0f;

        /** different effects can use different blend states. fire and explosions work
            well with additive blending, for example. */
        BlendState blendState;

    private:
        // a reference to the main game; we'll keep this around because it exposes a
        // content manager and a sprite batch for us to use.
        ParticleSampleGame& game;

        // the texture this particle system will use.
        std::optional<Texture2D> texture;

        // the origin when we're drawing textures. this will be the middle of the
        // texture.
        Vector2 origin;

        // this number represents the maximum number of effects this particle system
        // will be expected to draw at one time.
        int howManyEffects;

        // the array of particles used by this system. these are reused, so that calling
        // AddParticles will not cause any allocations. C#'s array holds references; the
        // queue below therefore holds pointers into this vector, which is sized once in
        // Initialize and never resized.
        std::vector<Particle> particles;

        // the queue of free particles keeps track of particles that are not currently
        // being used by an effect.
        Queue<Particle*> freeParticles;
    };
}
