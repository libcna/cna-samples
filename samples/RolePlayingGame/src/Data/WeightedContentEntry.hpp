#pragma once

// WeightedContentEntry.hpp -- C++ port of RolePlayingGameData/WeightedContentEntry.cs.

#include <memory>
#include <optional>
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "ContentEntry.hpp"

namespace RolePlayingGameData {

// A description of a piece of content, quantity and weight for various purposes.
template <typename T>
class WeightedContentEntry : public ContentEntry<T> {
public:
    int Weight = 0;
};

// Reads a WeightedContentEntry object from the content pipeline.
template <typename T>
class WeightedContentEntryReader
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<
          std::shared_ptr<WeightedContentEntry<T>>> {
public:
    explicit WeightedContentEntryReader(const std::string& typeName)
        : Microsoft::Xna::Framework::Content::ContentTypeReader<
              std::shared_ptr<WeightedContentEntry<T>>>(typeName) {}

protected:
    std::shared_ptr<WeightedContentEntry<T>> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<WeightedContentEntry<T>>> existingInstance) override {
        std::shared_ptr<WeightedContentEntry<T>> entry =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (entry == nullptr) {
            entry = std::make_shared<WeightedContentEntry<T>>();
        }

        ContentEntryReader<T> contentEntryReader("RolePlayingGameData.ContentEntry");
        input.ReadRawObject<std::shared_ptr<ContentEntry<T>>>(
            contentEntryReader, std::static_pointer_cast<ContentEntry<T>>(entry));
        entry->Weight = static_cast<int>(input.ReadInt32());

        return entry;
    }
};

} // namespace RolePlayingGameData
