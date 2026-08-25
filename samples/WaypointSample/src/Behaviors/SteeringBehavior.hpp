// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SteeringBehavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Behaviors/Behavior.hpp"

#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Waypoint
{
    /**
     * @brief This Behavior makes the tank turn and accelerate gradually, so it curves
     *        towards the current waypoint instead of snapping to face it.
     */
    class SteeringBehavior : public Behavior
    {
    public:
        /**
         * @brief Constructs the behavior for a tank.
         * @param tank The tank this behavior will modify.
         */
        explicit SteeringBehavior(Tank& tank);

        /**
         * @brief Gradually changes the tank's speed and turns it towards the waypoint.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        [[nodiscard]] float FindMaxMoveSpeed(Microsoft::Xna::Framework::Vector2 waypoint) const;

        [[nodiscard]] static float TurnToFace(
            Microsoft::Xna::Framework::Vector2 position,
            Microsoft::Xna::Framework::Vector2 faceThis,
            float currentAngle, float turnSpeed);

        [[nodiscard]] static float WrapAngle(float radians);
    };
}
