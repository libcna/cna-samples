// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Flock.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <vector>

#include "AIParameters.hpp"
#include "Animals/Bird.hpp"
#include "Animals/Cat.hpp"

namespace Flocking
{
    /**
     * @brief The whole flock: every bird reacts to every other bird and to the cat.
     */
    class Flock
    {
        /** Number of flock members */
        static constexpr int flockSize = 40;

        // birds that fly out of the boundry(screen) will wrap around to
        // the other side
        int boundryWidth = 0;
        int boundryHeight = 0;
        Texture2D birdTexture;
        std::vector<Bird> flock;

    protected:
        AIParameters flockParams;

    public:
        /**
         * @brief Gets the AI weights the flock runs on.
         *
         * @warning This reproduces an upstream defect. The original's getter is
         *          `get { return FlockParams; }` -- it returns the property, not the
         *          field, so reading it recurses until the stack is exhausted. Nothing in
         *          the sample reads it (the game only assigns), so the original never
         *          faults, and neither does this. It is kept because this port preserves
         *          the original's behaviour, defects included.
         * @return Never returns.
         */
        [[nodiscard]] AIParameters getFlockParamsProperty() const;

        /**
         * @brief Sets the AI weights the flock runs on.
         * @param value The new weights.
         */
        void setFlockParamsProperty(const AIParameters& value) { flockParams = value; }

        /**
         * @brief Constructs the flock and fills it with randomly placed birds.
         * @param tex The bird texture.
         * @param screenWidth The width birds wrap at.
         * @param screenHeight The height birds wrap at.
         * @param flockParameters The initial AI weights.
         */
        Flock(Texture2D tex, int screenWidth, int screenHeight,
              const AIParameters& flockParameters);

        /**
         * @brief Lets every bird look at every other bird and at the cat, then move.
         * @param gameTime Provides a snapshot of timing values.
         * @param cat The cat, or null when there is none.
         */
        void Update(const GameTime& gameTime, Cat* cat);

        /**
         * @brief Draws every bird.
         * @param spriteBatch An already-begun sprite batch.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(SpriteBatch& spriteBatch, const GameTime& gameTime);

        /** @brief Replaces every bird with a new one at a new random place. */
        void ResetFlock();
    };
}
