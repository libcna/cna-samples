// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Behavior.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "AIParameters.hpp"

#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Flocking
{
    using Microsoft::Xna::Framework::Vector2;

    class Animal;

    /**
     * @brief The base class of every reaction an animal can have to another animal.
     */
    class Behavior
    {
    public:
        /**
         * @brief Gets the animal this behavior modifies.
         * @return The animal.
         */
        [[nodiscard]] Animal* getAnimalProperty() const { return animal; }

        /**
         * @brief Sets the animal this behavior modifies.
         * @param value The animal.
         */
        void setAnimalProperty(Animal* value) { animal = value; }

        /**
         * @brief Gets the direction change this behavior produced.
         * @return The reaction vector.
         */
        [[nodiscard]] const Vector2& getReactionProperty() const { return reaction; }

        /**
         * @brief Gets whether this behavior reacted at all on the last update.
         * @return True when it reacted.
         */
        [[nodiscard]] bool getReactedProperty() const { return reacted; }

        /** @brief Releases the behavior. */
        virtual ~Behavior() = default;

        /**
         * @brief Works out this behavior's reaction to another animal.
         * @param otherAnimal The animal being reacted to.
         * @param aiParams The current AI weights.
         */
        virtual void Update(Animal* otherAnimal, const AIParameters& aiParams) = 0;

    protected:
        /**
         * @brief Constructs the behavior for an animal.
         * @param animal The animal this behavior modifies.
         */
        explicit Behavior(Animal* animal) : animal(animal) {}

        /** @brief Clears the reaction before working out a new one. */
        void ResetReaction()
        {
            reacted = false;
            reaction = Vector2::Zero;
        }

        Vector2 reaction;
        bool reacted = false;

    private:
        Animal* animal;
    };
}
