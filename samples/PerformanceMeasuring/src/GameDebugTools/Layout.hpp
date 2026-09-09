// SPDX-License-Identifier: MS-PL
#pragma once

// Layout.hpp — C++ port of GameDebugTools/Layout.cs (XNA 4.0 PerformanceMeasuring sample).

#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"

namespace PerformanceMeasuring::GameDebugTools {

using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::Viewport;

/**
 * @brief Where a rectangle sits inside its client area.
 *
 * A bit-flag set, matching the original's `[Flags]` enum: one horizontal flag combines with one
 * vertical flag, and the named pairs below are the combinations the sample uses.
 */
enum class Alignment {
    /** @brief No alignment; the rectangle keeps its own position. */
    None = 0,

    /** @brief Against the left edge. */
    Left = 1,
    /** @brief Against the right edge. */
    Right = 2,
    /** @brief Centred horizontally. */
    HorizontalCenter = 4,

    /** @brief Against the top edge. */
    Top = 8,
    /** @brief Against the bottom edge. */
    Bottom = 16,
    /** @brief Centred vertically. */
    VerticalCenter = 32,

    /** @brief Top-left corner. */
    TopLeft = Top | Left,
    /** @brief Top-right corner. */
    TopRight = Top | Right,
    /** @brief Centred along the top edge. */
    TopCenter = Top | HorizontalCenter,

    /** @brief Bottom-left corner. */
    BottomLeft = Bottom | Left,
    /** @brief Bottom-right corner. */
    BottomRight = Bottom | Right,
    /** @brief Centred along the bottom edge. */
    BottomCenter = Bottom | HorizontalCenter,

    /** @brief Centred against the left edge. */
    CenterLeft = VerticalCenter | Left,
    /** @brief Centred against the right edge. */
    CenterRight = VerticalCenter | Right,
    /** @brief Centred both ways. */
    Center = VerticalCenter | HorizontalCenter
};

/** @brief Combines two alignment flags. @param a First flag. @param b Second flag. @return The combination. */
constexpr Alignment operator|(Alignment a, Alignment b) {
    return static_cast<Alignment>(static_cast<int>(a) | static_cast<int>(b));
}

/** @brief Tests alignment flags. @param a Flags to test. @param b Flag to look for. @return Non-zero when set. */
constexpr int operator&(Alignment a, Alignment b) {
    return static_cast<int>(a) & static_cast<int>(b);
}

/**
 * @brief Places rectangles by alignment and margin, keeping them inside the title-safe area.
 *
 * The margins are fractions of the client area rather than pixels, so a layout holds on any
 * backbuffer size; whatever they produce is then clamped into the safe area.
 */
struct Layout {
    /** @brief Area the alignment and margins are measured against. */
    Rectangle ClientArea;
    /** @brief Area the result is clamped into; on a television this excludes the overscan. */
    Rectangle SafeArea;

    /** @brief Constructs an empty layout. */
    Layout() = default;

    /**
     * @brief Constructs a layout with distinct client and safe areas.
     *
     * @param clientArea Area to place within.
     * @param safeArea   Area to clamp into.
     */
    Layout(Rectangle clientArea, Rectangle safeArea)
        : ClientArea(clientArea), SafeArea(safeArea) {}

    /**
     * @brief Constructs a layout whose safe area is the whole client area.
     *
     * @param clientArea Area to place within and clamp into.
     */
    explicit Layout(Rectangle clientArea)
        : Layout(clientArea, clientArea) {}

    /**
     * @brief Constructs a layout from a viewport, taking its own title-safe area.
     *
     * @param viewport Viewport supplying both areas.
     */
    explicit Layout(const Viewport& viewport)
        : ClientArea(viewport.getBoundsProperty()),
          SafeArea(viewport.getTitleSafeAreaProperty()) {}

    /**
     * @brief Places a size and returns where its top-left corner lands.
     *
     * @param size             Extent to place.
     * @param horizontalMargin Left/right margin, as a fraction of the client width.
     * @param verticalMargin   Top/bottom margin, as a fraction of the client height.
     * @param alignment        Where in the client area to put it.
     * @return The top-left corner of the placed region.
     */
    Vector2 Place(Vector2 size, float horizontalMargin, float verticalMargin, Alignment alignment) const {
        Rectangle rc(0, 0, (int)size.X, (int)size.Y);
        rc = Place(rc, horizontalMargin, verticalMargin, alignment);
        return Vector2((float)rc.X, (float)rc.Y);
    }

    /**
     * @brief Places a rectangle and clamps it into the safe area.
     *
     * @param region           Rectangle to place; only its position changes.
     * @param horizontalMargin Left/right margin, as a fraction of the client width.
     * @param verticalMargin   Top/bottom margin, as a fraction of the client height.
     * @param alignment        Where in the client area to put it.
     * @return The placed rectangle.
     */
    Rectangle Place(Rectangle region, float horizontalMargin, float verticalMargin, Alignment alignment) const {
        // Horizontal layout.
        if ((alignment & Alignment::Left) != 0) {
            region.X = ClientArea.X + (int)((float)ClientArea.Width * horizontalMargin);
        } else if ((alignment & Alignment::Right) != 0) {
            region.X = ClientArea.X + (int)((float)ClientArea.Width * (1.0f - horizontalMargin)) - region.Width;
        } else if ((alignment & Alignment::HorizontalCenter) != 0) {
            region.X = ClientArea.X + (ClientArea.Width - region.Width) / 2 +
                       (int)(horizontalMargin * (float)ClientArea.Width);
        }

        // Vertical layout.
        if ((alignment & Alignment::Top) != 0) {
            region.Y = ClientArea.Y + (int)((float)ClientArea.Height * verticalMargin);
        } else if ((alignment & Alignment::Bottom) != 0) {
            region.Y = ClientArea.Y + (int)((float)ClientArea.Height * (1.0f - verticalMargin)) - region.Height;
        } else if ((alignment & Alignment::VerticalCenter) != 0) {
            region.Y = ClientArea.Y + (ClientArea.Height - region.Height) / 2 +
                       (int)(verticalMargin * (float)ClientArea.Height);
        }

        // Make sure the layout region is in the safe area.
        if (region.getLeftProperty() < SafeArea.getLeftProperty())
            region.X = SafeArea.getLeftProperty();

        if (region.getRightProperty() > SafeArea.getRightProperty())
            region.X = SafeArea.getRightProperty() - region.Width;

        if (region.getTopProperty() < SafeArea.getTopProperty())
            region.Y = SafeArea.getTopProperty();

        if (region.getBottomProperty() > SafeArea.getBottomProperty())
            region.Y = SafeArea.getBottomProperty() - region.Height;

        return region;
    }
};

} // namespace PerformanceMeasuring::GameDebugTools
