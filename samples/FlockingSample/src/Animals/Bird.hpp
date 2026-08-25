// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Bird.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "AIParameters.hpp"
#include "Animals/Animal.hpp"

#include "System/Random.hpp"

namespace Flocking
{
    /**
     * @brief One member of the flock: it reacts to its neighbours and to the cat.
     */
    class Bird : public Animal
    {
        System::Random random;
        Vector2 aiNewDir;
        int aiNumSeen = 0;

    public:
        /**
         * @brief Constructs a bird.
         * @param tex The texture to draw it with.
         * @param dir Its initial direction; normalized here.
         * @param loc Its initial location, which also seeds its random generator.
         * @param screenWidth The width it wraps at.
         * @param screenHeight The height it wraps at.
         */
        Bird(Texture2D tex, Vector2 dir, Vector2 loc, int screenWidth, int screenHeight);

        /**
         * @brief Turns the bird towards where its behaviors want it and moves it.
         * @param gameTime Provides a snapshot of timing values.
         * @param aiParams The current AI weights.
         */
        void Update(const GameTime& gameTime, AIParameters& aiParams);

        /**
         * @brief Draws the bird, pulsing red while it is fleeing.
         * @param spriteBatch An already-begun sprite batch.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(SpriteBatch& spriteBatch, const GameTime& gameTime) override;

        /** @brief Builds the reactions this bird has to cats and to other birds. */
        void BuildBehaviors();

        /** @brief Clears what this bird saw last frame, before it looks again. */
        void ResetThink();

        /**
         * @brief Accumulates this bird's reaction to another animal.
         * @param animal The animal to react to; ignored when null.
         * @param AIparams The current AI weights.
         */
        void ReactTo(Animal* animal, AIParameters& AIparams);

    private:
        void ClosestLocation(const Vector2& srcLocation, const Vector2& destLocation,
                             Vector2& outLocation) const;

        [[nodiscard]] static Vector2 ChangeDirection(
            Vector2 oldDir, Vector2 newDir, float maxTurnRadians);

        [[nodiscard]] static float WrapAngle(float radians);
    };
}
