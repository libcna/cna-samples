// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SeparationBehavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Behaviors/SeparationBehavior.hpp"

#include "Animals/Animal.hpp"

namespace Flocking
{
    SeparationBehavior::SeparationBehavior(Animal* animal)
        : Behavior(animal)
    {
    }

    void SeparationBehavior::Update(Animal* otherAnimal, const AIParameters& aiParams)
    {
        (void)otherAnimal;
        Behavior::ResetReaction();

        Vector2 pushDirection = Vector2::Zero;
        float weight = aiParams.PerMemberWeight;

        if (getAnimalProperty()->getReactionDistanceProperty() > 0.0f &&
            getAnimalProperty()->getReactionDistanceProperty() <= aiParams.SeparationDistance)
        {
            // The otherAnimal is too close so we figure out a pushDirection
            // vector in the opposite direction of the otherAnimal and then weight
            // that reaction based on how close it is vs. our separationDistance
            pushDirection = getAnimalProperty()->getLocationProperty() -
                            getAnimalProperty()->getReactionLocationProperty();
            Vector2::Normalize(pushDirection, pushDirection);
            // push away
            weight *= (1 -
                (float)getAnimalProperty()->getReactionDistanceProperty() / aiParams.SeparationDistance);
            pushDirection *= weight;
            reacted = true;
            reaction += pushDirection;
        }
    }
}
