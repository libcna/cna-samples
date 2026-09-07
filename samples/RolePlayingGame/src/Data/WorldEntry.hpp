#pragma once

// WorldEntry.hpp -- C++ port of RolePlayingGameData/WorldEntry.cs.

#include "XmlSaveHooks.hpp"
#include "XmlEntryNames.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"
#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "MapEntry.hpp"

namespace RolePlayingGameData {

// A description of a piece of content, including the name of the map it's on.
template <typename T>
class WorldEntry : public MapEntry<T> {
public:
    std::string MapContentName;

    SHARP_XML_SERIALIZABLE(WorldEntry, XmlEntryNames<T>::WorldEntry,
                           SHARP_XML_M(WorldEntry, ContentName), SHARP_XML_M(WorldEntry, Count),
                           SHARP_XML_M(WorldEntry, MapPosition),
                           ::System::Xml::Serialization::detail::MakeMember(
                               "Direction", &WorldEntry<T>::EntryDirection),
                           SHARP_XML_M(WorldEntry, MapSprite),
                           SHARP_XML_M(WorldEntry, MapContentName))
};

// Reads a WorldEntry object from the content pipeline.
template <typename T>
class WorldEntryReader
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<
          std::shared_ptr<WorldEntry<T>>> {
public:
    explicit WorldEntryReader(const std::string& typeName)
        : Microsoft::Xna::Framework::Content::ContentTypeReader<
              std::shared_ptr<WorldEntry<T>>>(typeName) {}

protected:
    std::shared_ptr<WorldEntry<T>> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<WorldEntry<T>>> existingInstance) override {
        std::shared_ptr<WorldEntry<T>> desc =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (desc == nullptr) {
            desc = std::make_shared<WorldEntry<T>>();
        }

        MapEntryReader<T> mapEntryReader("RolePlayingGameData.MapEntry");
        input.ReadRawObject<std::shared_ptr<MapEntry<T>>>(
            mapEntryReader, std::static_pointer_cast<MapEntry<T>>(desc));
        desc->MapContentName = input.ReadString();

        return desc;
    }
};

} // namespace RolePlayingGameData
