#pragma once

// GameStartDescription.hpp -- C++ port of RolePlayingGameData/GameStartDescription.cs.

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

// The data needed to start a new game.
class GameStartDescription {
public:
    std::string MapContentName;
    std::vector<std::string> PlayerContentNames;
    // The first quest will be started before the world is shown.
    std::string QuestLineContentName;
};

// Reads a GameStartDescription object from the content pipeline.
class GameStartDescriptionReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<
          std::shared_ptr<GameStartDescription>> {
public:
    GameStartDescriptionReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<
              std::shared_ptr<GameStartDescription>>(
              "RolePlayingGameData.GameStartDescription") {}

protected:
    std::shared_ptr<GameStartDescription> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<GameStartDescription>> existingInstance) override {
        std::shared_ptr<GameStartDescription> desc =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (desc == nullptr) {
            desc = std::make_shared<GameStartDescription>();
        }

        desc->MapContentName = input.ReadString();
        const auto playerContentNames = input.ReadObject<std::vector<std::string>>();
        desc->PlayerContentNames.insert(
            desc->PlayerContentNames.end(),
            playerContentNames.begin(), playerContentNames.end());
        desc->QuestLineContentName = input.ReadString();

        return desc;
    }
};

} // namespace RolePlayingGameData
