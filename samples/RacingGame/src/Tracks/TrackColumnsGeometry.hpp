// SPDX-License-Identifier: MS-PL

#pragma once

#include <cstdint>
#include <vector>

#include "Graphics/TangentVertex.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace RacingGame::Landscapes
{
    class Landscape;
}

namespace RacingGame::Tracks
{
    class TrackLine;

    /** @brief Owns the CPU mesh and model positions for track support columns. */
    class TrackColumnsGeometry
    {
    public:
        /**
         * @brief Generates track support columns above the landscape.
         *
         * @param trackLine Source road line.
         * @param landscape Landscape height provider.
         */
        TrackColumnsGeometry(const TrackLine& trackLine,
                             const Landscapes::Landscape& landscape);

        /** @brief Gets accepted column spline positions. */
        [[nodiscard]] const std::vector<Microsoft::Xna::Framework::Vector3>&
        getColumnPositionsProperty() const;
        /** @brief Gets generated column vertices. */
        [[nodiscard]] const std::vector<Graphics::TangentVertex>&
        getVerticesProperty() const;
        /** @brief Gets generated column triangle indices. */
        [[nodiscard]] const std::vector<std::int32_t>& getIndicesProperty() const;
        /** @brief Gets positions for the original RoadColumnSegment models. */
        [[nodiscard]] const std::vector<Microsoft::Xna::Framework::Vector3>&
        getSegmentPositionsProperty() const;

    private:
        std::vector<Microsoft::Xna::Framework::Vector3> columnPositions;
        std::vector<Graphics::TangentVertex> vertices;
        std::vector<std::int32_t> indices;
        std::vector<Microsoft::Xna::Framework::Vector3> segmentPositions;
    };
}
