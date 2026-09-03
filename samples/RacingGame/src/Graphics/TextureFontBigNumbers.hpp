// SPDX-License-Identifier: MS-PL

#pragma once

#include <array>

#include "Microsoft/Xna/Framework/Rectangle.hpp"

namespace RacingGame::Graphics
{
    class ResolutionMapper;
    class Texture;

    /** @brief Renders the ten large digit glyphs embedded in `ingame.xnb`. */
    class TextureFontBigNumbers
    {
    public:
        /**
         * @brief Creates the large-number renderer.
         * @param ingameTexture Authentic Racing in-game atlas.
         * @param mapper Original Racing screen-coordinate mapper.
         */
        TextureFontBigNumbers(Texture& ingameTexture,
                              ResolutionMapper& mapper);

        /** @brief Draws a number at the original atlas scale. */
        [[nodiscard]] int WriteNumber(int x, int y, int number);
        /** @brief Draws a number at the original scale with alpha. */
        [[nodiscard]] int WriteNumber(int x, int y, int number, float alpha);
        /** @brief Draws a number using an explicit source-space height. */
        [[nodiscard]] int WriteNumber(int x, int y, int height, int number);
        /** @brief Draws a number centered horizontally at the original scale. */
        void WriteNumberCentered(int x, int y, int number);
        /** @brief Draws an alpha-blended number centered horizontally. */
        void WriteNumberCentered(int x, int y, int number, float alpha);
        /** @brief Gets digit sprites emitted by the most recent public call. */
        [[nodiscard]] int getLastDigitCountProperty() const;

    private:
        static const std::array<Microsoft::Xna::Framework::Rectangle, 10>
            BigNumberRectangles;

        Texture& ingame;
        ResolutionMapper& mapper;
        int lastDigitCount = 0;

        [[nodiscard]] int WriteDigit(int x, int y, int digit);
        [[nodiscard]] int WriteDigit(int x, int y, int height, int digit);
        [[nodiscard]] int WriteDigit(int x, int y, int digit, float alpha);
        [[nodiscard]] static int Round(double value);
    };
}
