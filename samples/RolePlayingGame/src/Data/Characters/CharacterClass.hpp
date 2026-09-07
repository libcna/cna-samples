#pragma once

// CharacterClass.hpp -- C++ port of RolePlayingGameData/Characters/CharacterClass.cs.

#include <optional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "../ContentObject.hpp"
#include "../StatisticsValue.hpp"
#include "CharacterLevelDescription.hpp"
#include "CharacterLevelingStatistics.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

class CharacterClass : public ContentObject {
public:
    std::string Name;
    StatisticsValue InitialStatistics;
    CharacterLevelingStatistics LevelingStatistics;
    std::vector<CharacterLevelDescription> LevelEntries;

    // Calculate the statistics of a character of this class and the given level.
    StatisticsValue GetStatisticsForLevel(int characterLevel) const {
        if (characterLevel <= 0) throw std::out_of_range("characterLevel");

        StatisticsValue output = InitialStatistics;
        for (int i = 1; i < characterLevel; i++) {
            if (LevelingStatistics.LevelsPerHealthPointsIncrease > 0 &&
                (i % LevelingStatistics.LevelsPerHealthPointsIncrease) == 0)
                output.HealthPoints += LevelingStatistics.HealthPointsIncrease;
            if (LevelingStatistics.LevelsPerMagicPointsIncrease > 0 &&
                (i % LevelingStatistics.LevelsPerMagicPointsIncrease) == 0)
                output.MagicPoints += LevelingStatistics.MagicPointsIncrease;
            if (LevelingStatistics.LevelsPerPhysicalOffenseIncrease > 0 &&
                (i % LevelingStatistics.LevelsPerPhysicalOffenseIncrease) == 0)
                output.PhysicalOffense += LevelingStatistics.PhysicalOffenseIncrease;
            if (LevelingStatistics.LevelsPerPhysicalDefenseIncrease > 0 &&
                (i % LevelingStatistics.LevelsPerPhysicalDefenseIncrease) == 0)
                output.PhysicalDefense += LevelingStatistics.PhysicalDefenseIncrease;
            if (LevelingStatistics.LevelsPerMagicalOffenseIncrease > 0 &&
                (i % LevelingStatistics.LevelsPerMagicalOffenseIncrease) == 0)
                output.MagicalOffense += LevelingStatistics.MagicalOffenseIncrease;
            if (LevelingStatistics.LevelsPerMagicalDefenseIncrease > 0 &&
                (i % LevelingStatistics.LevelsPerMagicalDefenseIncrease) == 0)
                output.MagicalDefense += LevelingStatistics.MagicalDefenseIncrease;
        }
        return output;
    }

    // Build a list of all spells available to a character of this class and the given level.
    std::vector<std::shared_ptr<Spell>> GetAllSpellsForLevel(int characterLevel) const {
        if (characterLevel <= 0) throw std::out_of_range("characterLevel");

        std::vector<std::shared_ptr<Spell>> spells;
        for (int i = 0; i < characterLevel; i++) {
            if (i >= (int)LevelEntries.size()) break;
            for (auto& spell : LevelEntries[i].Spells) {
                std::shared_ptr<Spell> existing;
                for (auto& s : spells) {
                    if (s->AssetName() == spell->AssetName()) { existing = s; break; }
                }
                if (!existing) {
                    spells.push_back(spell->Clone());
                } else {
                    existing->SetLevel(existing->Level() + 1);
                }
            }
        }
        return spells;
    }

    int BaseExperienceValue = 0;
    int BaseGoldValue = 0;
};

// Reads a CharacterClass object from the content pipeline.
class CharacterClassReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<
          std::shared_ptr<CharacterClass>> {
public:
    CharacterClassReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<CharacterClass>>(
              "RolePlayingGameData.CharacterClass") {}

protected:
    std::shared_ptr<CharacterClass> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<CharacterClass>> existingInstance) override {
        std::shared_ptr<CharacterClass> characterClass =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (characterClass == nullptr) {
            characterClass = std::make_shared<CharacterClass>();
        }

        characterClass->SetAssetName(input.getAssetNameProperty());
        characterClass->Name = input.ReadString();
        characterClass->InitialStatistics = input.ReadObject<StatisticsValue>();
        characterClass->LevelingStatistics = input.ReadObject<CharacterLevelingStatistics>();
        // The .xnb holds a list of references, as C# has; this port stores the descriptions by
        // value, so each one is copied out of the reference the protocol produces.
        const auto levelEntries =
            input.ReadObject<std::vector<std::shared_ptr<CharacterLevelDescription>>>();
        for (const auto& entry : levelEntries) {
            characterClass->LevelEntries.push_back(*entry);
        }
        characterClass->BaseExperienceValue = static_cast<int>(input.ReadInt32());
        characterClass->BaseGoldValue = static_cast<int>(input.ReadInt32());

        return characterClass;
    }
};

} // namespace RolePlayingGameData
