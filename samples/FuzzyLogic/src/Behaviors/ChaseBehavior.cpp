// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ChaseBehavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "ChaseBehavior.hpp"

#include "../Entities/Entity.hpp"

namespace FuzzyLogic
{
    ChaseBehavior::ChaseBehavior(Entity* entity, Entity* chase)
        : Behavior(entity), chase(chase)
    {
    }

    void ChaseBehavior::Update()
    {
        // Chasing is simple: we just turn towards the entity we want to chase,
        // and go as fast as possible.
        TurnToFace(chase->getPositionProperty(), getEntityProperty()->getTurnSpeedProperty());
        getEntityProperty()->setCurrentSpeedProperty(
            getEntityProperty()->getMaxSpeedProperty());
    }
}
