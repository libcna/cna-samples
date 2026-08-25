// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Mouse.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Entity.hpp"

#include "System/Random.hpp"

namespace FuzzyLogic
{
    class Tank;

    /**
     * @brief The mouse is very basic, and will simply wander around unless the tank
     *        gets too close.
     *
     * If the tank gets too close, he will flee.
     */
    class Mouse : public Entity
    {
    public:
        /**
         * @brief How fast can the mouse move?
         * @return The maximum speed.
         */
        [[nodiscard]] float getMaxSpeedProperty() const override { return 4.25f; }

        /**
         * @brief And how fast can it turn?
         * @return The turn speed, in radians per update.
         */
        [[nodiscard]] float getTurnSpeedProperty() const override { return .2f; }

        /**
         * @brief What texture should Entity use for the mouse?
         * @return The asset name of the mouse's texture.
         */
        [[nodiscard]] String getTextureFileProperty() const override { return "Mouse"; }

        /**
         * @brief The constructor takes in the variables that the mouse needs to store,
         *        and starts the mouse to wandering.
         * @param levelBoundary The rectangle the mouse is clamped to.
         * @param tank The tank the mouse runs away from.
         */
        Mouse(Rectangle levelBoundary, Tank* tank);

    protected:
        /**
         * @brief ChooseBehavior is overriden from Entity, and will determine what the
         *        mouse should do on this update.
         * @param gameTime Provides a snapshot of timing values.
         */
        void ChooseBehavior(const GameTime& gameTime) override;

    private:
        // Controls the distance at which the mouse will flee from tank.
        static constexpr float MouseEvadeDistance = 125.0f;

        // used to avoid hysteresis when trying to decide whether or not to flee
        static constexpr float MouseHysteresis = 45.0f;

        // keep track of the tank, so we know when to run away
        Tank* tank;

        // we'll need a random number generator to randomly place new mice on the
        // screen.
        static System::Random random;
    };
}
