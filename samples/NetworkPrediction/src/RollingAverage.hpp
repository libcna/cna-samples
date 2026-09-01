// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// RollingAverage.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <vector>

namespace NetworkPrediction
{
    /**
     * @brief Tracks a rolling mean used to compensate for clock skew between peers.
     */
    class RollingAverage
    {
    public:
        /**
         * @brief Creates a rolling average with the requested sample capacity.
         *
         * @param sampleCount Number of recent values to retain.
         */
        explicit RollingAverage(int sampleCount);

        /**
         * @brief Adds a value, replacing the oldest entry when the buffer is full.
         *
         * @param newValue Value to add.
         */
        void AddValue(float newValue);

        /**
         * @brief Gets the average of all entries supplied so far.
         *
         * @return Current average, or zero when no value has been supplied.
         */
        [[nodiscard]] float getAverageValueProperty() const;

    private:
        std::vector<float> sampleValues;
        int sampleCount = 0;
        float valueSum = 0.0f;
        int currentPosition = 0;
    };
}
