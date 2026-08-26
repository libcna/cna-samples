// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Camera.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "System/Random.hpp"

namespace CameraShake
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;

    /**
     * @brief A very basic camera that supports the ability to shake.
     */
    class Camera
    {
    public:
        /** @brief The position of the camera. */
        Vector3 Position;

        /** @brief The target location the camera is looking at. */
        Vector3 Target = Vector3::Zero;

        /** @brief The up vector of the camera. */
        Vector3 Up = Vector3::Up;

        /** @brief The projection matrix for the camera. */
        Matrix Projection;

        /**
         * @brief Gets the View matrix from the camera.
         * @return The view matrix, including the current shake offset.
         */
        [[nodiscard]] Matrix getViewProperty() const;

        /**
         * @brief Shakes the camera with a specific magnitude and duration.
         *
         * @param magnitude The largest magnitude to apply to the shake.
         * @param duration The length of time (in seconds) for which the shake should occur.
         */
        void Shake(float magnitude, float duration);

        /**
         * @brief Updates the Camera.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(const GameTime& gameTime);

    private:
        /** Helper to generate a random float in the range of [-1, 1]. */
        [[nodiscard]] static float NextFloat();

        // We only need one Random object no matter how many Cameras we have
        static System::Random random;

        // Are we shaking?
        bool shaking = false;

        // The maximum magnitude of our shake offset
        float shakeMagnitude = 0.0f;

        // The total duration of the current shake
        float shakeDuration = 0.0f;

        // A timer that determines how far into our shake we are
        float shakeTimer = 0.0f;

        // The shake offset vector
        Vector3 shakeOffset;
    };
}
