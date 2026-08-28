// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// HeightMapInfo.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "HeightMapInfo.hpp"

#include <cmath>

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "System/ArgumentNullException.hpp"

namespace HeightmapCollision
{
    using Microsoft::Xna::Framework::MathHelper;

    HeightMapInfo::HeightMapInfo(std::vector<std::vector<float>> heightsIn, float terrainScaleIn)
    {
        if (heightsIn.empty())
        {
            throw System::ArgumentNullException("heights");
        }

        terrainScale = terrainScaleIn;
        heights = std::move(heightsIn);

        const auto width = static_cast<float>(heights.size());
        const auto depth = static_cast<float>(heights[0].size());

        heightmapWidth = (width - 1) * terrainScale;
        heightmapHeight = (depth - 1) * terrainScale;

        // the position of the heightmap's -x, -z corner, in worldspace.
        heightmapPosition.X = -(width - 1) / 2 * terrainScale;
        heightmapPosition.Z = -(depth - 1) / 2 * terrainScale;
    }

    const std::string& HeightMapInfo::GetTypeName() const
    {
        static const std::string name = "HeightmapCollision.HeightMapInfo";
        return name;
    }

    bool HeightMapInfo::IsOnHeightmap(const Vector3& position) const
    {
        // first we'll figure out where on the heightmap "position" is...
        const Vector3 positionOnHeightmap = position - heightmapPosition;

        // ... and then check to see if that value goes outside the bounds of the heightmap.
        return (positionOnHeightmap.X > 0 && positionOnHeightmap.X < heightmapWidth
                && positionOnHeightmap.Z > 0 && positionOnHeightmap.Z < heightmapHeight);
    }

    float HeightMapInfo::GetHeight(const Vector3& position) const
    {
        // the first thing we need to do is figure out where on the heightmap
        // "position" is. This'll make the math much simpler later.
        const Vector3 positionOnHeightmap = position - heightmapPosition;

        // we'll use integer division to figure out where in the "heights" array
        // positionOnHeightmap is. Remember that integer division always rounds
        // down, so that the result of these divisions is the indices of the "upper
        // left" of the 4 corners of that cell.
        const int left = static_cast<int>(positionOnHeightmap.X) / static_cast<int>(terrainScale);
        const int top = static_cast<int>(positionOnHeightmap.Z) / static_cast<int>(terrainScale);

        // next, we'll use modulus to find out how far away we are from the upper
        // left corner of the cell. Mod will give us a value from 0 to terrainScale,
        // which we then divide by terrainScale to normalize 0 to 1.
        const float xNormalized = std::fmod(positionOnHeightmap.X, terrainScale) / terrainScale;
        const float zNormalized = std::fmod(positionOnHeightmap.Z, terrainScale) / terrainScale;

        // Now that we've calculated the indices of the corners of our cell, and
        // where we are in that cell, we'll use bilinear interpolation to calculate
        // our height. This process is best explained with a diagram, so please see
        // the accompanying doc for more information.
        // First, we'll linearly interpolate between the heights of the top left and
        // top right corners.
        const float topHeight = MathHelper::Lerp(
            heights[static_cast<std::size_t>(left)][static_cast<std::size_t>(top)],
            heights[static_cast<std::size_t>(left + 1)][static_cast<std::size_t>(top)],
            xNormalized);

        // next, we'll linearly interpolate between the bottom left and bottom right
        // corners.
        const float bottomHeight = MathHelper::Lerp(
            heights[static_cast<std::size_t>(left)][static_cast<std::size_t>(top + 1)],
            heights[static_cast<std::size_t>(left + 1)][static_cast<std::size_t>(top + 1)],
            xNormalized);

        // finally, we'll use lerp again to find our value.
        return MathHelper::Lerp(topHeight, bottomHeight, zNormalized);
    }

    HeightMapInfoReader::HeightMapInfoReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<System::Object>>(
              "HeightmapCollision.HeightMapInfo")
    {
    }

    std::shared_ptr<System::Object> HeightMapInfoReader::Read(
        ContentReader& input, std::optional<std::shared_ptr<System::Object>> /*existingInstance*/)
    {
        const float terrainScale = input.ReadSingle();
        const int width = static_cast<int>(input.ReadInt32());
        const int height = static_cast<int>(input.ReadInt32());

        // The writer walks a C# float[width, height] with foreach, which visits the LAST index
        // fastest, so the file holds every z for x = 0, then every z for x = 1, and so on. This
        // reads them back in the same order, exactly as the original's own reader does.
        std::vector<std::vector<float>> heights(
            static_cast<std::size_t>(width), std::vector<float>(static_cast<std::size_t>(height)));
        for (int x = 0; x < width; x++)
        {
            for (int z = 0; z < height; z++)
            {
                heights[static_cast<std::size_t>(x)][static_cast<std::size_t>(z)] =
                    input.ReadSingle();
            }
        }

        return std::make_shared<HeightMapInfo>(std::move(heights), terrainScale);
    }
}
