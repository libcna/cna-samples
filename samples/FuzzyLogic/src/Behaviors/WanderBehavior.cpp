// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// WanderBehavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "WanderBehavior.hpp"

#include "../Entities/Entity.hpp"

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "System/Math.hpp"

namespace FuzzyLogic
{
    using Microsoft::Xna::Framework::MathHelper;

    System::Random WanderBehavior::random;

    WanderBehavior::WanderBehavior(Entity* entity)
        : Behavior(entity)
    {
        // Initialize wanderDirection so that the entity will start off wandering in
        // the direction he is already going.
        wanderDirection.X =
            (float)System::Math::Cos(getEntityProperty()->getOrientationProperty());
        wanderDirection.Y =
            (float)System::Math::Sin(getEntityProperty()->getOrientationProperty());
    }

    void WanderBehavior::Update()
    {
        wanderDirection.X +=
            MathHelper::Lerp(-.25f, .25f, (float)random.NextDouble());
        wanderDirection.Y +=
            MathHelper::Lerp(-.25f, .25f, (float)random.NextDouble());

        if (wanderDirection != Vector2::Zero)
        {
            wanderDirection.Normalize();
        }

        TurnToFace(getEntityProperty()->getPositionProperty() + wanderDirection,
                   .15f * getEntityProperty()->getTurnSpeedProperty());

        // Next, we'll turn the characters back towards the center of the screen, to
        // prevent them from getting stuck on the edges of the screen.
        Vector2 screenCenter(
            (float)(getEntityProperty()->getLevelBoundaryProperty().Width / 2),
            (float)(getEntityProperty()->getLevelBoundaryProperty().Height / 2));

        float distanceFromCenter =
            Vector2::Distance(screenCenter, getEntityProperty()->getPositionProperty());
        float MaxDistanceFromScreenCenter =
            System::Math::Min(screenCenter.Y, screenCenter.X);

        float normalizedDistance = distanceFromCenter / MaxDistanceFromScreenCenter;

        float turnToCenterSpeed = .3f * normalizedDistance * normalizedDistance *
            getEntityProperty()->getTurnSpeedProperty();

        // Once we've calculated how much we want to turn towards the center, we can
        // use the TurnToFace function to actually do the work.
        TurnToFace(screenCenter, turnToCenterSpeed);

        getEntityProperty()->setCurrentSpeedProperty(
            .25f * getEntityProperty()->getMaxSpeedProperty());
    }
}
