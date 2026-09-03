// SPDX-License-Identifier: MS-PL

#pragma once

#include "Microsoft/Xna/Framework/Rectangle.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class GraphicsDevice;
}

namespace RacingGame::Graphics
{
    /** @brief Applies the original BaseGame UI coordinate conversions to the live viewport. */
    class ResolutionMapper
    {
    public:
        /**
         * @brief Creates a mapper backed by the current graphics viewport.
         * @param device Graphics device providing the visible dimensions.
         */
        explicit ResolutionMapper(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);

        /** @brief Gets the visible width. */
        [[nodiscard]] int getWidthProperty() const;
        /** @brief Gets the visible height. */
        [[nodiscard]] int getHeightProperty() const;
        /** @brief Converts a 1024-wide X coordinate. */
        [[nodiscard]] int XToRes(int value) const;
        /** @brief Converts a 640-high Y coordinate. */
        [[nodiscard]] int YToRes(int value) const;
        /** @brief Converts a 768-high Y coordinate. */
        [[nodiscard]] int YToRes768(int value) const;
        /** @brief Converts a 1600-wide X coordinate. */
        [[nodiscard]] int XToRes1600(int value) const;
        /** @brief Converts a 1200-high Y coordinate. */
        [[nodiscard]] int YToRes1200(int value) const;
        /** @brief Converts a 1400-wide X coordinate. */
        [[nodiscard]] int XToRes1400(int value) const;
        /** @brief Converts a 1050-high Y coordinate. */
        [[nodiscard]] int YToRes1050(int value) const;

        /** @brief Converts a rectangle from the original 1024x640 UI space. */
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle CalcRectangle(
            int x, int y, int width, int height) const;
        /** @brief Converts and scales a rectangle around its original centre. */
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle
        CalcRectangleWithBounce(int x, int y, int width, int height,
                                float bounce) const;
        /** @brief Converts a rectangle from the original 1024x768 UI space. */
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle
        CalcRectangleKeep4To3(int x, int y, int width, int height) const;
        /** @brief Converts an existing rectangle from the 1024x768 UI space. */
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle
        CalcRectangleKeep4To3(
            const Microsoft::Xna::Framework::Rectangle& rectangle) const;
        /** @brief Converts a rectangle from the original 1600x1200 UI space. */
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle CalcRectangle1600(
            int x, int y, int width, int height) const;
        /** @brief Converts a rectangle from the original 2000x1500 UI space. */
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle CalcRectangle2000(
            int x, int y, int width, int height) const;
        /** @brief Converts a 4:3 rectangle aligned to the 16:9 bottom edge. */
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle
        CalcRectangleKeep4To3AlignBottom(
            int x, int y, int width, int height) const;
        /** @brief Converts a 4:3 rectangle aligned to the bottom-right edge. */
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle
        CalcRectangleKeep4To3AlignBottomRight(
            int x, int y, int width, int height) const;
        /** @brief Converts a centred UI rectangle while preserving source aspect. */
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle
        CalcRectangleCenteredWithGivenHeight(
            int x, int y, int height,
            const Microsoft::Xna::Framework::Rectangle& source) const;

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device;
        [[nodiscard]] static int Round(double value);
    };
}
