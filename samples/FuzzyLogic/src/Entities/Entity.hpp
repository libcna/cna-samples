// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Entity.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>

#include "../Behaviors/Behavior.hpp"

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace FuzzyLogic
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::Texture2D;
    using SharpRuntime::String;

    /**
     * @brief An Entity is an abstract class that contains the code that is common
     *        between the tank and the mouse.
     *
     * It is in charge of drawing and updating the tank and mouse objects.
     */
    class Entity
    {
    public:
        /**
         * @brief How fast can this entity move? This property is implemented by
         *        subclasses of Entity.
         * @return The maximum speed.
         */
        [[nodiscard]] virtual float getMaxSpeedProperty() const = 0;

        /**
         * @brief How fast can this entity turn? This property is implemented by
         *        subclasses of Entity.
         * @return The turn speed, in radians per update.
         */
        [[nodiscard]] virtual float getTurnSpeedProperty() const = 0;

        /**
         * @brief Determines what texture file is loaded and drawn for this entity.
         *        This property is implemented by subclasses of Entity.
         * @return The asset name of the entity's texture.
         */
        [[nodiscard]] virtual String getTextureFileProperty() const = 0;

        /**
         * @brief Gets the entity's position on the screen.
         * @return The position.
         */
        [[nodiscard]] Vector2 getPositionProperty() const { return position; }

        /**
         * @brief Sets the entity's position on the screen.
         * @param value The new position.
         */
        void setPositionProperty(Vector2 value) { position = value; }

        /**
         * @brief Gets the entity's orientation. This value is in terms of radians.
         * @return The orientation, in radians.
         */
        [[nodiscard]] float getOrientationProperty() const { return orientation; }

        /**
         * @brief Sets the entity's orientation. This value is in terms of radians.
         * @param value The new orientation, in radians.
         */
        void setOrientationProperty(float value) { orientation = value; }

        /**
         * @brief Gets the current speed of the entity.
         * @return The current speed.
         */
        [[nodiscard]] float getCurrentSpeedProperty() const { return currentSpeed; }

        /**
         * @brief Sets the current speed of the entity. Typically, behaviors will
         *        modify this value.
         * @param value The new speed.
         */
        void setCurrentSpeedProperty(float value) { currentSpeed = value; }

        /**
         * @brief Gets the entity's current behavior.
         *
         * The behavior is in charge of updating the entity's speed and orientation.
         *
         * @return The current behavior, or nullptr when the entity has none.
         */
        [[nodiscard]] Behavior* getCurrentBehaviorProperty() const
        {
            return currentBehavior.get();
        }

        /**
         * @brief Sets the entity's current behavior.
         * @param value The new behavior; ownership is taken.
         */
        void setCurrentBehaviorProperty(std::unique_ptr<Behavior> value)
        {
            currentBehavior = std::move(value);
        }

        /**
         * @brief Gets whether the entity is highlighted.
         *
         * If the entity is highlighted, it will have a pulsing red tint when it is
         * drawn. The tank itself and its prey when it is chasing a mouse.
         *
         * @return True when the entity is highlighted.
         */
        [[nodiscard]] bool getIsHighlightedProperty() const { return isHighlighted; }

        /**
         * @brief Sets whether the entity is highlighted.
         * @param value True to highlight the entity.
         */
        void setIsHighlightedProperty(bool value) { isHighlighted = value; }

        /**
         * @brief Gets the rectangle of the view port the entity may move within.
         *
         * Entities keep track of a rectangle of the view port so that they know where
         * they can go on screen. This is exposed through a property so that behaviors
         * have access to the same information.
         *
         * @return The level boundary.
         */
        [[nodiscard]] Rectangle getLevelBoundaryProperty() const { return levelBoundary; }

        /** @brief Releases the entity. */
        virtual ~Entity() = default;

        /**
         * @brief LoadContent will load the entity's texture.
         * @param content The content manager to load through.
         */
        void LoadContent(ContentManager& content);

        /**
         * @brief Draw will draw the entity using the specified SpriteBatch.
         * @param spriteBatch An already-begun sprite batch.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(SpriteBatch& spriteBatch, const GameTime& gameTime);

        /**
         * @brief Update chooses a new behavior, allows that behavior to update, and
         *        then moves the entity forward.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(const GameTime& gameTime);

    protected:
        /**
         * @brief Constructor takes in the boundary that this entity may move within
         *        and stores it.
         * @param levelBoundary The rectangle the entity is clamped to.
         */
        explicit Entity(Rectangle levelBoundary);

        /**
         * @brief ChooseBehavior will be defined by Entity's subclasses, and is used to
         *        decide which behavior an entity will use next.
         *
         * For example, this is where the tank will change from idling to chasing.
         *
         * @param gameTime Provides a snapshot of timing values.
         */
        virtual void ChooseBehavior(const GameTime& gameTime) = 0;

    private:
        /**
         * This function takes a Vector2 as input, and returns that vector "clamped"
         * to the current graphics title safe area. We use this function to make sure
         * that no one can go off of the screen.
         */
        Vector2 ClampToLevelBoundary(Vector2 vector) const;

        /**
         * The texture that will be drawn to represent this entity.
         */
        Texture2D texture;

        Vector2 position;
        float orientation = 0.0f;
        float currentSpeed = 0.0f;
        std::unique_ptr<Behavior> currentBehavior;
        bool isHighlighted = false;
        Rectangle levelBoundary;
    };
}
