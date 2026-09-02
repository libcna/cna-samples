// SPDX-License-Identifier: MS-PL

#include "Tracks/TrackColumnsGeometry.hpp"

#include <array>

#include "Landscapes/Landscape.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Tracks/TrackLine.hpp"

namespace RacingGame::Tracks
{
    using Graphics::TangentVertex;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;

    namespace
    {
        constexpr float ColumnsDistance = 33.0f;
        constexpr float ColumnGroundHeight = 1.0f;
        constexpr float MinimumColumnHeight = 2.5f;
        constexpr float TopColumnSubHeight = 0.55f;

        const std::array<TangentVertex, 7> BaseVertices = {
            TangentVertex(Vector3(1, 0, 0), Vector2(0.0f / 6.0f, 0),
                          Vector3(1, 0, 0), Vector3(0, 0, -1)),
            TangentVertex(Vector3(0.5f, 0.866025f, 0), Vector2(1.0f / 6.0f, 0),
                          Vector3(0.5f, 0.866025f, 0), Vector3(0, 0, -1)),
            TangentVertex(Vector3(-0.5f, 0.866025f, 0), Vector2(2.0f / 6.0f, 0),
                          Vector3(-0.5f, 0.866025f, 0), Vector3(0, 0, -1)),
            TangentVertex(Vector3(-1, 0, 0), Vector2(3.0f / 6.0f, 0),
                          Vector3(-1, 0, 0), Vector3(0, 0, -1)),
            TangentVertex(Vector3(-0.5f, -0.866025f, 0), Vector2(4.0f / 6.0f, 0),
                          Vector3(-0.5f, -0.866025f, 0), Vector3(0, 0, -1)),
            TangentVertex(Vector3(0.5f, -0.866025f, 0), Vector2(5.0f / 6.0f, 0),
                          Vector3(0.5f, -0.866025f, 0), Vector3(0, 0, -1)),
            TangentVertex(Vector3(1, 0, 0), Vector2(6.0f / 6.0f, 0),
                          Vector3(1, 0, 0), Vector3(0, 0, -1)),
        };
    }

    TrackColumnsGeometry::TrackColumnsGeometry(
        const TrackLine& trackLine, const Landscapes::Landscape& landscape)
    {
        const auto& points = trackLine.getPointsProperty();
        std::vector<Matrix> topSpaces;
        std::vector<Matrix> bottomSpaces;
        float remainingDistance = ColumnsDistance;
        for (int num = 0; num < points.getCountProperty(); ++num)
        {
            const float distance = Vector3::Distance(
                points.getItem((num + 1) % points.getCountProperty()).pos,
                points.getItem(num).pos);
            if (remainingDistance - distance <= 0.0f)
            {
                const Vector3 p1 = points.getItem(
                    num - 1 < 0 ? points.getCountProperty() - 1 : num - 1).pos;
                const Vector3 p2 = points.getItem(num).pos;
                const Vector3 p3 = points.getItem(
                    (num + 1) % points.getCountProperty()).pos;
                const Vector3 p4 = points.getItem(
                    (num + 2) % points.getCountProperty()).pos;
                const Vector3 point = Vector3::CatmullRom(
                    p1, p2, p3, p4, remainingDistance / distance);
                const float draft = Vector3::Dot(points.getItem(num).up,
                                                  Vector3::UnitZ);
                const float height = point.Z - landscape.GetMapHeight(point.X, point.Y);
                if (draft > 0.3f && height > MinimumColumnHeight)
                {
                    columnPositions.push_back(point);
                    const Vector3 right = points.getItem(num).right;
                    const Vector3 direction = points.getItem(num).dir;
                    const Vector3 up = points.getItem(num).up;
                    Matrix space = Matrix::getIdentityProperty();
                    space.M11 = right.X; space.M12 = right.Y; space.M13 = right.Z;
                    space.M21 = direction.X; space.M22 = direction.Y;
                    space.M23 = direction.Z;
                    space.M31 = up.X; space.M32 = up.Y; space.M33 = up.Z;
                    topSpaces.push_back(space);

                    space = Matrix::getIdentityProperty();
                    const Vector3 bottomRight = Vector3::Cross(direction, Vector3::UnitZ);
                    space.M11 = bottomRight.X; space.M12 = bottomRight.Y;
                    space.M13 = bottomRight.Z;
                    space.M21 = direction.X; space.M22 = direction.Y;
                    space.M23 = direction.Z;
                    bottomSpaces.push_back(space);
                }
                remainingDistance += ColumnsDistance;
            }
            remainingDistance -= distance;
        }

        vertices.resize(columnPositions.size() * BaseVertices.size() * 2);
        for (std::size_t num = 0; num < columnPositions.size(); ++num)
        {
            const Vector3 pos = columnPositions[num];
            const Vector3 bottomPos(
                pos.X, pos.Y,
                landscape.GetMapHeight(pos.X, pos.Y) + ColumnGroundHeight);
            const Vector3 topPos(pos.X, pos.Y, pos.Z - TopColumnSubHeight);
            const float topTexV = Vector3::Distance(topPos, bottomPos) /
                                  (MathHelper::Pi * 2.0f);
            for (int topBottom = 0; topBottom < 2; ++topBottom)
            {
                for (std::size_t index = 0; index < BaseVertices.size(); ++index)
                {
                    const std::size_t output = num * BaseVertices.size() * 2 +
                                               topBottom * BaseVertices.size() + index;
                    const Matrix& transform = topBottom == 0
                                                  ? bottomSpaces[num]
                                                  : topSpaces[num];
                    vertices[output] = TangentVertex(
                        (topBottom == 0 ? bottomPos : topPos) +
                            Vector3::Transform(BaseVertices[index].pos, transform),
                        Vector2(BaseVertices[index].getUProperty(),
                                topBottom == 0 ? 0.0f : topTexV),
                        Vector3::Transform(BaseVertices[index].normal, transform),
                        Vector3::Transform(-BaseVertices[index].tangent, transform));
                }
            }
            segmentPositions.emplace_back(
                bottomPos.X, bottomPos.Y, bottomPos.Z - ColumnGroundHeight);
        }

        constexpr std::size_t quadPolysPerColumn = BaseVertices.size() - 1;
        indices.resize(6 * quadPolysPerColumn * columnPositions.size());
        int vertexIndex = 0;
        for (std::size_t num = 0; num < columnPositions.size(); ++num)
        {
            for (std::size_t index = 0; index < quadPolysPerColumn; ++index)
            {
                const std::size_t output = 6 * (num * quadPolysPerColumn + index);
                indices[output + 0] = vertexIndex + static_cast<int>(index);
                indices[output + 1] = vertexIndex + 1 +
                    static_cast<int>(BaseVertices.size() + index);
                indices[output + 2] = vertexIndex + 1 + static_cast<int>(index);
                indices[output + 3] = indices[output + 1];
                indices[output + 4] = indices[output + 0];
                indices[output + 5] = vertexIndex +
                    static_cast<int>(BaseVertices.size() + index);
            }
            vertexIndex += static_cast<int>(BaseVertices.size() * 2);
        }
    }

    const std::vector<Vector3>&
    TrackColumnsGeometry::getColumnPositionsProperty() const
    {
        return columnPositions;
    }

    const std::vector<TangentVertex>&
    TrackColumnsGeometry::getVerticesProperty() const
    {
        return vertices;
    }

    const std::vector<std::int32_t>&
    TrackColumnsGeometry::getIndicesProperty() const
    {
        return indices;
    }

    const std::vector<Vector3>&
    TrackColumnsGeometry::getSegmentPositionsProperty() const
    {
        return segmentPositions;
    }
}
