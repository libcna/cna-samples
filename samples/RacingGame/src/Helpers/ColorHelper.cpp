// SPDX-License-Identifier: MS-PL

#include "Helpers/ColorHelper.hpp"

namespace RacingGame::Helpers
{
    using SharpRuntime::bytecs;
    using Microsoft::Xna::Framework::Color;

    const Color ColorHelper::Empty(0, 0, 0, 0);
    const Color ColorHelper::HalfAlpha(255, 255, 255, 128);

    float ColorHelper::StayInRange(
        const float value, const float minimum, const float maximum)
    {
        if (value < minimum)
            return minimum;
        if (value > maximum)
            return maximum;
        return value;
    }

    Color ColorHelper::MultiplyColors(const Color color1, const Color color2)
    {
        if (color1 == Color::White)
            return color2;
        if (color2 == Color::White)
            return color1;

        const float red1 = color1.getRProperty() / 255.0f;
        const float green1 = color1.getGProperty() / 255.0f;
        const float blue1 = color1.getBProperty() / 255.0f;
        const float alpha1 = color1.getAProperty() / 255.0f;
        const float red2 = color2.getRProperty() / 255.0f;
        const float green2 = color2.getGProperty() / 255.0f;
        const float blue2 = color2.getBProperty() / 255.0f;
        const float alpha2 = color2.getAProperty() / 255.0f;
        return Color(
            static_cast<bytecs>(StayInRange(red1 * red2, 0.0f, 1.0f) * 255.0f),
            static_cast<bytecs>(StayInRange(green1 * green2, 0.0f, 1.0f) * 255.0f),
            static_cast<bytecs>(StayInRange(blue1 * blue2, 0.0f, 1.0f) * 255.0f),
            static_cast<bytecs>(StayInRange(alpha1 * alpha2, 0.0f, 1.0f) * 255.0f));
    }

    bool ColorHelper::SameColor(const Color color, const Color checkColor)
    {
        return color.getRProperty() == checkColor.getRProperty() &&
               color.getGProperty() == checkColor.getGProperty() &&
               color.getBProperty() == checkColor.getBProperty();
    }

    Color ColorHelper::InterpolateColor(
        const Color color1, const Color color2, const float percent)
    {
        return Color(
            static_cast<bytecs>(color1.getRProperty() * (1.0f - percent) +
                                color2.getRProperty() * percent),
            static_cast<bytecs>(color1.getGProperty() * (1.0f - percent) +
                                color2.getGProperty() * percent),
            static_cast<bytecs>(color1.getBProperty() * (1.0f - percent) +
                                color2.getBProperty() * percent),
            static_cast<bytecs>(color1.getAProperty() * (1.0f - percent) +
                                color2.getAProperty() * percent));
    }

    Color ColorHelper::ApplyAlphaToColor(
        const Color color, const float newAlpha)
    {
        const float alpha = StayInRange(newAlpha, 0.0f, 1.0f);
        return Color(
            color.getRProperty(), color.getGProperty(), color.getBProperty(),
            static_cast<bytecs>(alpha * 255.0f));
    }

    Color ColorHelper::MixAlphaToColor(
        const Color color, const float newAlpha)
    {
        const float alpha = StayInRange(newAlpha, 0.0f, 1.0f);
        return Color(
            static_cast<bytecs>(color.getRProperty() * alpha),
            static_cast<bytecs>(color.getGProperty() * alpha),
            static_cast<bytecs>(color.getBProperty() * alpha),
            static_cast<bytecs>(alpha * 255.0f));
    }
}
