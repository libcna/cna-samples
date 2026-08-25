// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Behavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace FuzzyLogic
{
    using Microsoft::Xna::Framework::Vector2;

    class Entity;

    /**
     * @brief Behavior is the base class for the three behaviors in this sample: chasing,
     *        evading, and wandering.
     *
     * It is an abstract class, leaving the implementation of Update up to its subclasses.
     * Entity objects keep track of their current behavior and tell it to update.
     */
    class Behavior
    {
    public:
        /**
         * @brief Gets the entity that this behavior will modify.
         * @return The entity.
         */
        [[nodiscard]] Entity* getEntityProperty() const { return entity; }

        /**
         * @brief Sets the entity that this behavior will modify.
         * @param value The entity.
         */
        void setEntityProperty(Entity* value) { entity = value; }

        /** @brief Releases the behavior. */
        virtual ~Behavior() = default;

        /** @brief Updates the associated entity's speed and orientation. */
        virtual void Update() = 0;

        /**
         * @brief Turns the entity towards a vector at the given speed.
         *
         * This is the same logic as the TurnToFace function that was introduced in the
         * Chase and Evade sample.
         *
         * @param facePosition The position to turn towards.
         * @param turnSpeed The maximum amount to turn by, in radians.
         */
        void TurnToFace(Vector2 facePosition, float turnSpeed);

        /**
         * @brief Returns the angle expressed in radians between -Pi and Pi.
         * @param radians The angle to wrap, in radians.
         * @return The input value expressed in radians from -Pi to Pi.
         */
        static float WrapAngle(float radians);

    protected:
        /**
         * @brief Constructs the behavior for the given entity.
         * @param entity The entity this behavior will modify.
         */
        explicit Behavior(Entity* entity);

    private:
        Entity* entity;
    };
}
