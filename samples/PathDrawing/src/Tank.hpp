// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Tank.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "WaypointList.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/Math.hpp"

namespace PathDrawing
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using Microsoft::Xna::Framework::Content::ContentManager;

    /**
     * @brief A simple object that moves towards its set destination.
     *
     * This class is largely borrowed from the Waypoint Sample with the exception that
     * behaviors have been removed and instead we've hard coded the LinearBehavior into
     * the Tank because other steering behaviors usually cause issues with path-based
     * waypoints given that the points are generally quite close to each other.
     */
    class Tank
    {
        /**
         * The "close enough" limit, if the tank is inside this many pixel
         * to it's destination it's considered at it's destination
         */
        static constexpr float atDestinationLimit = 5.0f;

        static constexpr float maxAngularVelocity = MathHelper::Pi;
        static constexpr float maxMoveSpeed = 100.0f;
        static constexpr float maxMoveSpeedDelta = maxMoveSpeed / 2;

        // Graphics data
        Texture2D tankTexture;
        Vector2 tankTextureCenter;

        // Rotation values
        float rotation = 0.0f;
        bool recomputeTargetRotation = true;
        float targetRotation = 0.0f;
        float previousRotation = 0.0f;
        float rotationInterpolation = 0.0f;

    public:
        /**
         * @brief This is how much the Tank can turn in one second in radians, since Pi
         *        radians makes half a circle the tank can all the way around in one second.
         * @return The maximum angular velocity in radians per second.
         */
        [[nodiscard]] static float getMaxAngularVelocityProperty() { return maxAngularVelocity; }

        /**
         * @brief This is the Tank's best possible movement speed.
         * @return The maximum movement speed.
         */
        [[nodiscard]] static float getMaxMoveSpeedProperty() { return maxMoveSpeed; }

        /**
         * @brief This is most the tank can speed up or slow down in one second.
         * @return The maximum movement speed delta.
         */
        [[nodiscard]] static float getMaxMoveSpeedDeltaProperty() { return maxMoveSpeedDelta; }

        /**
         * @brief Gets the length 1 vector that represents the tank's movement and facing direction.
         * @return The direction vector.
         */
        [[nodiscard]] const Vector2& getDirectionProperty() const { return direction; }

        /**
         * @brief Sets the length 1 vector that represents the tank's movement and facing direction.
         * @param value The direction vector.
         */
        void setDirectionProperty(const Vector2& value) { direction = value; }

        /**
         * @brief Gets the tank's current movement speed.
         * @return The movement speed.
         */
        [[nodiscard]] float getMoveSpeedProperty() const { return moveSpeed; }

        /**
         * @brief Sets the tank's current movement speed.
         * @param value The movement speed.
         */
        void setMoveSpeedProperty(float value) { moveSpeed = value; }

        /**
         * @brief Gets the tank's location on the map.
         * @return The location.
         */
        [[nodiscard]] const Vector2& getLocationProperty() const { return location; }

        /**
         * @brief Gets the list of points the tank will move to in order from first to last.
         * @return The waypoint list.
         */
        [[nodiscard]] WaypointList& getWaypointsProperty() { return waypoints; }

        /**
         * @brief Gets the list of points the tank will move to in order from first to last.
         * @return The waypoint list.
         */
        [[nodiscard]] const WaypointList& getWaypointsProperty() const { return waypoints; }

        /**
         * @brief Gets the linear distance to the Tank's current destination.
         * @return The distance in pixels.
         */
        [[nodiscard]] float getDistanceToDestinationProperty() const
        {
            return Vector2::Distance(location, waypoints.Peek());
        }

        /**
         * @brief Gets whether the tank is "close enough" to its destination.
         * @return True when the tank has arrived.
         */
        [[nodiscard]] bool getAtDestinationProperty() const
        {
            return getDistanceToDestinationProperty() < atDestinationLimit;
        }

    protected:
        Vector2 direction;
        float moveSpeed = 0.0f;

    private:
        Vector2 location;
        WaypointList waypoints;

    public:
        /**
         * @brief Tank constructor.
         *
         * @param graphicsDevice The graphics device the tank is drawn with.
         * @param content The content manager the tank texture is loaded from.
         */
        Tank(GraphicsDevice& graphicsDevice, ContentManager& content)
        {
            (void)graphicsDevice;
            location = Vector2::Zero;

            tankTexture = content.Load<Texture2D>("tank");

            tankTextureCenter = Vector2((float)(tankTexture.getWidthProperty() / 2),
                                        (float)(tankTexture.getHeightProperty() / 2));
        }

        /**
         * @brief Reset the Tank's location on the map.
         * @param newLocation New location on the map.
         */
        void Reset(Vector2 newLocation)
        {
            location = newLocation;
            waypoints.Clear();
        }

        /**
         * @brief Tests if a given point is considered to "hit" the tank.
         * @param point The point to test against.
         * @return True if the point is "hitting" the tank, false otherwise.
         */
        [[nodiscard]] bool HitTest(Vector2 point) const
        {
            // We leverage a comparison of squared distances to avoid two square root operations,
            // which can be a slow operation if performed frequently.
            return Vector2::DistanceSquared(point, location) < tankTextureCenter.LengthSquared() * 1.5f;
        }

        /**
         * @brief Update the Tank's position if it's not "close enough" to its destination.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(const GameTime& gameTime)
        {
            float elapsedTime = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

            // If we have any waypoints, the first one on the list is where
            // we want to go
            if (waypoints.getCountProperty() > 0)
            {
                if (getAtDestinationProperty())
                {
                    // If we're at the destination and there is at least one
                    // waypoint in the list, get rid of the first one since we're
                    // there now
                    waypoints.Dequeue();

                    // Whenever we arrive at a destination, we are going to need to
                    // figure out a new target rotation.
                    recomputeTargetRotation = true;
                }
                else
                {
                    // This gives us a vector that points directly from the tank's
                    // current location to the waypoint.
                    direction = -(getLocationProperty() - getWaypointsProperty().Peek());

                    // This scales the vector to 1, we'll use move Speed and elapsed Time
                    // in the Tank's Update function to find the how far the tank moves
                    direction.Normalize();

                    // If we need to recompute our target rotation...
                    if (recomputeTargetRotation)
                    {
                        // Store the previous rotation for interpolation
                        previousRotation = rotation;

                        // Calculate the new rotation based on the direction
                        targetRotation = (float)System::Math::Atan2(direction.Y, direction.X);

                        // Reset our interpolation value
                        rotationInterpolation = 0.0f;

                        // We want to make sure we always turn the shortest way, so we need
                        // to check our rotation values and correct the target value if the
                        // two are more than 180 degrees different.
                        if (targetRotation - previousRotation > MathHelper::Pi)
                            targetRotation -= MathHelper::TwoPi;
                        else if (targetRotation - previousRotation < -MathHelper::Pi)
                            targetRotation += MathHelper::TwoPi;

                        // We don't need to recompute the rotation until we hit the next destination
                        recomputeTargetRotation = false;
                    }

                    // Increase our interpolation value
                    rotationInterpolation =
                        MathHelper::Clamp(rotationInterpolation + elapsedTime * 10.0f, 0.0f, 1.0f);

                    // Calculate our rotation using linear interpolation between our rotation values
                    rotation = previousRotation +
                               (targetRotation - previousRotation) * rotationInterpolation;

                    // Move us along in our direction
                    location += (direction * getMoveSpeedProperty() * elapsedTime);
                }
            }
        }

        /**
         * @brief Draw the Tank.
         * @param spriteBatch An already-begun sprite batch.
         */
        void Draw(SpriteBatch& spriteBatch) const
        {
            spriteBatch.Draw(tankTexture, location, std::nullopt, Color::White, rotation,
                             tankTextureCenter, 1.0f, SpriteEffects::None, 0.0f);
        }
    };
}
