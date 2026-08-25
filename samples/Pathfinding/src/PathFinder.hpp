// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PathFinder.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <limits>
#include <optional>
#include <string>

#include "Map.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/Collections/Generic/Dictionary.hpp"
#include "System/Collections/Generic/LinkedList.hpp"
#include "System/Collections/Generic/List.hpp"

namespace Pathfinding
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using Microsoft::Xna::Framework::Content::ContentManager;

    /** @brief Whether the search is stopped, running, finished or failed. */
    enum class SearchStatus
    {
        /** @brief The search is not running. */
        Stopped,
        /** @brief The search is running. */
        Searching,
        /** @brief The search finished without reaching the end tile. */
        NoPath,
        /** @brief The search reached the end tile. */
        PathFound,
    };

    /** @brief Which search algorithm the pathfinder is using. */
    enum class SearchMethod
    {
        /** @brief Looks at every path in the order it was seen. */
        BreadthFirst,
        /** @brief Always looks at whatever path is closest to the goal. */
        BestFirst,
        /** @brief Uses an admissible heuristic to find the best path. */
        AStar,
        /** @brief One past the last real method; the cycle wraps here. */
        Max,
    };

    /**
     * @brief The member name of a SearchMethod, as C#'s Enum.ToString() would render it.
     *
     * The HUD prints this, so the strings are the C# member names rather than anything
     * more C++-idiomatic.
     *
     * @param method The method to name.
     * @return Its member name.
     */
    [[nodiscard]] inline std::string SearchMethodToString(SearchMethod method)
    {
        switch (method)
        {
            case SearchMethod::BreadthFirst: return "BreadthFirst";
            case SearchMethod::BestFirst:    return "BestFirst";
            case SearchMethod::AStar:        return "AStar";
            case SearchMethod::Max:          return "Max";
        }
        return "Max";
    }

    /**
     * @brief Searches the map for a path from its start tile to its end tile.
     */
    class PathFinder
    {
        /**
         * Represents one node in the search space.
         *
         * A C# struct has value equality; C++ does not synthesise it, and openList.Remove
         * needs it, so it is written out here.
         */
        struct SearchNode
        {
            /** Location on the map */
            Point Position;
            /** Distance to goal estimate */
            int DistanceToGoal = 0;
            /** Distance traveled from the start */
            int DistanceTraveled = 0;

            SearchNode() = default;

            SearchNode(Point mapPosition, int distanceToGoal, int distanceTraveled)
                : Position(mapPosition),
                  DistanceToGoal(distanceToGoal),
                  DistanceTraveled(distanceTraveled)
            {
            }

            bool operator==(const SearchNode& other) const
            {
                return Position == other.Position &&
                       DistanceToGoal == other.DistanceToGoal &&
                       DistanceTraveled == other.DistanceTraveled;
            }
        };

        /** Scales the draw size of the search nodes */
        static constexpr float searchNodeDrawScale = 0.75f;

        // Draw data
        Texture2D nodeTexture;
        Vector2 nodeTextureCenter;
        Color openColor = Color::Green;
        Color closedColor = Color::Red;

        // How much time has passed since the last search step
        float timeSinceLastSearchStep = 0.0f;
        // Holds search nodes that are avaliable to search
        System::Collections::Generic::List<SearchNode> openList;
        // Holds the nodes that have already been searched
        System::Collections::Generic::List<SearchNode> closedList;
        // Holds all the paths we've creted so far
        System::Collections::Generic::Dictionary<Point, Point> paths;
        // The map we're searching
        Map* map = nullptr;

        SearchStatus searchStatus = SearchStatus::Stopped;
        SearchMethod searchMethod = SearchMethod::BestFirst;
        float scale = 0.0f;
        int totalSearchSteps = 0;

    public:
        /** Seconds per search step */
        float timeStep = 0.5f;

        /**
         * @brief Tells us if the search is stopped, started, finished or failed.
         * @return The search status.
         */
        [[nodiscard]] SearchStatus getSearchStatusProperty() const { return searchStatus; }

        /**
         * @brief Tells us which search type we're using right now.
         * @return The search method.
         */
        [[nodiscard]] SearchMethod getSearchMethodProperty() const { return searchMethod; }

        /**
         * @brief Gets the draw scale of the search nodes.
         * @return The scale already multiplied by the node draw scale.
         */
        [[nodiscard]] float getScaleProperty() const { return scale; }

        /**
         * @brief Sets the draw scale of the search nodes.
         * @param value Tile scale; the node draw scale is applied to it.
         */
        void setScaleProperty(float value) { scale = value * searchNodeDrawScale; }

        /**
         * @brief Gets the seconds per search step.
         * @return The time step.
         */
        [[nodiscard]] float getTimeStepProperty() const { return timeStep; }

        /**
         * @brief Sets the seconds per search step.
         * @param value The time step.
         */
        void setTimeStepProperty(float value) { timeStep = value; }

        /**
         * @brief Gets whether the search is currently running.
         * @return True while searching.
         */
        [[nodiscard]] bool getIsSearchingProperty() const
        {
            return searchStatus == SearchStatus::Searching;
        }

        /**
         * @brief Toggles searching on and off.
         *
         * As in the original, the assigned value is ignored: assigning anything flips a
         * stopped search to searching and a running one to stopped, and does nothing once
         * the search has finished.
         *
         * @param value Ignored.
         */
        void setIsSearchingProperty(bool value)
        {
            (void)value;
            if (searchStatus == SearchStatus::Searching)
            {
                searchStatus = SearchStatus::Stopped;
            }
            else if (searchStatus == SearchStatus::Stopped)
            {
                searchStatus = SearchStatus::Searching;
            }
        }

        /**
         * @brief How many search steps have elapsed on this map.
         * @return The step count.
         */
        [[nodiscard]] int getTotalSearchStepsProperty() const { return totalSearchSteps; }

        /**
         * @brief Setup search.
         * @param mazeMap Map to search.
         */
        void Initialize(Map& mazeMap)
        {
            searchStatus = SearchStatus::Stopped;
            openList.Clear();
            closedList.Clear();
            paths.Clear();
            map = &mazeMap;
        }

        /**
         * @brief Load the Draw texture.
         * @param content The content manager to load from.
         */
        void LoadContent(ContentManager& content)
        {
            nodeTexture = content.Load<Texture2D>("dot");
            nodeTextureCenter = Vector2((float)(nodeTexture.getWidthProperty() / 2),
                                        (float)(nodeTexture.getHeightProperty() / 2));
        }

        /**
         * @brief Search Update.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(const GameTime& gameTime)
        {
            if (searchStatus == SearchStatus::Searching)
            {
                timeSinceLastSearchStep +=
                    (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
                if (timeSinceLastSearchStep >= timeStep)
                {
                    DoSearchStep();
                    timeSinceLastSearchStep = 0.0f;
                }
            }
        }

        /**
         * @brief Draw the search space.
         * @param spriteBatch The sprite batch to draw with.
         */
        void Draw(SpriteBatch& spriteBatch) const
        {
            if (searchStatus != SearchStatus::PathFound)
            {
                spriteBatch.Begin();
                for (const SearchNode& node : openList)
                {
                    spriteBatch.Draw(nodeTexture,
                        map->MapToWorld(node.Position, true), std::nullopt, openColor, 0.0f,
                        nodeTextureCenter, scale, SpriteEffects::None, 0.0f);
                }
                for (const SearchNode& node : closedList)
                {
                    spriteBatch.Draw(nodeTexture,
                        map->MapToWorld(node.Position, true), std::nullopt, closedColor, 0.0f,
                        nodeTextureCenter, scale, SpriteEffects::None, 0.0f);
                }
                spriteBatch.End();
            }
        }

        /** @brief Reset the search. */
        void Reset()
        {
            searchStatus = SearchStatus::Stopped;
            totalSearchSteps = 0;
            setScaleProperty(map->getScaleProperty());
            openList.Clear();
            closedList.Clear();
            paths.Clear();
            openList.Add(SearchNode(map->getStartTileProperty(),
                Map::StepDistance(map->getStartTileProperty(), map->getEndTileProperty()),
                0));
        }

        /** @brief Cycle through the search method to the next type. */
        void NextSearchType()
        {
            searchMethod = (SearchMethod)(((int)searchMethod + 1) %
                (int)SearchMethod::Max);
        }

    private:
        /**
         * This method find the next path node to visit, puts that node on the
         * closed list and adds any nodes adjacent to the visited node to the
         * open list.
         */
        void DoSearchStep()
        {
            SearchNode newOpenListNode;

            bool foundNewNode = SelectNodeToVisit(newOpenListNode);
            if (foundNewNode)
            {
                Point currentPos = newOpenListNode.Position;
                for (const Point& point : map->OpenMapTiles(currentPos))
                {
                    SearchNode mapTile = SearchNode(point,
                        map->StepDistanceToEnd(point),
                        newOpenListNode.DistanceTraveled + 1);
                    if (!InList(openList, point) &&
                        !InList(closedList, point))
                    {
                        openList.Add(mapTile);
                        paths[point] = newOpenListNode.Position;
                    }
                }
                if (currentPos == map->getEndTileProperty())
                {
                    searchStatus = SearchStatus::PathFound;
                }
                openList.Remove(newOpenListNode);
                closedList.Add(newOpenListNode);
            }
            else
            {
                searchStatus = SearchStatus::NoPath;
            }
        }

        /** Determines if the given Point is inside the SearchNode list given */
        [[nodiscard]] static bool InList(
            const System::Collections::Generic::List<SearchNode>& list, Point point)
        {
            bool inList = false;
            for (const SearchNode& node : list)
            {
                if (node.Position == point)
                {
                    inList = true;
                }
            }
            return inList;
        }

        /**
         * This Method looks at everything in the open list and chooses the next
         * path to visit based on which search type is currently selected.
         *
         * The C# `out` parameter becomes a reference parameter here.
         */
        bool SelectNodeToVisit(SearchNode& result)
        {
            result = SearchNode();
            bool success = false;
            float smallestDistance = std::numeric_limits<float>::infinity();
            float currentDistance = 0.0f;
            if (openList.getCountProperty() > 0)
            {
                switch (searchMethod)
                {
                    // Breadth first search looks at every possible path in the
                    // order that we see them in.
                    case SearchMethod::BreadthFirst:
                        totalSearchSteps++;
                        result = openList.getItem(0);
                        success = true;
                        break;
                    // Best first search always looks at whatever path is closest to
                    // the goal regardless of how long that path is.
                    case SearchMethod::BestFirst:
                        totalSearchSteps++;
                        for (const SearchNode& node : openList)
                        {
                            currentDistance = (float)node.DistanceToGoal;
                            if (currentDistance < smallestDistance)
                            {
                                success = true;
                                result = node;
                                smallestDistance = currentDistance;
                            }
                        }
                        break;
                    // A* search uses a heuristic, an estimate, to try to find the
                    // best path to take. As long as the heuristic is admissible,
                    // meaning that it never over-estimates, it will always find
                    // the best path.
                    case SearchMethod::AStar:
                        totalSearchSteps++;
                        for (const SearchNode& node : openList)
                        {
                            currentDistance = Heuristic(node);
                            // The heuristic value gives us our optimistic estimate
                            // for the path length, while any path with the same
                            // heuristic value is equally "good" in this case we're
                            // favoring paths that have the same heuristic value
                            // but are longer.
                            if (currentDistance <= smallestDistance)
                            {
                                if (currentDistance < smallestDistance)
                                {
                                    success = true;
                                    result = node;
                                    smallestDistance = currentDistance;
                                }
                                else if (currentDistance == smallestDistance &&
                                    node.DistanceTraveled > result.DistanceTraveled)
                                {
                                    success = true;
                                    result = node;
                                    smallestDistance = currentDistance;
                                }
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            return success;
        }

        /**
         * Generates an optimistic estimate of the total path length to the goal
         * from the given position.
         */
        [[nodiscard]] static float Heuristic(const SearchNode& location)
        {
            return (float)(location.DistanceTraveled + location.DistanceToGoal);
        }

    public:
        /**
         * @brief Generates the path from start to end.
         * @return The path from start to end.
         */
        [[nodiscard]] System::Collections::Generic::LinkedList<Point> FinalPath() const
        {
            System::Collections::Generic::LinkedList<Point> path;
            if (searchStatus == SearchStatus::PathFound)
            {
                Point curPrev = map->getEndTileProperty();
                path.AddFirst(curPrev);
                while (paths.ContainsKey(curPrev))
                {
                    curPrev = paths[curPrev];
                    path.AddFirst(curPrev);
                }
            }
            return path;
        }
    };
}
