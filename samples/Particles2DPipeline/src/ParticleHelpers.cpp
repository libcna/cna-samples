// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleHelpers.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ParticleHelpers.hpp"

namespace Particles2DPipelineSample
{
    System::Random& ParticleHelpers::Random()
    {
        // The original's `public static readonly Random Random = new Random()`. A function-local
        // static keeps the single shared instance without a static initialization order problem.
        static System::Random random;
        return random;
    }

    float ParticleHelpers::RandomBetween(float min, float max)
    {
        return min + (float)Random().NextDouble() * (max - min);
    }
}
