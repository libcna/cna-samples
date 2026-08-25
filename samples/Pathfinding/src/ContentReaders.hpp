// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "MapData.hpp"

#include "CNA/Internal/Xnb/CollectionContentTypeReaders.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"

namespace PathfindingData::ContentReaders
{
    using Microsoft::Xna::Framework::Point;
    using Microsoft::Xna::Framework::Content::ContentReader;
    using Microsoft::Xna::Framework::Content::ContentTypeReader;
    using Microsoft::Xna::Framework::Content::ContentTypeReaderManager;

    /** @brief Canonical XNB reader name the pipeline emits for the four map assets. */
    inline constexpr const char* MapDataReaderName =
        "Microsoft.Xna.Framework.Content.ReflectiveReader`1[[PathfindingData.MapData]]";
    /** @brief Canonical XNB reader name for MapData's barrier list. */
    inline constexpr const char* PointListReaderName =
        "Microsoft.Xna.Framework.Content.ListReader`1[[Microsoft.Xna.Framework.Point]]";

    /**
     * @brief AOT equivalent of the reflective XNA reader the pipeline used for MapData.
     *
     * The upstream content project runs `Map1.xml`-`Map4.xml` through `XmlImporter` and
     * `PassThroughProcessor` with a project reference to the `PathfindingData` library, so
     * each XNB holds a `ReflectiveReader<PathfindingData.MapData>` writing the type's five
     * public fields in declaration order: two `Int32`s, two `Point`s and a `List<Point>`.
     * This reads exactly those, in that order, into the same runtime type.
     */
    class MapDataReader : public ContentTypeReader<MapData>
    {
    public:
        /** @brief Constructs the reader for the PathfindingData.MapData runtime type. */
        MapDataReader()
            : ContentTypeReader<MapData>("PathfindingData.MapData")
        {
        }

    protected:
        MapData Read(ContentReader& input, std::optional<MapData>) override
        {
            MapData value;
            // The reflective writer inlines a value-type field and writes no reader index
            // for it, so the two ints and the two Points are read raw. A Point is exactly
            // what PointReader writes: two Int32s. Only the barrier list is a reference
            // type, and only it carries a reader index, which ReadObject consumes.
            value.NumberRows = input.ReadInt32();
            value.NumberColumns = input.ReadInt32();
            value.Start = ReadRawPoint(input);
            value.End = ReadRawPoint(input);
            value.Barriers = input.ReadObject<std::vector<Point>>();
            return value;
        }

    private:
        /** @brief Reads the two Int32s PointReader writes, with no reader index. */
        [[nodiscard]] static Point ReadRawPoint(ContentReader& input)
        {
            int X = input.ReadInt32();
            int Y = input.ReadInt32();
            return Point(X, Y);
        }
    };

    /** @brief Registers the closed readers the four map assets need. Idempotent. */
    inline void Register()
    {
        ContentTypeReaderManager::AddTypeCreator(
            MapDataReaderName,
            [] { return std::make_unique<MapDataReader>(); });
        ContentTypeReaderManager::AddTypeCreator(
            PointListReaderName,
            [] {
                return std::make_unique<CNA::Internal::Xnb::ListReader<Point>>(
                    "System.Collections.Generic.List`1[[Microsoft.Xna.Framework.Point]]",
                    "Microsoft.Xna.Framework.Content.PointReader");
            });
    }
}
