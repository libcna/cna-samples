// SPDX-License-Identifier: MS-PL

#pragma once

namespace RacingGame::GameLogic::Physics
{
    /** @brief Simulates the Racing game's one-dimensional damped spring. */
    class SpringPhysicsObject
    {
    public:
        /** @brief Default spring mass. */
        static constexpr float DefaultMass = 0.5f;
        /** @brief Default spring friction. */
        static constexpr float DefaultFriction = 0.9f;
        /** @brief Default spring constant. */
        static constexpr float DefaultSpringConstant = 1.0f;

        /** @brief Distance of the attached object from the spring center. */
        float pos = 0.0f;
        /** @brief Velocity of the attached object. */
        float velocity = 0.0f;
        /** @brief Current accumulated spring force. */
        float force = 0.0f;

        /** @brief Creates a spring with the original default constants. */
        SpringPhysicsObject() = default;

        /**
         * @brief Creates a spring with explicitly supplied constants.
         *
         * @param setMass Spring mass.
         * @param setFriction Damping factor.
         * @param setSpringConstant Spring constant.
         * @param setInitialPos Initial displacement.
         */
        SpringPhysicsObject(float setMass, float setFriction,
                            float setSpringConstant, float setInitialPos);

        /**
         * @brief Advances the spring by the supplied time interval.
         *
         * @param timeChange Elapsed time in seconds.
         */
        void Simulate(float timeChange);

        /**
         * @brief Adds a displacement to the current spring position.
         *
         * @param change Displacement to add.
         */
        void ChangePos(float change);

    private:
        float mass = DefaultMass;
        float friction = DefaultFriction;
        float springConstant = DefaultSpringConstant;
    };
}
