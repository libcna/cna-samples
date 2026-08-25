// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// WanderBehavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Behavior.hpp"

#include "System/Random.hpp"

namespace FuzzyLogic
{
    /**
     * @brief WanderBehavior is a Behavior that will make entities move around the screen
     *        aimlessly.
     *
     * The logic is the same as we have seen in the previous AI sample.
     */
    class WanderBehavior : public Behavior
    {
    public:
        /**
         * @brief Constructs the behavior, wandering off in the direction the entity is
         *        already going.
         * @param entity The entity that will wander.
         */
        explicit WanderBehavior(Entity* entity);

        /**
         * @brief Modifies the associated entity's orientation and speed to make it
         *        wander around.
         *
         * The logic contained in this function is the same as we saw in the Chase and
         * Evade sample.
         */
        void Update() override;

    private:
        // The direction we are currently wandering in.
        Vector2 wanderDirection;

        // We'll use this random number generator to tweak wanderDirection a little bit
        // on every update.
        static System::Random random;
    };
}
