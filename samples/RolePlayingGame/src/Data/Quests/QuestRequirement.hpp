#pragma once

// QuestRequirement.hpp -- C++ port of RolePlayingGameData/Quests/QuestRequirement.cs.

#include <memory>
#include <optional>
#include <vector>
#include <string>
#include "../ContentEntry.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

// A requirement for a particular number of a piece of content.
// Used to track gear acquired and monsters killed.
template <typename T>
class QuestRequirement : public ContentEntry<T> {
public:
    int CompletedCount = 0;
};

// Reads a QuestRequirement object from the content pipeline.
//
// The original branches on typeof(T) to pick the content directory. C++ has no runtime type
// switch here, so the directory is a constructor argument the registration supplies -- the same
// two values the original's two branches produce.
template <typename T>
class QuestRequirementReader
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<
          std::shared_ptr<QuestRequirement<T>>> {
public:
    QuestRequirementReader(const std::string& typeName, std::string contentDirectory)
        : Microsoft::Xna::Framework::Content::ContentTypeReader<
              std::shared_ptr<QuestRequirement<T>>>(typeName),
          contentDirectory_(std::move(contentDirectory)) {}

protected:
    std::shared_ptr<QuestRequirement<T>> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<QuestRequirement<T>>> existingInstance) override {
        std::shared_ptr<QuestRequirement<T>> requirement =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (requirement == nullptr) {
            requirement = std::make_shared<QuestRequirement<T>>();
        }

        ContentEntryReader<T> contentEntryReader("RolePlayingGameData.ContentEntry");
        input.ReadRawObject<std::shared_ptr<ContentEntry<T>>>(
            contentEntryReader, std::static_pointer_cast<ContentEntry<T>>(requirement));
        requirement->Content = input.getContentManagerProperty()->Load<std::shared_ptr<T>>(
            contentDirectory_ + "/" + requirement->ContentName);

        return requirement;
    }

private:
    std::string contentDirectory_;
};

} // namespace RolePlayingGameData
