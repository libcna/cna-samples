// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// HeightMapInfo.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "System/Object.hpp"

namespace HeightmapCollision
{
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Content::ContentReader;

    /**
     * @brief HeightMapInfo is a collection of data about the heightmap.
     *
     * It includes information about how high the terrain is, and how far apart each vertex is.
     * It also has several functions to get information about the heightmap, including its height
     * at a point, and whether a point is on the heightmap.
     *
     * It is created by the TerrainProcessor during the content pipeline build, and attached to the
     * terrain model's `Tag`. `Model::Tag` is a `System::Object*`, so unlike the C# class this one
     * derives from `System::Object`; that is the C++ counterpart of C#'s `object` reference, and
     * `Model.Tag as HeightMapInfo` becomes a `dynamic_cast`.
     */
    class HeightMapInfo : public System::Object
    {
    private:
        // the height of the terrain, indexed [x][z], and how far apart the vertices are.
        float terrainScale;
        std::vector<std::vector<float>> heights;

        // the position of the heightmap's -x, -z corner, in worldspace.
        Vector3 heightmapPosition;

        // the total width and height of the heightmap, in worldspace.
        float heightmapWidth;
        float heightmapHeight;

    public:
        /**
         * @brief Constructs the heightmap information from the pipeline's own data.
         *
         * @param heights The height at each vertex, indexed [x][z].
         * @param terrainScale The distance between two neighbouring vertices.
         * @throws System::ArgumentNullException if @p heights is empty.
         */
        HeightMapInfo(std::vector<std::vector<float>> heights, float terrainScale);

        /**
         * @brief Returns the fully qualified logical type name of this object.
         * @return "HeightmapCollision.HeightMapInfo".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Checks whether a point is on the heightmap at all.
         *
         * @param position The world-space position to test.
         * @return True when the position is above the heightmap.
         */
        [[nodiscard]] bool IsOnHeightmap(const Vector3& position) const;

        /**
         * @brief Gets the height of the terrain under a point, interpolated between the four
         *        surrounding vertices.
         *
         * @param position The world-space position to sample under.
         * @return The terrain height at that position.
         */
        [[nodiscard]] float GetHeight(const Vector3& position) const;
    };

    /**
     * @brief Reads the `HeightMapInfo` the sample's own ContentTypeWriter produced.
     *
     * XNA finds this reader by reflecting over the game assembly for the name the `.xnb` records
     * (`HeightmapCollision.HeightMapInfoReader`). C++ has no reflection, so the game registers it
     * by that same name; see `HeightmapCollisionGame`'s constructor.
     *
     * It returns `std::shared_ptr<System::Object>` rather than `HeightMapInfo` because that is the
     * shape `Model::Tag` carries — the object outlives the read and the model owns it.
     */
    class HeightMapInfoReader
        : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<System::Object>>
    {
    public:
        /** @brief Constructs the reader under the runtime type name the `.xnb` records. */
        HeightMapInfoReader();

    protected:
        /**
         * @brief Reads the terrain scale, the grid size and every height, in that order.
         *
         * @param input The reader positioned at this object's data.
         * @param existingInstance Unused; this type is always constructed fresh.
         * @return The heightmap information.
         */
        std::shared_ptr<System::Object> Read(
            ContentReader& input,
            std::optional<std::shared_ptr<System::Object>> existingInstance) override;
    };
}
