// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// LinearBehavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Behaviors/Behavior.hpp"

namespace Waypoint
{
    /**
     * @brief This Behavior makes the tank turn instantly and follow a direct line to
     *        the current waypoint.
     */
    class LinearBehavior : public Behavior
    {
    public:
        /**
         * @brief Constructs the behavior for a tank.
         * @param tank The tank this behavior will modify.
         */
        explicit LinearBehavior(Tank& tank);

        /**
         * @brief Finds the direction vector that goes from a straight line directly to
         *        the current waypoint.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(const Microsoft::Xna::Framework::GameTime& gameTime) override;
    };
}
