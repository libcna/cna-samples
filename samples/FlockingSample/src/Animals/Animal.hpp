// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Animal.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Behaviors/Behaviors.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/Collections/Generic/Dictionary.hpp"

namespace Flocking
{
    using namespace Microsoft::Xna::Framework;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    /** @brief What kind of animal something is, which decides how others react to it. */
    enum class AnimalType
    {
        /** @brief No type. */
        Generic,
        /** @brief Flies around and reacts. */
        Bird,
        /** @brief Controlled by the thumbstick; birds flee from it. */
        Cat
    };

    /**
     * @brief The base of everything that moves on screen and can be reacted to.
     */
    class Animal
    {
    protected:
        Texture2D texture;
        Color color = Color::White;
        Vector2 textureCenter;
        float moveSpeed = 0.0f;
        System::Collections::Generic::Dictionary<AnimalType, Behaviors> behaviors;

        AnimalType animaltype = AnimalType::Generic;
        float reactionDistance = 0.0f;
        Vector2 reactionLocation;
        bool fleeing = false;
        int boundryWidth = 0;
        int boundryHeight = 0;
        Vector2 direction;
        Vector2 location;

    public:
        /**
         * @brief Gets what kind of animal this is.
         * @return The animal type.
         */
        [[nodiscard]] AnimalType getAnimalTypeProperty() const { return animaltype; }

        /**
         * @brief Gets the distance to the animal last reacted to.
         * @return The reaction distance.
         */
        [[nodiscard]] float getReactionDistanceProperty() const { return reactionDistance; }

        /**
         * @brief Gets the location of the animal last reacted to.
         * @return The reaction location.
         */
        [[nodiscard]] const Vector2& getReactionLocationProperty() const { return reactionLocation; }

        /**
         * @brief Gets whether this animal is currently fleeing.
         * @return True while fleeing.
         */
        [[nodiscard]] bool getFleeingProperty() const { return fleeing; }

        /**
         * @brief Sets whether this animal is currently fleeing.
         * @param value True while fleeing.
         */
        void setFleeingProperty(bool value) { fleeing = value; }

        /**
         * @brief Gets the width animals wrap or clamp at.
         * @return The boundary width.
         */
        [[nodiscard]] int getBoundryWidthProperty() const { return boundryWidth; }

        /**
         * @brief Gets the height animals wrap or clamp at.
         * @return The boundary height.
         */
        [[nodiscard]] int getBoundryHeightProperty() const { return boundryHeight; }

        /**
         * @brief Gets the direction this animal is travelling in.
         * @return The direction vector.
         */
        [[nodiscard]] const Vector2& getDirectionProperty() const { return direction; }

        /**
         * @brief Gets this animal's location.
         * @return The location.
         */
        [[nodiscard]] const Vector2& getLocationProperty() const { return location; }

        /**
         * @brief Sets this animal's location.
         * @param value The location.
         */
        void setLocationProperty(const Vector2& value) { location = value; }

        /**
         * @brief Constructs the animal.
         * @param tex The texture to draw it with; may be empty.
         * @param screenWidth The width it wraps or clamps at.
         * @param screenHeight The height it wraps or clamps at.
         */
        Animal(Texture2D tex, int screenWidth, int screenHeight);

        /** @brief Releases the animal. */
        virtual ~Animal() = default;

        /**
         * @brief Advances the animal.
         * @param gameTime Provides a snapshot of timing values.
         */
        virtual void Update(const GameTime& gameTime);

        /**
         * @brief Draws the animal, centred on its location and rotated to face its direction.
         * @param spriteBatch An already-begun sprite batch.
         * @param gameTime Provides a snapshot of timing values.
         */
        virtual void Draw(SpriteBatch& spriteBatch, const GameTime& gameTime);
    };
}
