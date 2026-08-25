// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Behavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Behaviors/Behavior.hpp"

#include "Tank.hpp"

namespace Waypoint
{
    Behavior::Behavior(Tank& tank)
        : tank(tank)
    {
        tank.setMoveSpeedProperty(Tank::getMaxMoveSpeedProperty());
    }
}
