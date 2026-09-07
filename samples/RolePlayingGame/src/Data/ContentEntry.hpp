#pragma once

// ContentEntry.hpp -- C++ port of RolePlayingGameData/ContentEntry.cs.

#include <optional>
#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

// A description of a piece of content and quantity for various purposes.
// T's content is not automatically loaded, as the content path may be incomplete
// (matches the original's remark) -- callers populate Content after construction.
template <typename T>
class ContentEntry {
public:
    std::string ContentName;
    std::shared_ptr<T> Content;
    int Count = 1;

    virtual ~ContentEntry() = default;
};

// Reads a ContentEntry object from the content pipeline.
//
// The original is nested in the generic class, so each instantiation registers under the name the
// .xnb records for it -- see RegisterContentTypeReaders().
template <typename T>
class ContentEntryReader
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<
          std::shared_ptr<ContentEntry<T>>> {
public:
    explicit ContentEntryReader(const std::string& typeName)
        : Microsoft::Xna::Framework::Content::ContentTypeReader<
              std::shared_ptr<ContentEntry<T>>>(typeName) {}

protected:
    std::shared_ptr<ContentEntry<T>> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<ContentEntry<T>>> existingInstance) override {
        std::shared_ptr<ContentEntry<T>> member =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (member == nullptr) {
            member = std::make_shared<ContentEntry<T>>();
        }

        member->ContentName = input.ReadString();
        member->Count = static_cast<int>(input.ReadInt32());

        return member;
    }
};

} // namespace RolePlayingGameData
