#pragma once

// QuestLine.hpp -- C++ port of RolePlayingGameData/Quests/QuestLine.cs.

#include <optional>
#include <memory>
#include <string>
#include <vector>

#include "../ContentObject.hpp"
#include "Quest.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "System/Random.hpp"
namespace RolePlayingGameData {

// A line of quests, presented to the player in order -- only one quest is
// presented at a time and must be completed before the line can continue.
class QuestLine : public ContentObject {
public:
    std::string Name;
    std::vector<std::string> QuestContentNames;
    std::vector<std::shared_ptr<Quest>> Quests;

    std::shared_ptr<QuestLine> Clone() const {
        auto line = std::make_shared<QuestLine>();
        line->SetAssetName(AssetName());
        line->Name = Name;
        line->QuestContentNames = QuestContentNames;
        for (auto& q : Quests) line->Quests.push_back(q->Clone());
        return line;
    }
};

// Reads a QuestLine object from the content pipeline.
class QuestLineReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<QuestLine>> {
public:
    QuestLineReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<QuestLine>>(
              "RolePlayingGameData.QuestLine") {}

protected:
    std::shared_ptr<QuestLine> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<QuestLine>> existingInstance) override {
        std::shared_ptr<QuestLine> questLine =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (questLine == nullptr) {
            questLine = std::make_shared<QuestLine>();
        }

        questLine->SetAssetName(input.getAssetNameProperty());
        questLine->Name = input.ReadString();
        const auto questContentNames = input.ReadObject<std::vector<std::string>>();
        questLine->QuestContentNames.insert(
            questLine->QuestContentNames.end(),
            questContentNames.begin(), questContentNames.end());
        for (const std::string& contentName : questLine->QuestContentNames) {
            questLine->Quests.push_back(
                input.getContentManagerProperty()->Load<std::shared_ptr<Quest>>(
                    "Quests/" + contentName));
        }

        return questLine;
    }
};

} // namespace RolePlayingGameData
