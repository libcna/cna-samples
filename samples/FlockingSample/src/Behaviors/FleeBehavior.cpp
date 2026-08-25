// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// FleeBehavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Behaviors/FleeBehavior.hpp"

#include "Animals/Animal.hpp"

#include "System/Math.hpp"

namespace Flocking
{
    FleeBehavior::FleeBehavior(Animal* animal)
        : Behavior(animal)
    {
    }

    void FleeBehavior::Update(Animal* otherAnimal, const AIParameters& aiParams)
    {
        (void)otherAnimal;
        Behavior::ResetReaction();

        Vector2 dangerDirection = Vector2::Zero;

        // Vector2.Dot will return a negative result in this case if the
        // otherAnimal is behind the animal, in that case we don't have to
        // worry about it because we're already moving away from it.
        if (Vector2::Dot(
            getAnimalProperty()->getLocationProperty(),
            getAnimalProperty()->getReactionLocationProperty()) >= -(System::Math::PI / 2))
        {
            // set the animal to fleeing so that it flashes red
            getAnimalProperty()->setFleeingProperty(true);
            reacted = true;
            dangerDirection = getAnimalProperty()->getLocationProperty() -
                              getAnimalProperty()->getReactionLocationProperty();
            Vector2::Normalize(dangerDirection, dangerDirection);
            reaction = (aiParams.PerDangerWeight * dangerDirection);
        }
    }
}
