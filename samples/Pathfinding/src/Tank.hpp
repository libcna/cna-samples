// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Tank.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>

#include "Map.hpp"
#include "WaypointList.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/Math.hpp"

namespace Pathfinding
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using Microsoft::Xna::Framework::Content::ContentManager;

    /**
     * @brief A simple object that moves towards its set destination.
     */
    class Tank
    {
        // The "close enough" limit, if the tank is inside this distance to it's
        // destination it's considered at it's destination
        static constexpr float atDestinationLimit = 5.0f;

        Texture2D tankTexture;
        Vector2 tankTextureCenter;
        Map* map = nullptr;

        float scale = 1.0f;

        /** The tanks' movement speed */
        static constexpr float moveSpeed = 100.0f;

        // the location of the tanks' current waypoint
        Vector2 destination;
        // the tanks' location on the map
        Vector2 location;

        WaypointList waypoints;

    protected:
        /** Length 1 vector that represents the tanks' movement and facing direction */
        Vector2 direction;
        bool moving = false;

    public:
        /**
         * @brief Gets the draw scale of the tank.
         * @return The scale.
         */
        [[nodiscard]] float getScaleProperty() const { return scale; }

        /**
         * @brief Sets the draw scale of the tank and of its waypoint list.
         * @param value The scale.
         */
        void setScaleProperty(float value)
        {
            scale = value;
            waypoints.setScaleProperty(value);
        }

        /**
         * @brief Gets the length 1 vector representing the tank's movement and facing direction.
         * @return The direction vector.
         */
        [[nodiscard]] const Vector2& getDirectionProperty() const { return direction; }

        /**
         * @brief Sets the length 1 vector representing the tank's movement and facing direction.
         * @param value The direction vector.
         */
        void setDirectionProperty(const Vector2& value) { direction = value; }

        /**
         * @brief Gets whether the tank is following its waypoints.
         * @return True while the tank is moving.
         */
        [[nodiscard]] bool getMovingProperty() const { return moving; }

        /**
         * @brief Sets whether the tank is following its waypoints.
         * @param value True to make the tank move.
         */
        void setMovingProperty(bool value) { moving = value; }

        /**
         * @brief Gets the tank's movement speed.
         * @return The movement speed.
         */
        [[nodiscard]] static float getMoveSpeedProperty() { return moveSpeed; }

        /**
         * @brief Gets the location of the tank's current waypoint.
         * @return The destination.
         */
        [[nodiscard]] const Vector2& getDestinationProperty() const { return destination; }

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
         * @brief Gets the linear distance to the tank's current destination.
         * @return The distance in pixels.
         */
        [[nodiscard]] float getDistanceToDestinationProperty() const
        {
            return Vector2::Distance(location, destination);
        }

        /**
         * @brief Gets whether the tank is "close enough" to its destination.
         * @return True when the tank has arrived.
         */
        [[nodiscard]] bool getAtDestinationProperty() const
        {
            return getDistanceToDestinationProperty() < atDestinationLimit;
        }

        /** @brief Tank constructor. */
        Tank() = default;

        /**
         * @brief Places the tank at the origin of the given map.
         * @param mazeMap The map the tank drives on.
         */
        void Initialize(Map& mazeMap)
        {
            location = Vector2::Zero;
            destination = location;
            map = &mazeMap;
        }

        /**
         * @brief Load the tank's texture resources.
         * @param content The content manager to load from.
         */
        void LoadContent(ContentManager& content)
        {
            tankTexture = content.Load<Texture2D>("tank");

            tankTextureCenter = Vector2((float)(tankTexture.getWidthProperty() / 2),
                                        (float)(tankTexture.getHeightProperty() / 2));

            waypoints.LoadContent(content);
        }

        /**
         * @brief Draw the Tank.
         * @param spriteBatch The sprite batch to draw with.
         */
        void Draw(SpriteBatch& spriteBatch) const
        {
            waypoints.Draw(spriteBatch);

            float facingDirection = (float)System::Math::Atan2(
                getDirectionProperty().Y, getDirectionProperty().X);

            spriteBatch.Begin();
            spriteBatch.Draw(tankTexture, location, std::nullopt, Color::White, facingDirection,
                             tankTextureCenter, scale, SpriteEffects::None, 0.0f);
            spriteBatch.End();
        }

        /**
         * @brief Update the tank's position if it's not "close enough" to its destination.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(const GameTime& gameTime)
        {
            float elapsedTime = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

            if (moving)
            {
                // If we have any waypoints, the first one on the list is where
                // we want to go
                if (waypoints.getCountProperty() >= 1)
                {
                    destination = waypoints.Peek();
                }

                // If we're at the destination and there is at least one waypoint in
                // the list, get rid of the first one since we're there now
                if (getAtDestinationProperty() && waypoints.getCountProperty() >= 1)
                {
                    waypoints.Dequeue();
                }

                if (!getAtDestinationProperty())
                {
                    direction = -(location - destination);
                    // This scales the vector to 1, we'll use move Speed and elapsed Time
                    // to find the how far the tank moves
                    direction.Normalize();
                    location = location + (getDirectionProperty() *
                        getMoveSpeedProperty() * elapsedTime);
                }
            }
        }

        /**
         * @brief Set the tank's location on the map.
         * @param newLocation New location on the map.
         */
        void SetLocation(Vector2 newLocation)
        {
            location = newLocation;
            // we set the destination to the location right here so the tank
            // doesn't move from where we just put it until we give it a different
            // destination
            destination = newLocation;
            direction = Vector2::Zero;
        }

        /**
         * @brief Set the tank to move toward a new destination.
         * @param newDestination The new destination.
         */
        void SetDestination(Vector2 newDestination)
        {
            destination = newDestination;
        }

        /** @brief Clears the waypoints and returns the tank to the map's start tile. */
        void Reset()
        {
            waypoints.Clear();
            direction = Vector2::Zero;
            moving = false;
            setScaleProperty(map->getScaleProperty());
            location = map->MapToWorld(map->getStartTileProperty(), true);
            destination = location;
        }
    };
}
