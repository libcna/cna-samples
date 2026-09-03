// SPDX-License-Identifier: MS-PL

#pragma once

#include <vector>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "Tracks/TrackLine.hpp"

namespace RacingGame::Landscapes
{
    class Landscape;
}

namespace RacingGame::Tracks
{
    /** @brief Represents the generated road and its gameplay coordinate system. */
    class Track : public TrackLine
    {
    public:
        /**
         * @brief Loads and generates a named original Racing track.
         *
         * @param setTrackName Original track content identifier.
         * @param landscape Landscape used to place the generated track.
         */
        Track(const SharpRuntime::String& setTrackName,
              Landscapes::Landscape& landscape);

        /**
         * @brief Gets the first generated track position.
         *
         * @return Track start position.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getStartPositionProperty() const;

        /**
         * @brief Gets the first generated track direction.
         *
         * @return Track start direction.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getStartDirectionProperty() const;

        /**
         * @brief Gets the first generated track up vector.
         *
         * @return Track start up vector.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getStartUpVectorProperty() const;

        /**
         * @brief Gets the nominal generated track length.
         *
         * @return Track length in meters.
         */
        [[nodiscard]] float getLengthProperty() const;

        /**
         * @brief Gets the number of generated track segments.
         *
         * @return Generated point count, including the closing point.
         */
        [[nodiscard]] int getNumberOfSegmentsProperty() const;

        /**
         * @brief Gets the generated checkpoint segment positions.
         *
         * @return Checkpoint segment indices in driving order.
         */
        [[nodiscard]] const std::vector<int>&
        getCheckpointSegmentPositionsProperty() const;

        /**
         * @brief Interpolates a coordinate system at a normalized track position.
         *
         * @param trackPositionPercent Normalized track position.
         * @param roadWidth Receives the interpolated road width.
         * @param nextRoadWidth Receives the following control-point road width.
         * @return Interpolated track coordinate matrix.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix GetTrackPositionMatrix(
            float trackPositionPercent, float& roadWidth,
            float& nextRoadWidth) const;

        /**
         * @brief Interpolates a coordinate system inside a numbered segment.
         *
         * @param trackSegmentNum Track segment number.
         * @param trackSegmentPercent Position inside the segment.
         * @param roadWidth Receives the interpolated road width.
         * @param nextRoadWidth Receives the next interpolated road width.
         * @return Interpolated track coordinate matrix.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix GetTrackPositionMatrix(
            int trackSegmentNum, float trackSegmentPercent, float& roadWidth,
            float& nextRoadWidth) const;

        /**
         * @brief Locates a car position in the generated track segments.
         *
         * @param carPos Current car position.
         * @param trackSegmentNumber In/out best known segment number.
         * @param trackSegmentPercent Receives the position inside the segment.
         */
        void UpdateCarTrackPosition(
            Microsoft::Xna::Framework::Vector3 carPos,
            int& trackSegmentNumber, float& trackSegmentPercent) const;

        /**
         * @brief Determines whether a segment belongs to a tunnel helper range.
         *
         * @param trackSegment Track segment number.
         * @return True when the segment is in a tunnel range.
         */
        [[nodiscard]] bool IsTunnel(int trackSegment) const;

    private:
        static constexpr float CheckpointGap = 500.0f;
        std::vector<int> checkpointSegmentPositions;
        void GenerateCheckpointSegmentPositions();
    };
}
