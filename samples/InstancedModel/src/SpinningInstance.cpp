// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SpinningInstance.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "SpinningInstance.hpp"

#include <cmath>

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "System/Random.hpp"

namespace InstancedModelSample
{
    namespace
    {
        // The original's `static Random random = new Random()`. A function-local static keeps the
        // C# field's single shared instance without a static initialization order dependency.
        System::Random& SharedRandom()
        {
            static System::Random random;
            return random;
        }
    }

    SpinningInstance::SpinningInstance()
    {
        size = RandomNumberBetween(0, 1);
        spiralSpeed = RandomNumberBetween(-1, 1);
        spinSpeed = RandomNumberBetween(-2, 2);

        // Choose a random axis for this instance to rotate around.
        spinAxis.X = RandomNumberBetween(-1, 1);
        spinAxis.Y = RandomNumberBetween(-1, 1);
        spinAxis.Z = RandomNumberBetween(-1, 1);

        if (spinAxis.LengthSquared() > 0.001f)
            spinAxis.Normalize();
        else
            spinAxis = Vector3::Up;
    }

    void SpinningInstance::Update(const GameTime& gameTime)
    {
        const float time =
            (float)gameTime.getTotalGameTimeProperty().getTotalSecondsProperty();

        // Combine scale and rotation transforms.
        Matrix scale, rotation;

        Matrix::CreateScale(size, scale);
        Matrix::CreateFromAxisAngle(spinAxis, spinSpeed * time, rotation);

        Matrix::Multiply(scale, rotation, transform);

        // Compute our position along the spiral.
        const float spiralTime = time * spiralSpeed;

        const float spiralSize = (float)std::sin(spiralTime / 4) * 4;

        transform.M41 = (float)std::cos(spiralTime) * spiralSize;
        transform.M42 = (float)std::sin(spiralTime) * spiralSize;
        transform.M43 = (float)std::sin(spiralTime / 3) * 6;
    }

    float SpinningInstance::RandomNumberBetween(float min, float max)
    {
        return MathHelper::Lerp(min, max, (float)SharedRandom().NextDouble());
    }
}
