// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Map.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>
#include <vector>

#include "MapData.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/Math.hpp"

namespace Pathfinding
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using PathfindingData::MapData;

    /** @brief The kind of thing occupying one map tile. */
    enum class MapTileType
    {
        /** @brief Nothing; the tile can be entered. */
        MapEmpty,
        /** @brief A barrier; the tile cannot be entered. */
        MapBarrier,
        /** @brief The tile the tank starts on. */
        MapStart,
        /** @brief The tile the tank is trying to reach. */
        MapExit
    };

    /**
     * @brief The grid the tank drives on and the pathfinder searches.
     */
    class Map
    {
        // Draw data
        Texture2D tileTexture;
        Vector2 tileSquareCenter;
        Texture2D dotTexture;
        Vector2 dotTextureCenter;
        Texture2D barrierTexture;
        Color tileColor1 = Color::Navy;
        Color tileColor2 = Color::LightBlue;
        Color startColor = Color::Green;
        Color exitColor = Color::Red;

        // Map data
        std::vector<MapData> maps;
        // C# declares this as MapTileType[numberColumns, numberRows]; the port keeps one
        // flat vector with the same [column, row] indexing.
        std::vector<MapTileType> mapTiles;
        int currentMap = 0;
        int numberColumns = 0;
        int numberRows = 0;

        float tileSize = 0.0f;
        float scale = 0.0f;
        Point startTile;
        Point endTile;
        bool mapReload = false;

        [[nodiscard]] MapTileType& tileAt(int column, int row)
        {
            return mapTiles[(std::size_t)column * (std::size_t)numberRows + (std::size_t)row];
        }

        [[nodiscard]] MapTileType tileAt(int column, int row) const
        {
            return mapTiles[(std::size_t)column * (std::size_t)numberRows + (std::size_t)row];
        }

    public:
        /**
         * @brief Gets the height/width of a tile square.
         * @return The tile size in pixels.
         */
        [[nodiscard]] float getTileSizeProperty() const { return tileSize; }

        /**
         * @brief Gets the draw scale as a percentage of TileSize.
         * @return The draw scale.
         */
        [[nodiscard]] float getScaleProperty() const { return scale; }

        /**
         * @brief Gets the start position on the Map.
         * @return The start tile.
         */
        [[nodiscard]] Point getStartTileProperty() const { return startTile; }

        /**
         * @brief Gets the end position in the Map.
         * @return The end tile.
         */
        [[nodiscard]] Point getEndTileProperty() const { return endTile; }

        /**
         * @brief Gets whether the Map data has changed.
         * @return True when the map must be reloaded.
         */
        [[nodiscard]] bool getMapReloadProperty() const { return mapReload; }

        /**
         * @brief Sets whether the Map data must be reloaded.
         * @param value True to request a reload.
         */
        void setMapReloadProperty(bool value) { mapReload = value; }

        /**
         * @brief Load Draw textures and Map data.
         * @param content The content manager to load from.
         */
        void LoadContent(ContentManager& content)
        {
            tileTexture = content.Load<Texture2D>("whiteTile");
            barrierTexture = content.Load<Texture2D>("barrier");
            dotTexture = content.Load<Texture2D>("dot");

            dotTextureCenter = Vector2((float)(dotTexture.getWidthProperty() / 2),
                                       (float)(dotTexture.getHeightProperty() / 2));

            maps.clear();
            maps.push_back(content.Load<MapData>("map1"));
            maps.push_back(content.Load<MapData>("map2"));
            maps.push_back(content.Load<MapData>("map3"));
            maps.push_back(content.Load<MapData>("map4"));

            ReloadMap();

            mapReload = true;
        }

        /**
         * @brief Draw the map and all its elements.
         * @param spriteBatch The sprite batch to draw with.
         */
        void Draw(SpriteBatch& spriteBatch) const
        {
            spriteBatch.Begin();

            // These two loops go through each tile in the map, starting at the upper
            // left and going to the upper right, then repeating for the next row of
            // tiles until the end
            for (int i = 0; i < numberRows; i++)
            {
                for (int j = 0; j < numberColumns; j++)
                {
                    // Get the screen coordinates of the tile
                    Vector2 tilePosition = MapToWorld(j, i, false);

                    // Alternate between the 2 tile colors to create a checker pattern
                    Color currentColor = (i + j) % 2 == 1 ? tileColor1 : tileColor2;

                    // Draw the tile
                    spriteBatch.Draw(tileTexture, tilePosition, std::nullopt, currentColor, 0.0f,
                                     Vector2::Zero, scale, SpriteEffects::None, 0.0f);

                    // If the current tile is a type with a special draw element, the
                    // start location, the end location or a barrier, then draw that.
                    switch (tileAt(j, i))
                    {
                        case MapTileType::MapBarrier:
                            spriteBatch.Draw(
                                barrierTexture, tilePosition, std::nullopt, Color::White,
                                0.0f, Vector2::Zero, scale, SpriteEffects::None, 0.25f);
                            break;
                        case MapTileType::MapStart:
                            spriteBatch.Draw(
                                dotTexture, tilePosition + tileSquareCenter, std::nullopt,
                                startColor, 0.0f, dotTextureCenter, scale,
                                SpriteEffects::None, 0.25f);
                            break;
                        case MapTileType::MapExit:
                            spriteBatch.Draw(
                                dotTexture, tilePosition + tileSquareCenter, std::nullopt,
                                exitColor, 0.0f, dotTextureCenter, scale,
                                SpriteEffects::None, 0.25f);
                            break;
                        default:
                            break;
                    }
                }
            }

            spriteBatch.End();
        }

        /**
         * @brief Translates a map tile location into a screen position.
         *
         * @param column Column position (x).
         * @param row Row position (y).
         * @param centered True to return the centre of the tile, false for its upper-left corner.
         * @return The screen position.
         */
        [[nodiscard]] Vector2 MapToWorld(int column, int row, bool centered) const
        {
            Vector2 screenPosition = Vector2();

            if (InMap(column, row))
            {
                screenPosition.X = column * tileSize;
                screenPosition.Y = row * tileSize;
                if (centered)
                {
                    screenPosition += tileSquareCenter;
                }
            }
            else
            {
                screenPosition = Vector2::Zero;
            }
            return screenPosition;
        }

        /**
         * @brief Translates a map tile location into a screen position.
         *
         * @param location Map location.
         * @param centered True to return the centre of the tile, false for its upper-left corner.
         * @return The screen position.
         */
        [[nodiscard]] Vector2 MapToWorld(Point location, bool centered) const
        {
            Vector2 screenPosition = Vector2();

            if (InMap(location.X, location.Y))
            {
                screenPosition.X = location.X * tileSize;
                screenPosition.Y = location.Y * tileSize;
                if (centered)
                {
                    screenPosition += tileSquareCenter;
                }
            }
            else
            {
                screenPosition = Vector2::Zero;
            }
            return screenPosition;
        }

    private:
        /** Returns true if the given map location exists */
        [[nodiscard]] bool InMap(int column, int row) const
        {
            return (row >= 0 && row < numberRows &&
                    column >= 0 && column < numberColumns);
        }

        /** Returns true if the given map location exists and is not blocked by a barrier */
        [[nodiscard]] bool IsOpen(int column, int row) const
        {
            return InMap(column, row) && tileAt(column, row) != MapTileType::MapBarrier;
        }

    public:
        /**
         * @brief Enumerate all the map locations that can be entered from the given map location.
         *
         * The original is a C# iterator method returning IEnumerable<Point>; C++ has no
         * `yield return`, so the same four candidates are collected in the same order.
         *
         * @param mapLoc The location to look around.
         * @return The enterable neighbours, in the original's order.
         */
        [[nodiscard]] std::vector<Point> OpenMapTiles(Point mapLoc) const
        {
            std::vector<Point> open;
            if (IsOpen(mapLoc.X, mapLoc.Y + 1))
                open.push_back(Point(mapLoc.X, mapLoc.Y + 1));
            if (IsOpen(mapLoc.X, mapLoc.Y - 1))
                open.push_back(Point(mapLoc.X, mapLoc.Y - 1));
            if (IsOpen(mapLoc.X + 1, mapLoc.Y))
                open.push_back(Point(mapLoc.X + 1, mapLoc.Y));
            if (IsOpen(mapLoc.X - 1, mapLoc.Y))
                open.push_back(Point(mapLoc.X - 1, mapLoc.Y));
            return open;
        }

        /**
         * @brief Create a viewport for the Map based on the passed in viewport and the size of
         *        the map, scaling the graphics to fit.
         * @param safeViewableArea Screen viewport.
         */
        void UpdateMapViewport(Rectangle safeViewableArea)
        {
            // This finds the largest sized tiles we can draw while still keeping
            // everything in the given viewable area
            tileSize = System::Math::Min(safeViewableArea.Height / (float)numberRows,
                                         safeViewableArea.Width / (float)numberColumns);

            scale = tileSize / (float)tileTexture.getHeightProperty();
            tileSquareCenter = Vector2(tileSize / 2);
        }

        /**
         * @brief Finds the minimum number of tiles it takes to move from Point A to Point B if
         *        there are no barriers in the way.
         *
         * @param pointA Start position.
         * @param pointB End position.
         * @return Distance in tiles.
         */
        [[nodiscard]] static int StepDistance(Point pointA, Point pointB)
        {
            int distanceX = System::Math::Abs(pointA.X - pointB.X);
            int distanceY = System::Math::Abs(pointA.Y - pointB.Y);

            return distanceX + distanceY;
        }

        /**
         * @brief Finds the minimum number of tiles it takes to move from the current position to
         *        the end location on the Map if there are no barriers in the way.
         *
         * @param point Current position.
         * @return Distance to end in tiles.
         */
        [[nodiscard]] int StepDistanceToEnd(Point point) const
        {
            return StepDistance(point, endTile);
        }

        /** @brief Load the next map. */
        void CycleMap()
        {
            currentMap = (currentMap + 1) % (int)maps.size();

            mapReload = true;
        }

        /** @brief Reload map data. */
        void ReloadMap()
        {
            // Set the map height and width
            numberColumns = maps[(std::size_t)currentMap].NumberColumns;
            numberRows = maps[(std::size_t)currentMap].NumberRows;

            // Recreate the tile array
            mapTiles.assign((std::size_t)numberColumns * (std::size_t)numberRows,
                            MapTileType::MapEmpty);

            // Set the start
            startTile = maps[(std::size_t)currentMap].Start;
            tileAt(startTile.X, startTile.Y) = MapTileType::MapStart;

            // Set the end
            endTile = maps[(std::size_t)currentMap].End;
            tileAt(endTile.X, endTile.Y) = MapTileType::MapExit;

            int x = 0;
            int y = 0;
            // Set the barriers
            for (std::size_t i = 0; i < maps[(std::size_t)currentMap].Barriers.size(); i++)
            {
                x = maps[(std::size_t)currentMap].Barriers[i].X;
                y = maps[(std::size_t)currentMap].Barriers[i].Y;

                tileAt(x, y) = MapTileType::MapBarrier;
            }

            mapReload = false;
        }
    };
}
