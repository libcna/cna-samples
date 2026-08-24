// SPDX-License-Identifier: MS-PL

#pragma once

#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace SafeArea
{
    /** @brief Ways to align text relative to a specified position. */
    enum class Alignment : int
    {
        /** @brief Aligns the left edge to the position. */
        Left = 0,
        /** @brief Aligns the right edge to the position. */
        Right = 1,
        /** @brief Horizontally centers the text on the position. */
        HorizontalCenter = 2,
        /** @brief Aligns the top edge to the position. */
        Top = 0,
        /** @brief Aligns the bottom edge to the position. */
        Bottom = 4,
        /** @brief Vertically centers the text on the position. */
        VerticalCenter = 8,
        /** @brief Aligns the top-left corner to the position. */
        TopLeft = 0,
        /** @brief Aligns the top-right corner to the position. */
        TopRight = 1,
        /** @brief Aligns the top center to the position. */
        TopCenter = 2,
        /** @brief Aligns the bottom-left corner to the position. */
        BottomLeft = 4,
        /** @brief Aligns the bottom-right corner to the position. */
        BottomRight = 5,
        /** @brief Aligns the bottom center to the position. */
        BottomCenter = 6,
        /** @brief Aligns the center-left edge to the position. */
        CenterLeft = 8,
        /** @brief Aligns the center-right edge to the position. */
        CenterRight = 9,
        /** @brief Centers the text on the position. */
        Center = 10,
    };

    /**
     * @brief Combines two alignment flags.
     *
     * @param left First alignment flag.
     * @param right Second alignment flag.
     * @return Combined alignment flags.
     */
    [[nodiscard]] inline Alignment operator|(Alignment left, Alignment right)
    {
        return static_cast<Alignment>(static_cast<int>(left) | static_cast<int>(right));
    }

    /**
     * @brief Tests whether an alignment contains a flag.
     *
     * @param value Alignment value.
     * @param flag Flag to test.
     * @return True when the flag is present.
     */
    [[nodiscard]] inline bool operator&(Alignment value, Alignment flag)
    {
        return (static_cast<int>(value) & static_cast<int>(flag)) != 0;
    }

    /** @brief SpriteBatch extension that aligns text around an anchor position. */
    class AlignedSpriteBatch : public Microsoft::Xna::Framework::Graphics::SpriteBatch
    {
    public:
        /**
         * @brief Creates an aligned sprite batch.
         *
         * @param graphicsDevice Graphics device used for drawing.
         */
        explicit AlignedSpriteBatch(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& graphicsDevice)
            : SpriteBatch(graphicsDevice)
        {
        }

        /**
         * @brief Draws a string using the specified anchor alignment.
         *
         * @param spriteFont Font used to draw the text.
         * @param text Text to draw.
         * @param position Alignment anchor position.
         * @param color Text color.
         * @param alignment Alignment relative to the anchor.
         */
        void DrawString(
            const Microsoft::Xna::Framework::Graphics::SpriteFont& spriteFont,
            const std::string& text,
            Microsoft::Xna::Framework::Vector2 position,
            Microsoft::Xna::Framework::Color color,
            Alignment alignment)
        {
            if (alignment & Alignment::Right)
            {
                position.X -= spriteFont.MeasureString(text).X;
            }
            else if (alignment & Alignment::HorizontalCenter)
            {
                position.X -= spriteFont.MeasureString(text).X / 2.0f;
            }

            if (alignment & Alignment::Bottom)
            {
                position.Y -= static_cast<float>(spriteFont.getLineSpacingProperty());
            }
            else if (alignment & Alignment::VerticalCenter)
            {
                position.Y -=
                    static_cast<float>(spriteFont.getLineSpacingProperty()) / 2.0f;
            }

            SpriteBatch::DrawString(spriteFont, text, position, color);
        }
    };
}
