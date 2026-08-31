// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// HeightMapInfo.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "System/Object.hpp"

namespace TanksOnAHeightmap
{
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Content::ContentReader;

    /**
     * @brief Stores the terrain heights, normals and vertex spacing baked by TerrainProcessor.
     */
    class HeightMapInfo : public System::Object
    {
    private:
        float terrainScale;
        std::vector<std::vector<float>> heights;
        std::vector<std::vector<Vector3>> normals;
        Vector3 heightmapPosition;
        float heightmapWidth;
        float heightmapHeight;

    public:
        /**
         * @brief Initializes all heightmap data from the content-pipeline output.
         *
         * @param heights The terrain height at each `[x][z]` vertex.
         * @param normals The terrain normal at each `[x][z]` vertex.
         * @param terrainScale The distance between neighbouring vertices.
         * @throws System::ArgumentNullException if either grid is empty.
         */
        HeightMapInfo(
            std::vector<std::vector<float>> heights,
            std::vector<std::vector<Vector3>> normals,
            float terrainScale);

        /**
         * @brief Returns the fully qualified logical type name of this object.
         * @return `TanksOnAHeightmap.HeightMapInfo`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Determines whether a world-space position lies inside the terrain grid.
         *
         * @param position The position to test.
         * @return True when the position is strictly inside the heightmap bounds.
         */
        [[nodiscard]] bool IsOnHeightmap(const Vector3& position) const;

        /**
         * @brief Bilinearly interpolates the terrain height and normal below a position.
         *
         * @param position The world-space position to sample.
         * @param height Receives the interpolated terrain height.
         * @param normal Receives the normalized interpolated terrain normal.
         */
        void GetHeightAndNormal(
            const Vector3& position, float& height, Vector3& normal) const;
    };

    /** @brief Reads the custom `HeightMapInfo` payload attached to the terrain model's Tag. */
    class HeightMapInfoReader
        : public Microsoft::Xna::Framework::Content::ContentTypeReader<
              std::shared_ptr<System::Object>>
    {
    public:
        /** @brief Constructs the reader with the runtime type name recorded by the XNB. */
        HeightMapInfoReader();

    protected:
        /**
         * @brief Reads the terrain scale, dimensions, heights and normals.
         *
         * @param input The content reader positioned at the custom payload.
         * @param existingInstance Unused; this object is constructed afresh.
         * @return The heightmap data as the reference type carried by `Model.Tag`.
         */
        std::shared_ptr<System::Object> Read(
            ContentReader& input,
            std::optional<std::shared_ptr<System::Object>> existingInstance) override;
    };
}
