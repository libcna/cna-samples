// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CohesionBehavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Behaviors/CohesionBehavior.hpp"

#include "Animals/Animal.hpp"

#include "System/Math.hpp"

namespace Flocking
{
    CohesionBehavior::CohesionBehavior(Animal* animal)
        : Behavior(animal)
    {
    }

    void CohesionBehavior::Update(Animal* otherAnimal, const AIParameters& aiParams)
    {
        (void)otherAnimal;
        Behavior::ResetReaction();

        Vector2 pullDirection = Vector2::Zero;
        float weight = aiParams.PerMemberWeight;

        // if the otherAnimal is too close we dont' want to fly any
        // closer to it
        if (getAnimalProperty()->getReactionDistanceProperty() > 0.0f
            && getAnimalProperty()->getReactionDistanceProperty() > aiParams.SeparationDistance)
        {
            // We want to make the animal move closer the the otherAnimal so we
            // create a pullDirection vector pointing to the otherAnimal bird and
            // weigh it based on how close the otherAnimal is relative to the
            // AIParameters.separationDistance.
            pullDirection = -(getAnimalProperty()->getLocationProperty() -
                              getAnimalProperty()->getReactionLocationProperty());
            Vector2::Normalize(pullDirection, pullDirection);
            weight *= (float)System::Math::Pow((double)
                (getAnimalProperty()->getReactionDistanceProperty() - aiParams.SeparationDistance) /
                    (aiParams.DetectionDistance - aiParams.SeparationDistance), 2);
            pullDirection *= weight;
            reacted = true;
            reaction = pullDirection;
        }
    }
}
