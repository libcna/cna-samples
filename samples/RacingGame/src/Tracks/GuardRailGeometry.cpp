// SPDX-License-Identifier: MS-PL

#include "Tracks/GuardRailGeometry.hpp"

#include <array>

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Tracks/TrackLine.hpp"

namespace RacingGame::Tracks
{
    using Graphics::TangentVertex;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;

    namespace
    {
        constexpr float CorrectionScale = 0.0019f;
        constexpr float HolderGap = 15.0f;
        constexpr float GuardRailHeight = 1.35f * 1.5f * 0.425f;
        constexpr float InsideRoadDistance = 0.5f;
        const Vector3 HolderPileCorrectionVector(0.225f, 0.0f, 0.0f);

        const std::array<TangentVertex, 17> BaseVertices = {
            TangentVertex(Vector3(10, 0, -105), Vector2(0.0f, 1 - 0.442877f),
                          Vector3(-0.382683f, 0, -0.923880f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(20, 0, -105), Vector2(0.0f, 1 - 0.432881f),
                          Vector3(0.923880f, 0, -0.382683f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(-10, 0, -75), Vector2(0.0f, 1 - 0.402893f),
                          Vector3(0.923880f, 0, 0.382683f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(-10, 0, -45), Vector2(0.0f, 1 - 0.372905f),
                          Vector3(0.923880f, 0, -0.382683f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(20, 0, -15), Vector2(0.0f, 1 - 0.342917f),
                          Vector3(0.923880f, 0, -0.382683f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(20, 0, 15), Vector2(0.0f, 1 - 0.312929f),
                          Vector3(0.923880f, 0, 0.382683f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(-10, 0, 45), Vector2(0.0f, 1 - 0.282941f),
                          Vector3(0.923880f, 0, 0.382683f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(-10, 0, 75), Vector2(0.0f, 1 - 0.252953f),
                          Vector3(0.923880f, 0, -0.382683f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(20, 0, 105), Vector2(0.0f, 1 - 0.222965f),
                          Vector3(0.923880f, 0, 0.382683f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(10, 0, 105), Vector2(0.0f, 1 - 0.212969f),
                          Vector3(-0.923880f, 0, 0.382683f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(-20, 0, 75), Vector2(0.0f, 1 - 0.182981f),
                          Vector3(-0.923880f, 0, 0.382683f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(-20, 0, 45), Vector2(0.0f, 1 - 0.152993f),
                          Vector3(-0.923880f, 0, -0.382683f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(10, 0, 15), Vector2(0.0f, 1 - 0.123005f),
                          Vector3(-0.923880f, 0, -0.382683f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(10, 0, -15), Vector2(0.0f, 1 - 0.093017f),
                          Vector3(-0.923880f, 0, 0.382683f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(-20, 0, -45), Vector2(0.0f, 1 - 0.063029f),
                          Vector3(-0.923880f, 0, 0.382683f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(-20, 0, -75), Vector2(0.0f, 1 - 0.033041f),
                          Vector3(-0.923880f, 0, -0.382683f), Vector3(0, -1, 0)),
            TangentVertex(Vector3(10, 0, -105), Vector2(0.0f, 1 - 0.003053f),
                          Vector3(-0.382683f, 0, -0.923880f), Vector3(0, -1, 0)),
        };
    }

    GuardRailGeometry::GuardRailGeometry(const TrackLine& trackLine, Mode mode)
    {
        const auto& points = trackLine.getPointsProperty();
        railPointCount = static_cast<std::size_t>(points.getCountProperty() / 2 + 1);
        std::vector<TrackVertex> railPoints(railPointCount);
        for (std::size_t num = 0; num < railPointCount; ++num)
        {
            int pointNum = static_cast<int>(num * 2);
            if (pointNum >= points.getCountProperty() - 1)
                pointNum = points.getCountProperty() - 1;
            if (mode == Mode::Left)
            {
                railPoints[num] = points.getItem(pointNum).getLeftTrackVertexProperty();
                railPoints[num].right = -railPoints[num].right;
                railPoints[num].dir = -railPoints[num].dir;
                railPoints[num].pos -= railPoints[num].right * InsideRoadDistance;
            }
            else
            {
                railPoints[num] = points.getItem(pointNum).getRightTrackVertexProperty();
                railPoints[num].pos -= railPoints[num].right * InsideRoadDistance;
            }
        }

        vertices.resize(railPointCount * BaseVertices.size());
        float uTexValue = 0.5f;
        float lastHolderGap = 0.0f;
        for (std::size_t num = 0; num < railPointCount; ++num)
        {
            const Vector3 right = railPoints[num].right;
            const Vector3 direction = railPoints[num].dir;
            const Vector3 up = railPoints[num].up;
            Matrix pointSpace = Matrix::getIdentityProperty();
            pointSpace.M11 = right.X; pointSpace.M12 = right.Y; pointSpace.M13 = right.Z;
            pointSpace.M21 = direction.X; pointSpace.M22 = direction.Y;
            pointSpace.M23 = direction.Z;
            pointSpace.M31 = up.X; pointSpace.M32 = up.Y; pointSpace.M33 = up.Z;
            const Vector3 localPos = railPoints[num].pos + up * GuardRailHeight;
            for (std::size_t index = 0; index < BaseVertices.size(); ++index)
            {
                const Vector3 pos = Vector3::Transform(
                    BaseVertices[index].pos * CorrectionScale,
                    pointSpace * Matrix::CreateTranslation(localPos));
                const Vector3 normal = Vector3::TransformNormal(
                    (mode == Mode::Left ? -1.0f : 1.0f) *
                        BaseVertices[index].normal,
                    pointSpace);
                const Vector3 tangent = Vector3::TransformNormal(
                    -BaseVertices[index].tangent, pointSpace);
                vertices[num * BaseVertices.size() + index] = TangentVertex(
                    pos, Vector2(uTexValue, BaseVertices[index].getVProperty()),
                    normal, tangent);
            }

            const float distance = Vector3::Distance(
                railPoints[(num + 1) % railPointCount].pos, railPoints[num].pos);
            uTexValue += (1.0f / HolderGap) * distance * 2.0f;
            if (lastHolderGap - distance <= 0.0f)
            {
                const Vector3 p1 = railPoints[
                    num == 0 ? railPointCount - 1 : num - 1].pos;
                const Vector3 p2 = railPoints[num].pos;
                const Vector3 p3 = railPoints[(num + 1) % railPointCount].pos;
                const Vector3 p4 = railPoints[(num + 2) % railPointCount].pos;
                const Vector3 holderPoint = Vector3::CatmullRom(
                    p1, p2, p3, p4, lastHolderGap / distance);
                holderMatrices.push_back(
                    Matrix::CreateScale(1.125f) *
                    Matrix::CreateTranslation(HolderPileCorrectionVector) *
                    pointSpace * Matrix::CreateTranslation(holderPoint));
                lastHolderGap += HolderGap;
            }
            lastHolderGap -= distance;
        }

        constexpr std::size_t quadPolysPerStrip = BaseVertices.size() - 1;
        indices.resize(6 * quadPolysPerStrip * (railPointCount - 1));
        int vertexIndex = 0;
        for (std::size_t num = 0; num < railPointCount - 1; ++num)
        {
            for (std::size_t index = 0; index < quadPolysPerStrip; ++index)
            {
                const std::size_t output = 6 * (num * quadPolysPerStrip + index);
                indices[output + 0] = vertexIndex + static_cast<int>(index);
                indices[output + 1] = vertexIndex + 1 + static_cast<int>(index);
                indices[output + 2] = vertexIndex + 1 +
                    static_cast<int>(BaseVertices.size() + index);
                indices[output + 3] = indices[output + 2];
                indices[output + 4] = vertexIndex +
                    static_cast<int>(BaseVertices.size() + index);
                indices[output + 5] = indices[output + 0];
            }
            vertexIndex += static_cast<int>(BaseVertices.size());
        }
    }

    std::size_t GuardRailGeometry::getRailPointCountProperty() const
    {
        return railPointCount;
    }

    const std::vector<TangentVertex>& GuardRailGeometry::getVerticesProperty() const
    {
        return vertices;
    }

    const std::vector<std::int32_t>& GuardRailGeometry::getIndicesProperty() const
    {
        return indices;
    }

    const std::vector<Matrix>& GuardRailGeometry::getHolderMatricesProperty() const
    {
        return holderMatrices;
    }
}
