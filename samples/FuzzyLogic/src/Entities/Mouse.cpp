// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Mouse.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Mouse.hpp"

#include "Tank.hpp"
#include "../Behaviors/EvadeBehavior.hpp"
#include "../Behaviors/WanderBehavior.hpp"

namespace FuzzyLogic
{
    System::Random Mouse::random;

    Mouse::Mouse(Rectangle levelBoundary, Tank* tank)
        : Entity(levelBoundary), tank(tank)
    {
        setPositionProperty(Vector2(
            (float)random.Next(levelBoundary.X, levelBoundary.X + levelBoundary.Width),
            (float)random.Next(levelBoundary.Y, levelBoundary.Y + levelBoundary.Height)));

        setCurrentBehaviorProperty(std::make_unique<WanderBehavior>(this));
        this->tank = tank;
    }

    void Mouse::ChooseBehavior(const GameTime& gameTime)
    {
        (void)gameTime;

        // the decision for what behavior to use is simple, and is based on the
        // tank's position. if the tank is far away, we'll idle. If it gets too
        // close we'll flee.

        float distanceFromTank =
            Vector2::Distance(getPositionProperty(), tank->getPositionProperty());

        // The tank is a safe distance away, so the mouse should idle:
        if (dynamic_cast<WanderBehavior*>(getCurrentBehaviorProperty()) == nullptr &&
            distanceFromTank > MouseEvadeDistance + MouseHysteresis)
        {
            setCurrentBehaviorProperty(std::make_unique<WanderBehavior>(this));
        }

        // The tank is too close; the mouse should run:
        else if (dynamic_cast<EvadeBehavior*>(getCurrentBehaviorProperty()) == nullptr &&
            distanceFromTank < MouseEvadeDistance - MouseHysteresis)
        {
            setCurrentBehaviorProperty(std::make_unique<EvadeBehavior>(this, tank));
        }
    }
}
