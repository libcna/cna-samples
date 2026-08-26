// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// BloomSettings.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "BloomSettings.hpp"

namespace BloomPostprocess
{
    BloomSettings::BloomSettings(const String& name, Single bloomThreshold, Single blurAmount,
                                 Single bloomIntensity, Single baseIntensity,
                                 Single bloomSaturation, Single baseSaturation)
        : Name(name),
          BloomThreshold(bloomThreshold),
          BlurAmount(blurAmount),
          BloomIntensity(bloomIntensity),
          BaseIntensity(baseIntensity),
          BloomSaturation(bloomSaturation),
          BaseSaturation(baseSaturation)
    {
    }

    std::array<BloomSettings, 6> BloomSettings::PresetSettings =
    {
        //                  Name           Thresh  Blur Bloom  Base  BloomSat BaseSat
        BloomSettings("Default",     0.25f,  4,   1.25f, 1,    1,       1),
        BloomSettings("Soft",        0,      3,   1,     1,    1,       1),
        BloomSettings("Desaturated", 0.5f,   8,   2,     1,    0,       1),
        BloomSettings("Saturated",   0.25f,  4,   2,     1,    2,       0),
        BloomSettings("Blurry",      0,      2,   1,     0.1f, 1,       1),
        BloomSettings("Subtle",      0.5f,   2,   1,     1,    1,       1),
    };
}
