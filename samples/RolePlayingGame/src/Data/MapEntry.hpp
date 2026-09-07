#pragma once

// MapEntry.hpp -- C++ port of RolePlayingGameData/MapEntry.cs.

#include "XmlSaveHooks.hpp"
#include "XmlEntryNames.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"
#include <memory>
#include <optional>
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "ContentEntry.hpp"
#include "Direction.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"

namespace RolePlayingGameData {

class AnimatingSprite;

// The description of where an instance of a world object is in the world.
template <typename T>
class MapEntry : public ContentEntry<T> {
public:
    Microsoft::Xna::Framework::Point MapPosition;
    Direction EntryDirection = Direction::South;

    // Only used when there might be several of the same WorldObject in the scene at once.
    std::shared_ptr<AnimatingSprite> MapSprite;

    // The base's two members come first, as C# XmlSerializer writes an inherited member before a
    // declared one; the element is named Direction, which this port spells EntryDirection.
    SHARP_XML_SERIALIZABLE(MapEntry, XmlEntryNames<T>::MapEntry,
                           SHARP_XML_M(MapEntry, ContentName), SHARP_XML_M(MapEntry, Count),
                           SHARP_XML_M(MapEntry, MapPosition),
                           ::System::Xml::Serialization::detail::MakeMember(
                               "Direction", &MapEntry<T>::EntryDirection),
                           SHARP_XML_M(MapEntry, MapSprite))
};

// Reads a MapEntry object from the content pipeline.
template <typename T>
class MapEntryReader
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<
          std::shared_ptr<MapEntry<T>>> {
public:
    explicit MapEntryReader(const std::string& typeName)
        : Microsoft::Xna::Framework::Content::ContentTypeReader<
              std::shared_ptr<MapEntry<T>>>(typeName) {}

protected:
    std::shared_ptr<MapEntry<T>> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<MapEntry<T>>> existingInstance) override {
        std::shared_ptr<MapEntry<T>> desc =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (desc == nullptr) {
            desc = std::make_shared<MapEntry<T>>();
        }

        ContentEntryReader<T> contentEntryReader("RolePlayingGameData.ContentEntry");
        input.ReadRawObject<std::shared_ptr<ContentEntry<T>>>(
            contentEntryReader, std::static_pointer_cast<ContentEntry<T>>(desc));
        desc->MapPosition = input.ReadObject<Microsoft::Xna::Framework::Point>();
        // The original member is named Direction; C++ cannot spell a member and a visible type the
        // same way in this scope, so the port keeps the original's value under EntryDirection.
        desc->EntryDirection = static_cast<Direction>(input.ReadInt32());

        return desc;
    }
};

} // namespace RolePlayingGameData
