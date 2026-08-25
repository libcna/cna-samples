// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// LinearBehavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Behaviors/LinearBehavior.hpp"

#include "Tank.hpp"

#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Waypoint
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Vector2;

    LinearBehavior::LinearBehavior(Tank& tank)
        : Behavior(tank)
    {
    }

    void LinearBehavior::Update(const GameTime& gameTime)
    {
        (void)gameTime;

        // This gives us a vector that points directly from the tank's
        // current location to the waypoint.
        Vector2 direction = -(tank.getLocationProperty() - tank.getWaypointsProperty().Peek());
        // This scales the vector to 1, we'll use move Speed and elapsed Time
        // in the Tank's Update function to find the how far the tank moves
        direction.Normalize();
        tank.setDirectionProperty(direction);
    }
}
