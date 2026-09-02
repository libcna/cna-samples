// SPDX-License-Identifier: MS-PL

#include "Tracks/TrackGeometry.hpp"

#include <cstddef>

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Tracks/TrackLine.hpp"

namespace RacingGame::Tracks
{
    using Graphics::TangentVertex;
    using Microsoft::Xna::Framework::Vector2;

    TrackGeometry::TrackGeometry(const TrackLine& trackLine)
    {
        const auto& points = trackLine.getPointsProperty();
        const int pointCount = points.getCountProperty();

        roadVertices.resize(static_cast<std::size_t>(pointCount * 5));
        for (int num = 0; num < pointCount; ++num)
        {
            const TrackVertex& point = points.getItem(num);
            roadVertices[static_cast<std::size_t>(num * 5 + 0)] =
                point.getRightTangentVertexProperty();
            roadVertices[static_cast<std::size_t>(num * 5 + 1)] =
                point.getMiddleRightTangentVertexProperty();
            roadVertices[static_cast<std::size_t>(num * 5 + 2)] =
                point.getMiddleTangentVertexProperty();
            roadVertices[static_cast<std::size_t>(num * 5 + 3)] =
                point.getMiddleLeftTangentVertexProperty();
            roadVertices[static_cast<std::size_t>(num * 5 + 4)] =
                point.getLeftTangentVertexProperty();
        }

        roadIndices.resize(static_cast<std::size_t>((pointCount - 1) * 24));
        int vertexIndex = 0;
        for (int num = 0; num < pointCount - 1; ++num)
        {
            for (int sideNum = 0; sideNum < 4; ++sideNum)
            {
                const std::size_t base =
                    static_cast<std::size_t>(num * 24 + 6 * sideNum);
                roadIndices[base + 0] = vertexIndex + sideNum;
                roadIndices[base + 1] = vertexIndex + 6 + sideNum;
                roadIndices[base + 2] = vertexIndex + 5 + sideNum;
                roadIndices[base + 3] = vertexIndex + 6 + sideNum;
                roadIndices[base + 4] = vertexIndex + sideNum;
                roadIndices[base + 5] = vertexIndex + 1 + sideNum;
            }
            vertexIndex += 5;
        }

        roadBackVertices.resize(static_cast<std::size_t>(pointCount * 4));
        for (int num = 0; num < pointCount; ++num)
        {
            const TrackVertex& point = points.getItem(num);
            TangentVertex& left = roadBackVertices[
                static_cast<std::size_t>(num * 4 + 0)];
            left = point.getLeftTangentVertexProperty();
            left.uv = Vector2(left.getUProperty() * RoadBackHullTextureWidthFactor,
                              0.0f);
            TangentVertex& bottomLeft = roadBackVertices[
                static_cast<std::size_t>(num * 4 + 1)];
            bottomLeft = point.getBottomLeftSideTangentVertexProperty();
            bottomLeft.uv = Vector2(
                left.getUProperty() * RoadBackHullTextureWidthFactor,
                RoadBackSideTextureHeight);
            TangentVertex& bottomRight = roadBackVertices[
                static_cast<std::size_t>(num * 4 + 2)];
            bottomRight = point.getBottomRightSideTangentVertexProperty();
            bottomRight.uv = Vector2(
                left.getUProperty() * RoadBackHullTextureWidthFactor,
                1.0f - RoadBackSideTextureHeight);
            TangentVertex& right = roadBackVertices[
                static_cast<std::size_t>(num * 4 + 3)];
            right = point.getRightTangentVertexProperty();
            right.uv = Vector2(
                right.getUProperty() * RoadBackHullTextureWidthFactor, 1.0f);
        }

        roadBackIndices.resize(static_cast<std::size_t>((pointCount - 1) * 18));
        vertexIndex = 0;
        for (int num = 0; num < pointCount - 1; ++num)
        {
            for (int sideNum = 0; sideNum < 3; ++sideNum)
            {
                const std::size_t base =
                    static_cast<std::size_t>(num * 18 + 6 * sideNum);
                roadBackIndices[base + 0] = vertexIndex + sideNum;
                roadBackIndices[base + 1] = vertexIndex + 5 + sideNum;
                roadBackIndices[base + 2] = vertexIndex + 4 + sideNum;
                roadBackIndices[base + 3] = vertexIndex + 5 + sideNum;
                roadBackIndices[base + 4] = vertexIndex + sideNum;
                roadBackIndices[base + 5] = vertexIndex + 1 + sideNum;
            }
            vertexIndex += 4;
        }

        int totalTunnelLength = 0;
        for (const TrackLine::RoadHelperPosition& helper :
             trackLine.getHelperPositionsProperty())
        {
            if (helper.type == TrackData::RoadHelper::HelperType::Tunnel)
                totalTunnelLength += 1 + helper.endNum - helper.startNum;
        }
        roadTunnelVertices.resize(
            static_cast<std::size_t>(totalTunnelLength * 4));
        vertexIndex = 0;
        for (const TrackLine::RoadHelperPosition& helper :
             trackLine.getHelperPositionsProperty())
        {
            if (helper.type != TrackData::RoadHelper::HelperType::Tunnel) continue;
            for (int num = helper.startNum; num <= helper.endNum; ++num)
            {
                const TrackVertex& point = points.getItem(num);
                TangentVertex& left = roadTunnelVertices[
                    static_cast<std::size_t>(vertexIndex + 0)];
                left = point.getLeftTangentVertexProperty();
                left.uv = Vector2(
                    left.getUProperty() * RoadTunnelTextureWidthFactor, 0.0f);
                TangentVertex& topLeft = roadTunnelVertices[
                    static_cast<std::size_t>(vertexIndex + 1)];
                topLeft = point.getTunnelTopLeftSideTangentVertexProperty();
                topLeft.uv = Vector2(
                    topLeft.getUProperty() * RoadTunnelTextureWidthFactor,
                    RoadTunnelSideTextureHeight);
                TangentVertex& topRight = roadTunnelVertices[
                    static_cast<std::size_t>(vertexIndex + 2)];
                topRight = point.getTunnelTopRightSideTangentVertexProperty();
                topRight.uv = Vector2(
                    topRight.getUProperty() * RoadTunnelTextureWidthFactor,
                    1.0f - RoadTunnelSideTextureHeight);
                TangentVertex& right = roadTunnelVertices[
                    static_cast<std::size_t>(vertexIndex + 3)];
                right = point.getRightTangentVertexProperty();
                right.uv = Vector2(
                    right.getUProperty() * RoadTunnelTextureWidthFactor, 1.0f);
                left.normal *= -1.0f;
                right.normal *= -1.0f;
                left.tangent *= -1.0f;
                right.tangent *= -1.0f;
                vertexIndex += 4;
            }
        }

        int totalTunnelSegments = 0;
        for (const TrackLine::RoadHelperPosition& helper :
             trackLine.getHelperPositionsProperty())
        {
            if (helper.type == TrackData::RoadHelper::HelperType::Tunnel)
                totalTunnelSegments += helper.endNum - helper.startNum;
        }
        roadTunnelIndices.resize(
            static_cast<std::size_t>(totalTunnelSegments * 18));
        vertexIndex = 0;
        int tunnelIndex = 0;
        for (const TrackLine::RoadHelperPosition& helper :
             trackLine.getHelperPositionsProperty())
        {
            if (helper.type != TrackData::RoadHelper::HelperType::Tunnel) continue;
            for (int num = helper.startNum; num < helper.endNum; ++num)
            {
                for (int sideNum = 0; sideNum < 3; ++sideNum)
                {
                    const std::size_t base = static_cast<std::size_t>(tunnelIndex);
                    roadTunnelIndices[base + 0] = vertexIndex + sideNum;
                    roadTunnelIndices[base + 2] = vertexIndex + 4 + sideNum;
                    roadTunnelIndices[base + 1] = vertexIndex + 5 + sideNum;
                    roadTunnelIndices[base + 3] = vertexIndex + 5 + sideNum;
                    roadTunnelIndices[base + 5] = vertexIndex + 1 + sideNum;
                    roadTunnelIndices[base + 4] = vertexIndex + sideNum;
                    tunnelIndex += 6;
                }
                vertexIndex += 4;
            }
            vertexIndex += 4;
        }
    }

    const std::vector<TangentVertex>& TrackGeometry::getRoadVerticesProperty() const
    {
        return roadVertices;
    }

    const std::vector<std::int32_t>& TrackGeometry::getRoadIndicesProperty() const
    {
        return roadIndices;
    }

    const std::vector<TangentVertex>&
    TrackGeometry::getRoadBackVerticesProperty() const
    {
        return roadBackVertices;
    }

    const std::vector<std::int32_t>&
    TrackGeometry::getRoadBackIndicesProperty() const
    {
        return roadBackIndices;
    }

    const std::vector<TangentVertex>&
    TrackGeometry::getRoadTunnelVerticesProperty() const
    {
        return roadTunnelVertices;
    }

    const std::vector<std::int32_t>&
    TrackGeometry::getRoadTunnelIndicesProperty() const
    {
        return roadTunnelIndices;
    }
}
