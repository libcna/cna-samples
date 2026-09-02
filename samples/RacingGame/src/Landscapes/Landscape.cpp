// SPDX-License-Identifier: MS-PL

#include "Landscapes/Landscape.hpp"

#include <stdexcept>

#include "Microsoft/Xna/Framework/TitleContainer.hpp"

namespace RacingGame::Landscapes
{
    using Graphics::TangentVertex;
    using Microsoft::Xna::Framework::TitleContainer;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;

    Landscape::Landscape(const String& heightPath)
        : vertices(static_cast<std::size_t>(GridWidth * GridHeight)),
          mapHeights(static_cast<std::size_t>(GridWidth * GridHeight))
    {
        auto file = TitleContainer::OpenStream(heightPath);
        std::vector<bytecs> heights(static_cast<std::size_t>(GridWidth * GridHeight));
        const int read = file->Read(heights.data(), 0, GridWidth * GridHeight);
        file->Close();
        if (read != GridWidth * GridHeight)
        {
            throw std::runtime_error(
                "LandscapeHeights.data does not contain exactly 257*257 bytes");
        }

        for (int x = 0; x < GridWidth; ++x)
        {
            for (int y = 0; y < GridHeight; ++y)
            {
                const int index = x + y * GridWidth;
                const Vector3 position = CalcLandscapePos(x, y, heights);
                mapHeights[HeightIndex(x, y)] = position.Z;
                TangentVertex& vertex = vertices[static_cast<std::size_t>(index)];
                vertex.pos = position;

                const Vector3 edge1 = position - CalcLandscapePos(x, y + 1, heights);
                const Vector3 edge2 = position - CalcLandscapePos(x + 1, y, heights);
                const Vector3 edge3 = position - CalcLandscapePos(x - 1, y + 1, heights);
                const Vector3 edge4 = position - CalcLandscapePos(x + 1, y + 1, heights);
                const Vector3 edge5 = position - CalcLandscapePos(x - 1, y - 1, heights);

                vertex.normal = Vector3::Normalize(
                    Vector3::Cross(edge2, edge1) +
                    Vector3::Cross(edge4, edge3) +
                    Vector3::Cross(edge3, edge5));
                vertex.tangent = Vector3::Normalize(edge1);
                vertex.uv = Vector2(y / static_cast<float>(GridHeight - 1),
                                    x / static_cast<float>(GridWidth - 1));
            }
        }

        std::vector<Vector3> normals(static_cast<std::size_t>(GridWidth * GridHeight));
        for (int x = 0; x < GridWidth; ++x)
        {
            for (int y = 0; y < GridHeight; ++y)
            {
                normals[HeightIndex(x, y)] =
                    vertices[static_cast<std::size_t>(x + y * GridWidth)].normal;
            }
        }

        for (int x = 1; x < GridWidth - 1; ++x)
        {
            for (int y = 1; y < GridHeight - 1; ++y)
            {
                const int index = x + y * GridWidth;
                TangentVertex& vertex = vertices[static_cast<std::size_t>(index)];
                Vector3 normal = vertex.normal * 4.0f;
                for (int xAdd = -1; xAdd <= 1; ++xAdd)
                {
                    for (int yAdd = -1; yAdd <= 1; ++yAdd)
                    {
                        normal += normals[HeightIndex(x + xAdd, y + yAdd)];
                    }
                }
                vertex.normal = Vector3::Normalize(normal);
                const Vector3 helper = Vector3::Cross(vertex.normal, vertex.tangent);
                vertex.tangent = Vector3::Cross(helper, vertex.normal);
            }
        }

        indices.resize(static_cast<std::size_t>((GridWidth - 1) * (GridHeight - 1) * 6));
        int current = 0;
        for (int x = 0; x < GridWidth - 1; ++x)
        {
            for (int y = 0; y < GridHeight - 1; ++y)
            {
                indices[static_cast<std::size_t>(current + 0)] =
                    static_cast<std::uint32_t>(x * GridHeight + y);
                indices[static_cast<std::size_t>(current + 2)] =
                    static_cast<std::uint32_t>((x + 1) * GridHeight + (y + 1));
                indices[static_cast<std::size_t>(current + 1)] =
                    static_cast<std::uint32_t>((x + 1) * GridHeight + y);
                indices[static_cast<std::size_t>(current + 3)] =
                    static_cast<std::uint32_t>((x + 1) * GridHeight + (y + 1));
                indices[static_cast<std::size_t>(current + 5)] =
                    static_cast<std::uint32_t>(x * GridHeight + y);
                indices[static_cast<std::size_t>(current + 4)] =
                    static_cast<std::uint32_t>(x * GridHeight + (y + 1));
                current += 6;
            }
        }
    }

