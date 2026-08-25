// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AlignBehavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Behaviors/Behavior.hpp"

namespace Flocking
{
    /**
     * @brief The align reaction of one animal to another.
     */
    class AlignBehavior : public Behavior
    {
    public:
        /**
         * @brief Constructs the behavior for an animal.
         * @param animal The animal this behavior modifies.
         */
        explicit AlignBehavior(Animal* animal);

        /**
         * @brief Works out this behavior's reaction to another animal.
         * @param otherAnimal The animal being reacted to.
         * @param aiParams The current AI weights.
         */
        void Update(Animal* otherAnimal, const AIParameters& aiParams) override;
    };
}
