// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Particle.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Particles2DPipelineSample
{
    using namespace Microsoft::Xna::Framework;

    /** @brief One particle: where it is, where it is going, and how long it has left. */
    class Particle
    {
    public:
        // Position, Velocity, and Acceleration represent exactly what their names
        // indicate. They are public fields rather than properties so that users
        // can directly access their .X and .Y properties.

        /** @brief Where the particle is. */
        Vector2 Position;

        /** @brief How fast it is moving. */
        Vector2 Velocity;

        /** @brief How its velocity is changing. */
        Vector2 Acceleration;

        /** @brief Gets how long this particle will live. */
        [[nodiscard]] float getLifetimeProperty() const { return lifetime; }

        /** @brief Sets how long this particle will live. */
        void setLifetimeProperty(float value) { lifetime = value; }

        /** @brief Gets how long it has been since Initialize was called. */
        [[nodiscard]] float getTimeSinceStartProperty() const { return timeSinceStart; }

        /** @brief Sets how long it has been since Initialize was called. */
        void setTimeSinceStartProperty(float value) { timeSinceStart = value; }

        /** @brief Gets the scale of this particle. */
        [[nodiscard]] float getScaleProperty() const { return scale; }

        /** @brief Sets the scale of this particle. */
        void setScaleProperty(float value) { scale = value; }

        /** @brief Gets its rotation, in radians. */
        [[nodiscard]] float getRotationProperty() const { return rotation; }

        /** @brief Sets its rotation, in radians. */
        void setRotationProperty(float value) { rotation = value; }

        /** @brief Gets how fast it rotates. */
        [[nodiscard]] float getRotationSpeedProperty() const { return rotationSpeed; }

        /** @brief Sets how fast it rotates. */
        void setRotationSpeedProperty(float value) { rotationSpeed = value; }

        /**
         * @brief Gets whether this particle is still alive.
         *
         * Once TimeSinceStart becomes greater than Lifetime, the particle should no longer be
         * drawn or updated.
         *
         * @return true while the particle should still be drawn.
         */
        [[nodiscard]] bool getActiveProperty() const { return timeSinceStart < lifetime; }

        /**
         * @brief Sets the particle up and prepares it for use.
         * @param position     Where it starts.
         * @param velocity     How fast it starts moving.
         * @param acceleration How its velocity changes.
         * @param lifetime     How long it lives, in seconds.
         * @param scale        Its scale.
         * @param rotationSpeed How fast it spins, in radians per second.
         */
        void Initialize(Vector2 position, Vector2 velocity, Vector2 acceleration,
                        float lifetime, float scale, float rotationSpeed);

        /**
         * @brief Called by the ParticleSystem on every frame.
         * @param dt Seconds since the previous update.
         */
        void Update(float dt);

    private:
        float lifetime = 0;
        float timeSinceStart = 0;
        float scale = 0;
        float rotation = 0;
        float rotationSpeed = 0;
    };
}
