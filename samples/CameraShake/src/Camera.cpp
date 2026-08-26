// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Camera.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Camera.hpp"

namespace CameraShake
{
    System::Random Camera::random;

    Matrix Camera::getViewProperty() const
    {
        // Start with our regular position and target
        Vector3 position = Position;
        Vector3 target = Target;

        // If we're shaking, add our offset to our position and target
        if (shaking)
        {
            position += shakeOffset;
            target += shakeOffset;
        }

        // Return the matrix using our modified position and target
        return Matrix::CreateLookAt(position, target, Up);
    }

    void Camera::Shake(float magnitude, float duration)
    {
        // We're now shaking
        shaking = true;

        // Store our magnitude and duration
        shakeMagnitude = magnitude;
        shakeDuration = duration;

        // Reset our timer
        shakeTimer = 0.0f;
    }

    void Camera::Update(const GameTime& gameTime)
    {
        // If we're shaking...
        if (shaking)
        {
            // Move our timer ahead based on the elapsed time
            shakeTimer +=
                (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

            // If we're at the max duration, we're not going to be shaking anymore
            if (shakeTimer >= shakeDuration)
            {
                shaking = false;
                shakeTimer = shakeDuration;
            }

            // Compute our progress in a [0, 1] range
            const float progress = shakeTimer / shakeDuration;

            // Compute our magnitude based on our maximum value and our progress. This causes
            // the shake to reduce in magnitude as time moves on, giving us a smooth transition
            // back to being stationary. We use progress * progress to have a non-linear fall
            // off of our magnitude. We could switch that with just progress if we want a linear
            // fall off.
            const float magnitude = shakeMagnitude * (1.0f - (progress * progress));

            // Generate a new offset vector with three random values and our magnitude
            shakeOffset = Vector3(NextFloat(), NextFloat(), NextFloat()) * magnitude;
        }
    }

    float Camera::NextFloat()
    {
        return (float)random.NextDouble() * 2.0f - 1.0f;
    }
}
