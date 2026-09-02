// SPDX-License-Identifier: MS-PL

#pragma once

#include <vector>

#include "CNA/CNAHelper.hpp"
#include "System/Collections/Generic/List.hpp"
#include "Tracks/TrackData.hpp"
#include "Tracks/TrackVertex.hpp"

namespace RacingGame::Landscapes
{
    class Landscape;
}

namespace RacingGame::Tracks
{
    class TrackLine
    {
    public:
        class RoadHelperPosition
        {
        public:
            TrackData::RoadHelper::HelperType type;
            int startNum;
            int endNum;

            RoadHelperPosition(TrackData::RoadHelper::HelperType setType,
                               int setStartNum, int setEndNum);
        };

        TrackLine(std::vector<Microsoft::Xna::Framework::Vector3> inputPoints,
                  TrackData::WidthHelperList widthHelpers,
                  TrackData::RoadHelperList roadHelpers,
                  TrackData::NeutralObjectList neutralObjects,
                  Landscapes::Landscape* landscape);
        explicit TrackLine(std::vector<Microsoft::Xna::Framework::Vector3> inputPoints);
        TrackLine(TrackData inputPointsFromTrack, Landscapes::Landscape* landscape);

        CNAEXT [[nodiscard]] const System::Collections::Generic::List<TrackVertex>&
        getPointsProperty() const;
        CNAEXT [[nodiscard]] const System::Collections::Generic::List<RoadHelperPosition>&
        getHelperPositionsProperty() const;

    protected:
        static constexpr int NumberOfIterationsPer100Meters = 40;
        System::Collections::Generic::List<TrackVertex> points;
        System::Collections::Generic::List<RoadHelperPosition> helperPositions;

        void Load(std::vector<Microsoft::Xna::Framework::Vector3> inputPoints,
                  TrackData::WidthHelperList widthHelpers,
                  TrackData::RoadHelperList roadHelpers,
                  TrackData::NeutralObjectList neutralObjects,
                  Landscapes::Landscape* landscape);
        void Load(TrackData trackData, Landscapes::Landscape* landscape);

    private:
        static constexpr float CurveFactor = 0.25f;
        static constexpr float UpFactorCorrector = 0.6f;
        static constexpr float RoadTextureStrechFactor = 0.125f;
        static constexpr int NumberOfUpSmoothValues = 10;
        static constexpr float MinimumLandscapeDistance = 2.0f;

        static const std::vector<Microsoft::Xna::Framework::Vector3> LoopingPoints;

        void AdjustRoadWidths(const TrackData::WidthHelperList& widthHelpers);
        void GenerateUTextureCoordinates();
        void GenerateTunnelsAndLandscapeObjects(
            TrackData::RoadHelperList roadHelpers,
            const TrackData::NeutralObjectList& neutralObjects,
            Landscapes::Landscape* landscape);
    };
}
