// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// NonPhotoRealisticSettings.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "NonPhotoRealisticSettings.hpp"

namespace NonPhotoRealistic
{
    NonPhotoRealisticSettings::NonPhotoRealisticSettings(
        const String& name, const bool enableToonShading,
        const bool enableEdgeDetect,
        const Single edgeWidth, const Single edgeIntensity,
        const bool enableSketch, const bool sketchInColor,
        const Single sketchThreshold, const Single sketchBrightness,
        const Single sketchJitterSpeed)
        : Name(name),
          EnableToonShading(enableToonShading),
          EnableEdgeDetect(enableEdgeDetect),
          EdgeWidth(edgeWidth),
          EdgeIntensity(edgeIntensity),
          EnableSketch(enableSketch),
          SketchInColor(sketchInColor),
          SketchThreshold(sketchThreshold),
          SketchBrightness(sketchBrightness),
          SketchJitterSpeed(sketchJitterSpeed)
    {
    }

    std::array<NonPhotoRealisticSettings, 6> NonPhotoRealisticSettings::PresetSettings =
    {
        NonPhotoRealisticSettings("Cartoon", true,
                                  true, 1, 1,
                                  false, false, 0, 0, 0),

        NonPhotoRealisticSettings("Pencil", false,
                                  true, 0.5f, 0.5f,
                                  true, false, 0.1f, 0.3f, 0.05f),

        NonPhotoRealisticSettings("Chunky Monochrome", true,
                                  true, 1.5f, 0.5f,
                                  true, false, 0, 0.35f, 0),

        NonPhotoRealisticSettings("Colored Hatching", false,
                                  true, 0.5f, 0.333f,
                                  true, true, 0.2f, 0.5f, 0.075f),

        NonPhotoRealisticSettings("Subtle Edge Enhancement", false,
                                  true, 0.5f, 0.5f,
                                  false, false, 0, 0, 0),

        NonPhotoRealisticSettings("Nothing Special", false,
                                  false, 0, 0,
                                  false, false, 0, 0, 0),
    };
}
