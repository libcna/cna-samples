#pragma once

// QuestNpc.hpp -- C++ port of RolePlayingGameData/Characters/QuestNpc.cs.

#include <memory>
#include <optional>
#include <vector>
#include <string>

#include "Character.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "System/ArgumentNullException.hpp"
namespace RolePlayingGameData {

class QuestNpc : public Character {
public:
    std::string IntroductionDialogue;
};

// Reads a QuestNpc object from the content pipeline.
class QuestNpcReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<QuestNpc>> {
public:
    QuestNpcReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<QuestNpc>>(
              "RolePlayingGameData.QuestNpc") {}

protected:
    std::shared_ptr<QuestNpc> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<QuestNpc>> existingInstance) override {
        std::shared_ptr<QuestNpc> questNpc =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (questNpc == nullptr) {
            questNpc = std::make_shared<QuestNpc>();
        }

        CharacterReader characterReader;
        input.ReadRawObject<std::shared_ptr<Character>>(
            characterReader, std::static_pointer_cast<Character>(questNpc));
        questNpc->IntroductionDialogue = input.ReadString();

        return questNpc;
    }
};

} // namespace RolePlayingGameData
