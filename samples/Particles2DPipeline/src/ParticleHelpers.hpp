// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleHelpers.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "System/Random.hpp"

namespace Particles2DPipelineSample
{
    /** @brief Random-number helpers shared by every particle system. */
    class ParticleHelpers
    {
    public:
        /**
         * @brief The shared random number generator.
         * @return The one generator every particle draws from.
         */
        static System::Random& Random();

        /**
         * @brief Returns a random float in the given range.
         * @param min Lower limit.
         * @param max Upper limit.
         * @return A value between @p min and @p max.
         */
        static float RandomBetween(float min, float max);
    };
}
