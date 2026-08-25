// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Behavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/GameTime.hpp"

namespace Waypoint
{
    class Tank;

    /**
     * @brief Behavior is the base class for the two behaviors in this sample: linear
     *        and steering.
     *
     * It is an abstract class, leaving the implementation of Update up to its
     * subclasses.
     */
    class Behavior
    {
    protected:
        // Keeps track of the tank that this behavior will modify
        Tank& tank;

        /**
         * @brief Constructs the behavior and gives its tank the maximum move speed.
         * @param tank The tank this behavior will modify.
         */
        explicit Behavior(Tank& tank);

    public:
        /** @brief Releases the behavior. */
        virtual ~Behavior() = default;

        /**
         * @brief Updates the tank's movement speed and direction.
         * @param gameTime Provides a snapshot of timing values.
         */
        virtual void Update(const Microsoft::Xna::Framework::GameTime& gameTime) = 0;
    };
}
