// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// HeightMapInfo.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "HeightMapInfo.hpp"

#include <cmath>
#include <utility>

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "System/ArgumentNullException.hpp"

namespace TanksOnAHeightmap
{
    using Microsoft::Xna::Framework::MathHelper;

    HeightMapInfo::HeightMapInfo(
        std::vector<std::vector<float>> heightsIn,
        std::vector<std::vector<Vector3>> normalsIn,
        const float terrainScaleIn)
    {
        if (heightsIn.empty())
        {
            throw System::ArgumentNullException("heights");
        }
        if (normalsIn.empty())
        {
            throw System::ArgumentNullException("normals");
        }

        terrainScale = terrainScaleIn;
        heights = std::move(heightsIn);
        normals = std::move(normalsIn);

        const auto width = static_cast<float>(heights.size());
        const auto depth = static_cast<float>(heights[0].size());

        heightmapWidth = (width - 1) * terrainScale;
        heightmapHeight = (depth - 1) * terrainScale;

        heightmapPosition.X = -(width - 1) / 2.0f * terrainScale;
        heightmapPosition.Z = -(depth - 1) / 2.0f * terrainScale;
    }

    const std::string& HeightMapInfo::GetTypeName() const
    {
        static const std::string name = "TanksOnAHeightmap.HeightMapInfo";
        return name;
    }

    bool HeightMapInfo::IsOnHeightmap(const Vector3& position) const
    {
        const Vector3 positionOnHeightmap = position - heightmapPosition;

        return positionOnHeightmap.X > 0
            && positionOnHeightmap.X < heightmapWidth
            && positionOnHeightmap.Z > 0
            && positionOnHeightmap.Z < heightmapHeight;
    }

    void HeightMapInfo::GetHeightAndNormal(
        const Vector3& position, float& height, Vector3& normal) const
    {
        const Vector3 positionOnHeightmap = position - heightmapPosition;

        const int left = static_cast<int>(positionOnHeightmap.X)
            / static_cast<int>(terrainScale);
        const int top = static_cast<int>(positionOnHeightmap.Z)
            / static_cast<int>(terrainScale);

        const float xNormalized =
            std::fmod(positionOnHeightmap.X, terrainScale) / terrainScale;
        const float zNormalized =
            std::fmod(positionOnHeightmap.Z, terrainScale) / terrainScale;

        const std::size_t x = static_cast<std::size_t>(left);
        const std::size_t z = static_cast<std::size_t>(top);

        const float topHeight = MathHelper::Lerp(
            heights[x][z], heights[x + 1][z], xNormalized);
        const float bottomHeight = MathHelper::Lerp(
            heights[x][z + 1], heights[x + 1][z + 1], xNormalized);
        height = MathHelper::Lerp(topHeight, bottomHeight, zNormalized);

        const Vector3 topNormal = Vector3::Lerp(
            normals[x][z], normals[x + 1][z], xNormalized);
        const Vector3 bottomNormal = Vector3::Lerp(
            normals[x][z + 1], normals[x + 1][z + 1], xNormalized);
        normal = Vector3::Lerp(topNormal, bottomNormal, zNormalized);
        normal.Normalize();
    }

    HeightMapInfoReader::HeightMapInfoReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<
              std::shared_ptr<System::Object>>("TanksOnAHeightmap.HeightMapInfo")
    {
    }

    std::shared_ptr<System::Object> HeightMapInfoReader::Read(
        ContentReader& input,
        std::optional<std::shared_ptr<System::Object>> /*existingInstance*/)
    {
        const float terrainScale = input.ReadSingle();
        const int width = input.ReadInt32();
        const int height = input.ReadInt32();

        std::vector<std::vector<float>> heights(
            static_cast<std::size_t>(width),
            std::vector<float>(static_cast<std::size_t>(height)));
        std::vector<std::vector<Vector3>> normals(
            static_cast<std::size_t>(width),
            std::vector<Vector3>(static_cast<std::size_t>(height)));

        for (int x = 0; x < width; x++)
        {
            for (int z = 0; z < height; z++)
            {
                heights[static_cast<std::size_t>(x)][static_cast<std::size_t>(z)] =
                    input.ReadSingle();
            }
        }
        for (int x = 0; x < width; x++)
        {
            for (int z = 0; z < height; z++)
            {
                normals[static_cast<std::size_t>(x)][static_cast<std::size_t>(z)] =
                    input.ReadVector3();
            }
        }

        return std::make_shared<HeightMapInfo>(
            std::move(heights), std::move(normals), terrainScale);
    }
}
