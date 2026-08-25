// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// EvadeBehavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Behavior.hpp"

namespace FuzzyLogic
{
    /**
     * @brief EvadeBehavior is a Behavior that will make an entity evade another.
     *
     * The logic is the same as we have seen in the previous AI sample.
     */
    class EvadeBehavior : public Behavior
    {
    public:
        /**
         * @brief Constructs the behavior.
         * @param entity The entity that will do the evading.
         * @param evade The entity being evaded.
         */
        EvadeBehavior(Entity* entity, Entity* evade);

        /** @brief Turns away from the evaded entity and goes as fast as possible. */
        void Update() override;

    private:
        // The entity to evade
        Entity* evade;
    };
}
