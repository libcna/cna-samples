// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Particle.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace ParticleSample
{
    using Microsoft::Xna::Framework::Vector2;

    /**
     * @brief Particles are the little bits that will make up an effect.
     *
     * Each effect will be comprised of many of these particles. They have basic
     * physical properties, such as position, velocity, acceleration, and rotation.
     * They'll be drawn as sprites, all layered on top of one another, and will be very
     * pretty.
     */
    class Particle
    {
    public:
        // Position, Velocity, and Acceleration represent exactly what their names
        // indicate. They are public fields rather than properties so that users
        // can directly access their .X and .Y properties.
        /** @brief World position of the particle. */
        Vector2 Position;
        /** @brief Current velocity. */
        Vector2 Velocity;
        /** @brief Current acceleration. */
        Vector2 Acceleration;

        /**
         * @brief Gets how long this particle will "live".
         * @return The lifetime, in seconds.
         */
        [[nodiscard]] float getLifetimeProperty() const { return lifetime; }

        /**
         * @brief Sets how long this particle will "live".
         * @param value The lifetime, in seconds.
         */
        void setLifetimeProperty(float value) { lifetime = value; }

        /**
         * @brief Gets how long it has been since Initialize was called.
         * @return The elapsed time, in seconds.
         */
        [[nodiscard]] float getTimeSinceStartProperty() const { return timeSinceStart; }

        /**
         * @brief Sets how long it has been since Initialize was called.
         * @param value The elapsed time, in seconds.
         */
        void setTimeSinceStartProperty(float value) { timeSinceStart = value; }

        /**
         * @brief Gets the scale of this particle.
         * @return The scale.
         */
        [[nodiscard]] float getScaleProperty() const { return scale; }

        /**
         * @brief Sets the scale of this particle.
         * @param value The scale.
         */
        void setScaleProperty(float value) { scale = value; }

        /**
         * @brief Gets the rotation, in radians.
         * @return The rotation.
         */
        [[nodiscard]] float getRotationProperty() const { return rotation; }

        /**
         * @brief Sets the rotation, in radians.
         * @param value The rotation.
         */
        void setRotationProperty(float value) { rotation = value; }

        /**
         * @brief Gets how fast this particle rotates.
         * @return The angular velocity, in radians per second.
         */
        [[nodiscard]] float getRotationSpeedProperty() const { return rotationSpeed; }

        /**
         * @brief Sets how fast this particle rotates.
         * @param value The angular velocity, in radians per second.
         */
        void setRotationSpeedProperty(float value) { rotationSpeed = value; }

        /**
         * @brief Gets whether this particle is still alive.
         *
         * Once TimeSinceStart becomes greater than Lifetime, the particle should no
         * longer be drawn or updated.
         *
         * @return True while the particle is alive.
         */
        [[nodiscard]] bool getActiveProperty() const
        {
            return getTimeSinceStartProperty() < getLifetimeProperty();
        }

        /**
         * @brief Called by ParticleSystem to set up the particle, and prepares the
         *        particle for use.
         *
         * @param position The starting position.
         * @param velocity The starting velocity.
         * @param acceleration The constant acceleration.
         * @param lifetime How long the particle lives, in seconds.
         * @param scale The particle's scale.
         * @param rotationSpeed The angular velocity, in radians per second.
         */
        void Initialize(Vector2 position, Vector2 velocity, Vector2 acceleration,
                        float lifetime, float scale, float rotationSpeed);

        /**
         * @brief Called by the ParticleSystem on every frame.
         *
         * This is where the particle's position and that kind of thing get updated.
         *
         * @param dt The elapsed time, in seconds.
         */
        void Update(float dt);

    private:
        float lifetime = 0.0f;
        float timeSinceStart = 0.0f;
        float scale = 0.0f;
        float rotation = 0.0f;
        float rotationSpeed = 0.0f;
    };
}
