// SPDX-License-Identifier: MS-PL

#include "Graphics/TextureFontBigNumbers.hpp"

#include <cmath>
#include <string>

#include "Graphics/ResolutionMapper.hpp"
#include "Graphics/Texture.hpp"
#include "Helpers/ColorHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"

namespace RacingGame::Graphics
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Rectangle;

    const std::array<Rectangle, 10>
        TextureFontBigNumbers::BigNumberRectangles{{
            {2, 342, 80, 133}, {84, 342, 80, 133},
            {167, 342, 80, 133}, {247, 342, 78, 133},
            {330, 342, 80, 133}, {411, 342, 80, 133},
            {495, 342, 80, 133}, {578, 342, 80, 133},
            {659, 342, 80, 133}, {749, 342, 80, 133},
        }};

    TextureFontBigNumbers::TextureFontBigNumbers(
        Texture& ingameTexture, ResolutionMapper& setMapper)
        : ingame(ingameTexture), mapper(setMapper)
    {
    }

    int TextureFontBigNumbers::Round(const double value)
    {
        return static_cast<int>(std::nearbyint(value));
    }

    int TextureFontBigNumbers::WriteDigit(
        const int x, const int y, const int digit)
    {
        if (digit < 0)
            return 0;
        const float scaleX = mapper.getWidthProperty() / 1600.0f;
        const float scaleY = mapper.getHeightProperty() / 1200.0f;
        const Rectangle& source = BigNumberRectangles[
            static_cast<std::size_t>(digit) % BigNumberRectangles.size()];
        ingame.RenderOnScreen(
            Rectangle(x, y, Round(source.Width * scaleX),
                      Round(source.Height * scaleY)),
            source);
        ++lastDigitCount;
        return Round(source.Width * scaleX);
    }

    int TextureFontBigNumbers::WriteDigit(
        const int x, const int y, const int height, const int digit)
    {
        if (digit < 0)
            return 0;
        const float scaleX = mapper.getWidthProperty() / 1600.0f;
        const float scaleY = mapper.getHeightProperty() / 1200.0f;
        const float scaleFactor =
            height / static_cast<float>(BigNumberRectangles[0].Height);
        const Rectangle& source = BigNumberRectangles[
            static_cast<std::size_t>(digit) % BigNumberRectangles.size()];
        ingame.RenderOnScreen(
            Rectangle(x, y, Round(source.Width * scaleX * scaleFactor),
                      Round(source.Height * scaleY * scaleFactor)),
            source);
        ++lastDigitCount;
        return Round(source.Width * scaleX * scaleFactor);
    }

    int TextureFontBigNumbers::WriteDigit(
        const int x, const int y, const int digit, const float alpha)
    {
        const float scaleX = mapper.getWidthProperty() / 1600.0f;
        const float scaleY = mapper.getHeightProperty() / 1200.0f;
        const Rectangle& source = BigNumberRectangles[
            static_cast<std::size_t>(digit) % BigNumberRectangles.size()];
        ingame.RenderOnScreen(
            Rectangle(x, y, Round(source.Width * scaleX),
                      Round(source.Height * scaleY)),
            source,
            Helpers::ColorHelper::ApplyAlphaToColor(Color::White, alpha));
        ++lastDigitCount;
        return Round(source.Width * scaleX);
    }

    int TextureFontBigNumbers::WriteNumber(
        const int x, const int y, const int number)
    {
        lastDigitCount = 0;
        int width = 0;
        for (const char character : std::to_string(number))
            width += WriteDigit(x + width, y, character - '0');
        return width;
    }

    int TextureFontBigNumbers::WriteNumber(
        const int x, const int y, const int number, const float alpha)
    {
        lastDigitCount = 0;
        int width = 0;
        for (const char character : std::to_string(number))
            width += WriteDigit(x + width, y, character - '0', alpha);
        return width;
    }

    int TextureFontBigNumbers::WriteNumber(
        const int x, const int y, const int height, const int number)
    {
        lastDigitCount = 0;
        int width = 0;
        for (const char character : std::to_string(number))
            width += WriteDigit(x + width, y, height, character - '0');
        return width;
    }

    void TextureFontBigNumbers::WriteNumberCentered(
        const int x, const int y, const int number)
    {
        const std::string text = std::to_string(number);
        (void)WriteNumber(
            static_cast<int>(x -
                (text.size() * BigNumberRectangles[0].Width / 2.0f) *
                (mapper.getWidthProperty() / 1600.0f)),
            y, number);
    }

    void TextureFontBigNumbers::WriteNumberCentered(
        const int x, const int y, const int number, const float alpha)
    {
        const std::string text = std::to_string(number);
        (void)WriteNumber(
            static_cast<int>(x -
                (text.size() * BigNumberRectangles[0].Width / 2.0f) *
                (mapper.getWidthProperty() / 1600.0f)),
            y, number, alpha);
    }

    int TextureFontBigNumbers::getLastDigitCountProperty() const
    {
        return lastDigitCount;
    }
}
