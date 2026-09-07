#pragma once

// Quest.hpp -- C++ port of RolePlayingGameData/Quests/Quest.cs.

#include <optional>
#include <memory>
#include <string>
#include <vector>

#include "../Characters/Monster.hpp"
#include "../ContentObject.hpp"
#include "../Gear/Gear.hpp"
#include "../Map/Chest.hpp"
#include "../Map/FixedCombat.hpp"
#include "../WorldEntry.hpp"
#include "QuestRequirement.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "System/Random.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"

namespace RolePlayingGameData {

// A quest that the party can embark on, with goals and rewards.
class Quest : public ContentObject {
public:
    enum class QuestStage { NotStarted, InProgress, RequirementsMet, Completed };

    QuestStage Stage = QuestStage::NotStarted;

    std::string Name;
    std::string Description;
    std::string ObjectiveMessage;
    std::string CompletionMessage;

    std::vector<std::shared_ptr<QuestRequirement<Gear>>> GearRequirements;
    std::vector<std::shared_ptr<QuestRequirement<Monster>>> MonsterRequirements;

    bool AreRequirementsMet() const {
        for (auto& r : GearRequirements)
            if (r->CompletedCount < r->Count) return false;
        for (auto& r : MonsterRequirements)
            if (r->CompletedCount < r->Count) return false;
        return true;
    }

    std::vector<std::shared_ptr<WorldEntry<FixedCombat>>> FixedCombatEntries;
    std::vector<std::shared_ptr<WorldEntry<Chest>>> ChestEntries;

    std::string DestinationMapContentName;
    std::string DestinationNpcContentName;
    std::string DestinationObjectiveMessage;

    int ExperienceReward = 0;
    int GoldReward = 0;
    std::vector<std::string> GearRewardContentNames;
    std::vector<std::shared_ptr<Gear>> GearRewards;

    std::shared_ptr<Quest> Clone() const {
        auto quest = std::make_shared<Quest>();
        quest->SetAssetName(AssetName());
        for (auto& chestEntry : ChestEntries) {
            auto worldEntry = std::make_shared<WorldEntry<Chest>>();
            worldEntry->Content = chestEntry->Content ? chestEntry->Content->Clone() : nullptr;
            worldEntry->ContentName = chestEntry->ContentName;
            worldEntry->Count = chestEntry->Count;
            worldEntry->EntryDirection = chestEntry->EntryDirection;
            worldEntry->MapContentName = chestEntry->MapContentName;
            worldEntry->MapPosition = chestEntry->MapPosition;
            quest->ChestEntries.push_back(worldEntry);
        }
        quest->CompletionMessage = CompletionMessage;
        quest->Description = Description;
        quest->DestinationMapContentName = DestinationMapContentName;
        quest->DestinationNpcContentName = DestinationNpcContentName;
        quest->DestinationObjectiveMessage = DestinationObjectiveMessage;
        quest->ExperienceReward = ExperienceReward;
        quest->FixedCombatEntries = FixedCombatEntries;
        quest->GearRequirements = GearRequirements;
        quest->GearRewardContentNames = GearRewardContentNames;
        quest->GearRewards = GearRewards;
        quest->GoldReward = GoldReward;
        quest->MonsterRequirements = MonsterRequirements;
        quest->Name = Name;
        quest->ObjectiveMessage = ObjectiveMessage;
        quest->Stage = Stage;
        return quest;
    }
};

// Reads a Quest object from the content pipeline.
class QuestReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Quest>> {
public:
    QuestReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Quest>>(
              "RolePlayingGameData.Quest") {}

protected:
    std::shared_ptr<Quest> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Quest>> existingInstance) override {
        std::shared_ptr<Quest> quest = existingInstance.has_value() ? *existingInstance : nullptr;
        if (quest == nullptr) {
            quest = std::make_shared<Quest>();
        }
        auto* content = input.getContentManagerProperty();

        quest->SetAssetName(input.getAssetNameProperty());
        quest->Name = input.ReadString();
        quest->Description = input.ReadString();
        quest->ObjectiveMessage = input.ReadString();
        quest->CompletionMessage = input.ReadString();

        const auto gearRequirements =
            input.ReadObject<std::vector<std::shared_ptr<QuestRequirement<Gear>>>>();
        quest->GearRequirements.insert(
            quest->GearRequirements.end(), gearRequirements.begin(), gearRequirements.end());
        const auto monsterRequirements =
            input.ReadObject<std::vector<std::shared_ptr<QuestRequirement<Monster>>>>();
        quest->MonsterRequirements.insert(
            quest->MonsterRequirements.end(),
            monsterRequirements.begin(), monsterRequirements.end());

        // load the fixed combat entries
        System::Random random;
        const auto fixedCombatEntries =
            input.ReadObject<std::vector<std::shared_ptr<WorldEntry<FixedCombat>>>>();
        quest->FixedCombatEntries.insert(
            quest->FixedCombatEntries.end(),
            fixedCombatEntries.begin(), fixedCombatEntries.end());
        for (const auto& fixedCombatEntry : quest->FixedCombatEntries) {
            fixedCombatEntry->Content = content->Load<std::shared_ptr<FixedCombat>>(
                "Maps/FixedCombats/" + fixedCombatEntry->ContentName);
            // clone the map sprite in the entry, as there may be many entries per FixedCombat
            fixedCombatEntry->MapSprite =
                fixedCombatEntry->Content->Entries[0]->Content->MapSprite->Clone();
            // play the idle animation
            fixedCombatEntry->MapSprite->PlayAnimation("Idle", fixedCombatEntry->EntryDirection);
            // advance in a random amount so the animations aren't synchronized
            fixedCombatEntry->MapSprite->UpdateAnimation(
                4.0f * static_cast<float>(random.NextDouble()));
        }

        const auto chestEntries =
            input.ReadObject<std::vector<std::shared_ptr<WorldEntry<Chest>>>>();
        quest->ChestEntries.insert(
            quest->ChestEntries.end(), chestEntries.begin(), chestEntries.end());
        for (const auto& chestEntry : quest->ChestEntries) {
            chestEntry->Content =
                content->Load<std::shared_ptr<Chest>>("Maps/Chests/" + chestEntry->ContentName)
                    ->Clone();
        }

        quest->DestinationMapContentName = input.ReadString();
        quest->DestinationNpcContentName = input.ReadString();
        quest->DestinationObjectiveMessage = input.ReadString();
        quest->ExperienceReward = static_cast<int>(input.ReadInt32());
        quest->GoldReward = static_cast<int>(input.ReadInt32());

        const auto gearRewardContentNames = input.ReadObject<std::vector<std::string>>();
        quest->GearRewardContentNames.insert(
            quest->GearRewardContentNames.end(),
            gearRewardContentNames.begin(), gearRewardContentNames.end());
        for (const std::string& contentName : quest->GearRewardContentNames) {
            quest->GearRewards.push_back(
                content->Load<std::shared_ptr<Gear>>("Gear/" + contentName));
        }

        return quest;
    }
};

// The save file stores the current quest's stage as its enumerator name, which is what
// Enum.Parse/ToString read and write in the original.
SHARP_XML_ENUM(Quest::QuestStage, SHARP_XML_E(Quest::QuestStage, NotStarted),
               SHARP_XML_E(Quest::QuestStage, InProgress),
               SHARP_XML_E(Quest::QuestStage, RequirementsMet),
               SHARP_XML_E(Quest::QuestStage, Completed))

} // namespace RolePlayingGameData
