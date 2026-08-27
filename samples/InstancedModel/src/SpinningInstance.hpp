// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SpinningInstance.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace InstancedModelSample
{
    using namespace Microsoft::Xna::Framework;

    /**
     * @brief Helper class holds the current position and speed of a single instance, which will
     *        be drawn along with many others in a single batch.
     */
    class SpinningInstance
    {
    public:
        /** @brief Constructor randomly chooses different movement parameters for each instance. */
        SpinningInstance();

        /**
         * @brief Gets a transform matrix describing the current position of this instance.
         * @return The instance's world transform.
         */
        [[nodiscard]] const Matrix& getTransformProperty() const { return transform; }

        /**
         * @brief Updates the position of the instance, moving it along a randomized spiral.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(const GameTime& gameTime);

    private:
        /** Helper for picking a random number inside the specified range. */
        static float RandomNumberBetween(float min, float max);

        float size;
        float spiralSpeed;
        float spinSpeed;
        Vector3 spinAxis;

        Matrix transform;
    };
}
