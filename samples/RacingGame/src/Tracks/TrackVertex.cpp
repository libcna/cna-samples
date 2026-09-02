// SPDX-License-Identifier: MS-PL

#include "Tracks/TrackVertex.hpp"

namespace RacingGame::Tracks
{
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;

    TrackVertex TrackVertex::getLeftTrackVertexProperty() const
    {
        return TrackVertex(pos - RoadWidthScale * roadWidth * right / 2.0f,
                           right, up, dir, Vector2(uv.X, 0.0f), roadWidth);
    }

    TrackVertex TrackVertex::getRightTrackVertexProperty() const
    {
        return TrackVertex(pos + RoadWidthScale * roadWidth * right / 2.0f,
                           right, up, dir, Vector2(uv.X, roadWidth), roadWidth);
    }

    Graphics::TangentVertex TrackVertex::getLeftTangentVertexProperty() const
    {
        return {pos - RoadWidthScale * roadWidth * right / 2.0f,
                Vector2(uv.X, 0.0f), up, right};
    }

    Graphics::TangentVertex TrackVertex::getRightTangentVertexProperty() const
    {
        return {pos + RoadWidthScale * roadWidth * right / 2.0f,
                Vector2(uv.X, roadWidth), up, right};
    }

    Graphics::TangentVertex TrackVertex::getMiddleTangentVertexProperty() const
    {
        return {pos, Vector2(uv.X, roadWidth / 2.0f), up, right};
    }

    Graphics::TangentVertex TrackVertex::getMiddleLeftTangentVertexProperty() const
    {
        return {pos - RoadWidthScale * roadWidth * right / 4.0f,
                Vector2(uv.X, roadWidth / 4.0f), up, right};
    }

    Graphics::TangentVertex TrackVertex::getMiddleRightTangentVertexProperty() const
    {
        return {pos + RoadWidthScale * roadWidth * right / 4.0f,
                Vector2(uv.X, roadWidth * 3.0f / 4.0f), up, right};
    }

    Graphics::TangentVertex TrackVertex::getBottomLeftSideTangentVertexProperty() const
    {
        return {pos - RoadWidthScale * roadWidth * right / 2.0f -
                    up * RoadThickness * roadWidth,
                Vector2(uv.X, 0.0f), -up, -right};
    }

    Graphics::TangentVertex TrackVertex::getBottomRightSideTangentVertexProperty() const
    {
        return {pos + RoadWidthScale * roadWidth * right / 2.0f -
                    up * RoadThickness * roadWidth,
                Vector2(uv.X, 1.0f), -up, -right};
    }

    Graphics::TangentVertex TrackVertex::getTunnelTopLeftSideTangentVertexProperty() const
    {
        return {pos - RoadWidthScale * roadWidth * right / 2.0f +
                    up * RoadTunnelHeight,
                Vector2(uv.X, 0.0f), -up, -right};
    }

    Graphics::TangentVertex TrackVertex::getTunnelTopRightSideTangentVertexProperty() const
    {
        return {pos + RoadWidthScale * roadWidth * right / 2.0f +
                    up * RoadTunnelHeight,
                Vector2(uv.X, 1.0f), -up, -right};
    }

    TrackVertex::TrackVertex(Vector3 setPos, Vector3 setRight, Vector3 setUp,
                             Vector3 setDir, Vector2 setUv, float setRoadWidth)
        : pos(setPos), right(setRight), up(setUp), dir(setDir), uv(setUv),
          roadWidth(setRoadWidth)
    {
    }

    TrackVertex::TrackVertex(Vector3 setPos)
        : pos(setPos), right(Vector3::Right), up(Vector3::Up),
          dir(Vector3::Forward), uv(Vector2::Zero)
    {
    }
}
