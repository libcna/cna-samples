// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Behavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Behavior.hpp"

#include "../Entities/Entity.hpp"

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "System/Math.hpp"

namespace FuzzyLogic
{
    using Microsoft::Xna::Framework::MathHelper;

    Behavior::Behavior(Entity* entity)
        : entity(entity)
    {
    }

    void Behavior::TurnToFace(Vector2 facePosition, float turnSpeed)
    {
        float x = facePosition.X - getEntityProperty()->getPositionProperty().X;
        float y = facePosition.Y - getEntityProperty()->getPositionProperty().Y;

        float desiredAngle = (float)System::Math::Atan2(y, x);
        float difference =
            WrapAngle(desiredAngle - getEntityProperty()->getOrientationProperty());

        difference = MathHelper::Clamp(difference, -turnSpeed, turnSpeed);
        getEntityProperty()->setOrientationProperty(
            WrapAngle(getEntityProperty()->getOrientationProperty() + difference));
    }

    float Behavior::WrapAngle(float radians)
    {
        while (radians < -MathHelper::Pi)
        {
            radians += MathHelper::TwoPi;
        }
        while (radians > MathHelper::Pi)
        {
            radians -= MathHelper::TwoPi;
        }
        return radians;
    }
}
