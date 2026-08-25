// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ChaseBehavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Behavior.hpp"

namespace FuzzyLogic
{
    /**
     * @brief ChaseBehavior is a Behavior that will make an entity chase after another.
     *
     * The logic is the same as we have seen in the previous AI sample.
     */
    class ChaseBehavior : public Behavior
    {
    public:
        /**
         * @brief Constructs the behavior.
         * @param entity The entity that will do the chasing.
         * @param chase The entity being chased.
         */
        ChaseBehavior(Entity* entity, Entity* chase);

        /** @brief Turns towards the chased entity and goes as fast as possible. */
        void Update() override;

    private:
        // The entity we are chasing
        Entity* chase;
    };
}
