// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// RollingAverage.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "RollingAverage.hpp"

#include <cstddef>

namespace NetworkPrediction
{
    RollingAverage::RollingAverage(int sampleCount)
        : sampleValues(static_cast<std::size_t>(sampleCount), 0.0f)
    {
    }

    void RollingAverage::AddValue(float newValue)
    {
        valueSum -= sampleValues[static_cast<std::size_t>(currentPosition)];
        valueSum += newValue;

        sampleValues[static_cast<std::size_t>(currentPosition)] = newValue;

        ++currentPosition;

        if (currentPosition > sampleCount)
        {
            sampleCount = currentPosition;
        }

        if (currentPosition >= static_cast<int>(sampleValues.size()))
        {
            currentPosition = 0;

            valueSum = 0.0f;
            for (const float value : sampleValues)
            {
                valueSum += value;
            }
        }
    }

    float RollingAverage::getAverageValueProperty() const
    {
        if (sampleCount == 0)
        {
            return 0.0f;
        }

        return valueSum / static_cast<float>(sampleCount);
    }
}
