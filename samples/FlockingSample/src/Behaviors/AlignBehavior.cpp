// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AlignBehavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Behaviors/AlignBehavior.hpp"

#include "Animals/Animal.hpp"

namespace Flocking
{
    AlignBehavior::AlignBehavior(Animal* animal)
        : Behavior(animal)
    {
    }

    void AlignBehavior::Update(Animal* otherAnimal, const AIParameters& aiParams)
    {
        Behavior::ResetReaction();

        if (otherAnimal != nullptr)
        {
            reacted = true;
            reaction = otherAnimal->getDirectionProperty() * aiParams.PerMemberWeight;
        }
    }
}
