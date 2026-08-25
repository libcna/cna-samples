// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// EvadeBehavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "EvadeBehavior.hpp"

#include "../Entities/Entity.hpp"

namespace FuzzyLogic
{
    EvadeBehavior::EvadeBehavior(Entity* entity, Entity* evade)
        : Behavior(entity), evade(evade)
    {
    }

    void EvadeBehavior::Update()
    {
        // The evasion behavior is accomplished by using the TurnToFace function to
        // turn towards a point on a straight line facing away from the entity we're
        // trying to evade. In other words, if the tank is point A, and the mouse is
        // point B, the "seek point" is C.
        //     C
        //   B
        // A
        Vector2 seekPosition =
            2 * getEntityProperty()->getPositionProperty() - evade->getPositionProperty();
        TurnToFace(seekPosition, getEntityProperty()->getTurnSpeedProperty());
        getEntityProperty()->setCurrentSpeedProperty(
            getEntityProperty()->getMaxSpeedProperty());
    }
}
