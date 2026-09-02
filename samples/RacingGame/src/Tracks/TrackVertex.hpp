// SPDX-License-Identifier: MS-PL

#pragma once

#include "Graphics/TangentVertex.hpp"

namespace RacingGame::Tracks
{
    class TrackVertex
    {
    public:
        Microsoft::Xna::Framework::Vector3 pos;
        Microsoft::Xna::Framework::Vector3 right;
        Microsoft::Xna::Framework::Vector3 up;
        Microsoft::Xna::Framework::Vector3 dir;
        Microsoft::Xna::Framework::Vector2 uv;

        static constexpr float MinRoadWidth = 0.25f;
        static constexpr float DefaultRoadWidth = 1.0f;
        static constexpr float MaxRoadWidth = 2.0f;
        static constexpr float RoadWidthScale = 13.25f;
        float roadWidth = DefaultRoadWidth;

        [[nodiscard]] TrackVertex getLeftTrackVertexProperty() const;
        [[nodiscard]] TrackVertex getRightTrackVertexProperty() const;
        [[nodiscard]] Graphics::TangentVertex getLeftTangentVertexProperty() const;
        [[nodiscard]] Graphics::TangentVertex getRightTangentVertexProperty() const;
        [[nodiscard]] Graphics::TangentVertex getMiddleTangentVertexProperty() const;
        [[nodiscard]] Graphics::TangentVertex getMiddleLeftTangentVertexProperty() const;
        [[nodiscard]] Graphics::TangentVertex getMiddleRightTangentVertexProperty() const;
        [[nodiscard]] Graphics::TangentVertex getBottomLeftSideTangentVertexProperty() const;
        [[nodiscard]] Graphics::TangentVertex getBottomRightSideTangentVertexProperty() const;
        [[nodiscard]] Graphics::TangentVertex getTunnelTopLeftSideTangentVertexProperty() const;
        [[nodiscard]] Graphics::TangentVertex getTunnelTopRightSideTangentVertexProperty() const;

        TrackVertex(Microsoft::Xna::Framework::Vector3 setPos,
                    Microsoft::Xna::Framework::Vector3 setRight,
                    Microsoft::Xna::Framework::Vector3 setUp,
                    Microsoft::Xna::Framework::Vector3 setDir,
                    Microsoft::Xna::Framework::Vector2 setUv,
                    float setRoadWidth);
        explicit TrackVertex(Microsoft::Xna::Framework::Vector3 setPos = {});

    private:
        static constexpr float RoadThickness = 1.0f;
        static constexpr float RoadTunnelHeight = 7.125f;
    };
}
