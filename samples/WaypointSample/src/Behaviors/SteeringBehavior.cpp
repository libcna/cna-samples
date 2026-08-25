// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SteeringBehavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Behaviors/SteeringBehavior.hpp"

#include "Tank.hpp"

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "System/Math.hpp"

namespace Waypoint
{
    using namespace Microsoft::Xna::Framework;

    SteeringBehavior::SteeringBehavior(Tank& tank)
        : Behavior(tank)
    {
    }

    void SteeringBehavior::Update(const GameTime& gameTime)
    {
        float elapsedTime = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        // This code causes the tank to change its speed gradually while it
        // moves toward the waypoint previousMoveSpeed tracks how fast the
        // tank was going, desiredMoveSpeed finds how fast the tank want to
        // go and Math.Clamp keeps the tank from accelerating or decelerating
        // too fast.
        float previousMoveSpeed = tank.getMoveSpeedProperty();
        float desiredMoveSpeed = FindMaxMoveSpeed(tank.getWaypointsProperty().Peek());
        tank.setMoveSpeedProperty(MathHelper::Clamp(desiredMoveSpeed,
            previousMoveSpeed - Tank::getMaxMoveSpeedDeltaProperty() * elapsedTime,
            previousMoveSpeed + Tank::getMaxMoveSpeedDeltaProperty() * elapsedTime));

        // This code causes the tank to turn towards the waypoint. First we
        // take the vector that represents the tanks' current heading,
        // Tank.Direction, and convert it into an angle in radians. Then we
        // use TurnToFace to make the tank turn towards it's waypoint based
        // on it's turning speed, Tank.MaxAngularVelocity. After we have the
        // new direction in radian we convert it back into a vector.
        float facingDirection = (float)System::Math::Atan2(
            tank.getDirectionProperty().Y, tank.getDirectionProperty().X);
        facingDirection = TurnToFace(tank.getLocationProperty(),
                tank.getWaypointsProperty().Peek(),
                facingDirection, Tank::getMaxAngularVelocityProperty() * elapsedTime);
        tank.setDirectionProperty(Vector2(
            (float)System::Math::Cos(facingDirection),
            (float)System::Math::Sin(facingDirection)));
    }

    float SteeringBehavior::FindMaxMoveSpeed(Vector2 waypoint) const
    {
        float finalSpeed = Tank::getMaxMoveSpeedProperty();

        // Given a velocity v (Tank.MaxMoveSpeed) and an angular velocity
        // w(Tank.MaxAngularVelocity), the smallest turning radius
        // r(turningRadius) ofthe tank is the velocity divided by the turning
        // speed: r = v/w
        float turningRadius =
            Tank::getMaxMoveSpeedProperty() / Tank::getMaxAngularVelocityProperty();

        // This code figures out if the tank can move to its waypoint from its
        // current location based on its turning circle(turningRadius) when its
        // moving as fast as possible(Tank.MaxMoveSpeed). For any given turning
        // circle there is an area to either side of the tank that it cannot
        // move into that can be represented by 2 circles of radius turningRadius
        // on either side of the tank. If the waypoint is inside one of these
        // 2 circles the tank will have to slow down before it can move to it

        // This creates a vector that's orthogonal to the tank in the direction
        // it's facing. This means that the vector is at a right angle to the
        // direction the tank is pointing in.
        Vector2 orth = Vector2(tank.getDirectionProperty().Y, -tank.getDirectionProperty().X);

        // In this code we can combine the tanks' location, the orthogonal
        // vector and the tanks' turning radius to find the 2 points that
        // describe the centers of the circles the tanks cannot move into.
        // Then we use Vector2.Distance to find the distances from each circle
        // center to the waypoint. Afterwards Math.Min return the distance from
        // the waypoint to whichever circle was closest.
        float closestDistance = System::Math::Min(
            Vector2::Distance(waypoint, tank.getLocationProperty() + (orth * turningRadius)),
            Vector2::Distance(waypoint, tank.getLocationProperty() - (orth * turningRadius)));

        // If closestDistance is less than turningRadius, then the waypoint is
        // inside one of the 2 circles the Tank cannot turn into when moving at
        // Tank.MaxMoveSpeed, instead we need to estimate a speed that the tank
        // can move at.
        if (closestDistance < turningRadius)
        {
            // This finds the radius of a circle where the Tank's location and
            // the waypoint are 2 points on opposite sides of the circle.
            float radius = Vector2::Distance(tank.getLocationProperty(), waypoint) / 2;

            // Now we use the radius from above to and Tank.MaxAngularVelocity
            // to find out how fast we can move towards the waypoint by taking
            // r = v/w and turning it into v = r*w
            finalSpeed = Tank::getMaxAngularVelocityProperty() * radius;
        }

        return finalSpeed;
    }

    float SteeringBehavior::TurnToFace(Vector2 position, Vector2 faceThis,
        float currentAngle, float turnSpeed)
    {
        // x and y are just the differences in position between the two objects.
        float x = faceThis.X - position.X;
        float y = faceThis.Y - position.Y;

        // we'll use the Atan2 function. Atan will calculates the arc tangent of
        // y / x for us, and has the added benefit that it will use the signs of x
        // and y to determine what cartesian quadrant to put the result in.
        float desiredAngle = (float)System::Math::Atan2(y, x);

        // so now we know where we WANT to be facing, and where we ARE facing...
        // we have to calculate how much we WANT to turn, and then make sure that
        // amount is no greater than turnSpeed.
        float difference = WrapAngle(desiredAngle - currentAngle);

        difference = MathHelper::Clamp(difference, -turnSpeed, turnSpeed);

        return WrapAngle(currentAngle + difference);
    }

    float SteeringBehavior::WrapAngle(float radians)
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
