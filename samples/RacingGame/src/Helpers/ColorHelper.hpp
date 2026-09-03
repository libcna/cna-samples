// SPDX-License-Identifier: MS-PL

#pragma once

#include "Microsoft/Xna/Framework/Color.hpp"

namespace RacingGame::Helpers
{
    /** @brief Supplies the original Racing color arithmetic helpers. */
    class ColorHelper final
    {
    public:
        /** @brief Transparent marker color. */
        static const Microsoft::Xna::Framework::Color Empty;
        /** @brief White with fifty-percent alpha. */
        static const Microsoft::Xna::Framework::Color HalfAlpha;

        /** @brief Multiplies two colors component by component. */
        [[nodiscard]] static Microsoft::Xna::Framework::Color MultiplyColors(
            Microsoft::Xna::Framework::Color color1,
            Microsoft::Xna::Framework::Color color2);
        /** @brief Compares RGB components while ignoring alpha. */
        [[nodiscard]] static bool SameColor(
            Microsoft::Xna::Framework::Color color,
            Microsoft::Xna::Framework::Color checkColor);
        /** @brief Linearly interpolates every color component. */
        [[nodiscard]] static Microsoft::Xna::Framework::Color InterpolateColor(
            Microsoft::Xna::Framework::Color color1,
            Microsoft::Xna::Framework::Color color2, float percent);
        /** @brief Replaces a color's alpha with a clamped normalized value. */
        [[nodiscard]] static Microsoft::Xna::Framework::Color ApplyAlphaToColor(
            Microsoft::Xna::Framework::Color color, float newAlpha);
        /** @brief Premultiplies RGB and replaces alpha with a normalized value. */
        [[nodiscard]] static Microsoft::Xna::Framework::Color MixAlphaToColor(
            Microsoft::Xna::Framework::Color color, float newAlpha);

    private:
        [[nodiscard]] static float StayInRange(float value,
                                               float minimum, float maximum);
    };
}
