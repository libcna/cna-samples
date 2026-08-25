// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Tank.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>

#include "WaypointList.hpp"

#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Waypoint
{
    using namespace Microsoft::Xna::Framework;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    class Behavior;

    /** @brief Which movement behavior the tank is using. */
    enum class BehaviorType
    {
        /** @brief Turn instantly and drive straight at the waypoint. */
        Linear,
        /** @brief Turn and accelerate gradually, curving towards the waypoint. */
        Steering,
    };

    /**
     * @brief The member name of a BehaviorType, as C#'s Enum.ToString() would render it.
     *
     * The HUD prints this, so the strings are the C# member names.
     *
     * @param type The behavior type to name.
     * @return Its member name.
     */
    [[nodiscard]] std::string BehaviorTypeToString(BehaviorType type);

    /**
     * @brief A simple object that moves towards its set destination.
     */
    class Tank : public DrawableGameComponent
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
        std::unique_ptr<SpriteBatch> spriteBatch;
        Texture2D tankTexture;
        Vector2 tankTextureCenter;

        /**
         * The tanks' current movement behavior, it's responsible for updating the
         * tanks' movement speed and direction
         */
        std::unique_ptr<Behavior> currentBehavior;

        BehaviorType behaviorType = BehaviorType::Linear;

        Vector2 location;
        WaypointList waypoints;

    protected:
        /** Length 1 vector that represents the tanks' movement and facing direction */
        Vector2 direction;
        /** The tank's current movement speed */
        float moveSpeed = 0.0f;

    public:
        /**
         * @brief This is how much the Tank can turn in one second in radians.
         * @return The maximum angular velocity.
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
         * @brief Gets the current movement behavior.
         * @return The behavior type.
         */
        [[nodiscard]] BehaviorType getBehaviorTypeProperty() const { return behaviorType; }

        /**
         * @brief Sets the current movement behavior, creating it if it changed.
         * @param value The behavior type.
         */
        void setBehaviorTypeProperty(BehaviorType value);

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
         * @brief Gets the linear distance to the tank's current destination.
         * @return The distance in pixels.
         */
        [[nodiscard]] float getDistanceToDestinationProperty() const;

        /**
         * @brief Gets whether the tank is "close enough" to its destination.
         * @return True when the tank has arrived.
         */
        [[nodiscard]] bool getAtDestinationProperty() const;

        /**
         * @brief Tank constructor.
         * @param game The game this component belongs to.
         */
        explicit Tank(Game& game);

        /** @brief Releases the tank. */
        ~Tank() override;

        /**
         * @brief Returns the fully qualified logical type name of this component.
         * @return "Waypoint.Tank".
         */
        [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Reset the tank's location on the map.
         * @param newLocation New location on the map.
         */
        void Reset(Vector2 newLocation);

        /**
         * @brief Update the tank's position if it's not "close enough" to its destination.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief Draw the tank and its waypoints.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

        /** @brief Change the tank movement Behavior. */
        void CycleBehaviorType();

    protected:
        /** @brief Load the tank's texture resources. */
        void LoadContent() override;
    };
}
