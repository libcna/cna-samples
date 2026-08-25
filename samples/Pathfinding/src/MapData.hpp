// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// MapData.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <utility>
#include <vector>

#include "Microsoft/Xna/Framework/Point.hpp"

namespace PathfindingData
{
    using Microsoft::Xna::Framework::Point;

    /**
     * @brief The map layout deserialized from each of the sample's four map assets.
     *
     * Upstream this type lives in its own class library, `PathfindingData`, which the game
     * and the content project both reference. The library exists only to carry this type,
     * so the port keeps the type and its namespace and drops the project file around it.
     */
    class MapData
    {
    public:
        /** @brief Number of tile rows in the map. */
        int NumberRows = 0;
        /** @brief Number of tile columns in the map. */
        int NumberColumns = 0;
        /** @brief Tile the tank starts on. */
        Point Start;
        /** @brief Tile the tank is trying to reach. */
        Point End;
        /** @brief Tiles that cannot be entered. */
        std::vector<Point> Barriers;

        /** @brief Constructs an empty map. */
        MapData() = default;

        /**
         * @brief Constructs a map from its layout.
         *
         * @param columns Number of tile columns.
         * @param rows Number of tile rows.
         * @param startPosition Tile the tank starts on.
         * @param endPosition Tile the tank is trying to reach.
         * @param barriersList Tiles that cannot be entered.
         */
        MapData(int columns, int rows, Point startPosition, Point endPosition,
                std::vector<Point> barriersList)
            : NumberRows(rows),
              NumberColumns(columns),
              Start(startPosition),
              End(endPosition),
              Barriers(std::move(barriersList))
        {
        }
    };
}
