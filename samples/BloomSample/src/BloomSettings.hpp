// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// BloomSettings.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <array>

#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace BloomPostprocess
{
    using SharpRuntime::Single;
    using SharpRuntime::String;

    /**
     * @brief Class holds all the settings used to tweak the bloom effect.
     */
    class BloomSettings
    {
    public:
        /**
         * @brief Constructs a new bloom settings descriptor.
         *
         * @param name            Name of a preset bloom setting, for display to the user.
         * @param bloomThreshold  How bright a pixel needs to be before it will bloom.
         * @param blurAmount      How much blurring is applied to the bloom image.
         * @param bloomIntensity  Amount of the bloom image mixed into the final scene.
         * @param baseIntensity   Amount of the base image mixed into the final scene.
         * @param bloomSaturation Color saturation of the bloom image.
         * @param baseSaturation  Color saturation of the base image.
         */
        BloomSettings(const String& name, Single bloomThreshold, Single blurAmount,
                      Single bloomIntensity, Single baseIntensity,
                      Single bloomSaturation, Single baseSaturation);

        /** @brief Name of a preset bloom setting, for display to the user. */
        String Name;

        /**
         * @brief Controls how bright a pixel needs to be before it will bloom.
         *
         * Zero makes everything bloom equally, while higher values select only brighter
         * colors. Somewhere between 0.25 and 0.5 is good.
         */
        Single BloomThreshold;

        /**
         * @brief Controls how much blurring is applied to the bloom image.
         *
         * The typical range is from 1 up to 10 or so.
         */
        Single BlurAmount;

        /** @brief Amount of the bloom image that will be mixed into the final scene. */
        Single BloomIntensity;

        /** @brief Amount of the base image that will be mixed into the final scene. */
        Single BaseIntensity;

        /**
         * @brief Color saturation of the bloom image.
         *
         * Zero is totally desaturated, 1.0 leaves saturation unchanged, while higher
         * values increase the saturation level.
         */
        Single BloomSaturation;

        /** @brief Color saturation of the base image. */
        Single BaseSaturation;

        /** @brief Table of preset bloom settings, used by the sample program. */
        static std::array<BloomSettings, 6> PresetSettings;
    };
}
