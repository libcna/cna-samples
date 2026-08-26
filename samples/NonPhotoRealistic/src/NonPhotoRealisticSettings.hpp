// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// NonPhotoRealisticSettings.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <array>

#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace NonPhotoRealistic
{
    using SharpRuntime::Single;
    using SharpRuntime::String;

    /**
     * @brief Class holds all the settings used to tweak the non-photorealistic rendering.
     */
    class NonPhotoRealisticSettings
    {
    public:
        /**
         * @brief Constructs a new non-photorealistic settings descriptor.
         *
         * @param name              Name of a preset setting, for display to the user.
         * @param enableToonShading Is the cartoon lighting shader enabled?
         * @param enableEdgeDetect  Is the edge detect filter enabled?
         * @param edgeWidth         How wide the detected edges are drawn.
         * @param edgeIntensity     How strongly the detected edges are drawn.
         * @param enableSketch      Is the pencil sketch effect enabled?
         * @param sketchInColor     True to keep the scene's colours in the sketch.
         * @param sketchThreshold   Where the sketch pattern starts to show through.
         * @param sketchBrightness  How bright the sketch pattern is.
         * @param sketchJitterSpeed How often the sketch pattern is re-offset, in seconds.
         */
        NonPhotoRealisticSettings(const String& name, bool enableToonShading,
                                  bool enableEdgeDetect,
                                  Single edgeWidth, Single edgeIntensity,
                                  bool enableSketch, bool sketchInColor,
                                  Single sketchThreshold, Single sketchBrightness,
                                  Single sketchJitterSpeed);

        /** @brief Name of a preset setting, for display to the user. */
        String Name;

        /** @brief Is the cartoon lighting shader enabled? */
        bool EnableToonShading;

        /** @brief Is the edge detect filter enabled? */
        bool EnableEdgeDetect;
        /** @brief How wide the detected edges are drawn. */
        Single EdgeWidth;
        /** @brief How strongly the detected edges are drawn. */
        Single EdgeIntensity;

        /** @brief Is the pencil sketch effect enabled? */
        bool EnableSketch;
        /** @brief True to keep the scene's colours in the sketch. */
        bool SketchInColor;
        /** @brief Where the sketch pattern starts to show through. */
        Single SketchThreshold;
        /** @brief How bright the sketch pattern is. */
        Single SketchBrightness;
        /** @brief How often the sketch pattern is re-offset, in seconds. */
        Single SketchJitterSpeed;

        /** @brief Table of preset settings, used by the sample program. */
        static std::array<NonPhotoRealisticSettings, 6> PresetSettings;
    };
}