    void Landscape::KillAllLoadedObjects()
    {
        landscapeObjects.clear();
    }

    void Landscape::AddObjectToRender(
        const String& modelName, Microsoft::Xna::Framework::Matrix renderMatrix,
        bool isNearTrackForShadowGeneration)
    {
        landscapeObjects.push_back(
            {modelName, renderMatrix, isNearTrackForShadowGeneration});
    }

    float Landscape::GetMapHeight(int x, int y) const
    {
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x >= GridWidth) x = GridWidth - 1;
        if (y >= GridHeight) y = GridHeight - 1;
        return mapHeights[HeightIndex(x, y)];
    }

    int Landscape::ModulateValueInRange(float value, int maximum)
    {
        return value < 0.0f
                   ? (maximum - 1) - (static_cast<int>(-value) % maximum)
                   : static_cast<int>(value) % maximum;
    }

    float Landscape::GetMapHeight(float x, float y) const
    {
        x /= MapWidthFactor;
        y /= MapHeightFactor;
        const int ix = ModulateValueInRange(x, GridWidth - 1);
        const int iy = ModulateValueInRange(y, GridHeight - 1);
        const float fX = x - static_cast<float>(static_cast<int>(x));
        const float fY = y - static_cast<float>(static_cast<int>(y));
        const int ix2 = (ix + 1) % (GridWidth - 1);
        const int iy2 = (iy + 1) % (GridHeight - 1);

        if (fX + fY < 1.0f)
        {
            return GetMapHeight(ix, iy) +
                   fX * (GetMapHeight(ix2, iy) - GetMapHeight(ix, iy)) +
                   fY * (GetMapHeight(ix, iy2) - GetMapHeight(ix, iy));
        }
        return GetMapHeight(ix2, iy2) +
               (1.0f - fY) * (GetMapHeight(ix2, iy) - GetMapHeight(ix2, iy2)) +
               (1.0f - fX) * (GetMapHeight(ix, iy2) - GetMapHeight(ix2, iy2));
    }

    const std::vector<TangentVertex>& Landscape::getVerticesProperty() const
    {
        return vertices;
    }

    const std::vector<std::uint32_t>& Landscape::getIndicesProperty() const
    {
        return indices;
    }

    const std::vector<Landscape::LandscapeObjectRecord>&
    Landscape::getLandscapeObjectRecordsProperty() const
    {
        return landscapeObjects;
    }

    Vector3 Landscape::CalcLandscapePos(int x, int y,
                                        const std::vector<bytecs>& heights)
    {
        const int mapX = x < 0 ? 0 : x >= GridWidth ? GridWidth - 1 : x;
        const int mapY = y < 0 ? 0 : y >= GridHeight ? GridHeight - 1 : y;
        const float heightPercent =
            heights[static_cast<std::size_t>(mapX + mapY * GridWidth)] / 255.0f;
        return Vector3(x * MapWidthFactor, y * MapHeightFactor,
                       heightPercent * MapZScale);
    }

    std::size_t Landscape::HeightIndex(int x, int y)
    {
        return static_cast<std::size_t>(x * GridHeight + y);
    }
}
