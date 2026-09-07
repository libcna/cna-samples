#pragma once

// Monster.hpp -- C++ port of RolePlayingGameData/Characters/Monster.cs.

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "../Gear/GearDrop.hpp"
#include "FightingCharacter.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

class Monster : public FightingCharacter {
public:
    int DefendPercentage = 0;
    std::vector<GearDrop> GearDrops;

    int CalculateGoldReward(System::Random& random) const {
        return GetCharacterClass()->BaseGoldValue * CharacterLevel();
    }

    int CalculateExperienceReward(System::Random& random) const {
        return GetCharacterClass()->BaseExperienceValue * CharacterLevel();
    }

    std::vector<std::string> CalculateGearDrop(System::Random& random) const {
        std::vector<std::string> gearNames;
        for (auto& drop : GearDrops) {
            if (random.Next(0, 100) < drop.DropPercentage()) gearNames.push_back(drop.GearName);
        }
        return gearNames;
    }
};

// Reads a Monster object from the content pipeline.
class MonsterReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Monster>> {
public:
    MonsterReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Monster>>(
              "RolePlayingGameData.Monster") {}

protected:
    std::shared_ptr<Monster> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Monster>> existingInstance) override {
        std::shared_ptr<Monster> monster =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (monster == nullptr) {
            monster = std::make_shared<Monster>();
        }

        FightingCharacterReader fightingCharacterReader;
        input.ReadRawObject<std::shared_ptr<FightingCharacter>>(
            fightingCharacterReader, std::static_pointer_cast<FightingCharacter>(monster));

        monster->DefendPercentage = static_cast<int>(input.ReadInt32());
        // The .xnb holds references, as C# does; this port stores the drops by value.
        const auto gearDrops = input.ReadObject<std::vector<std::shared_ptr<GearDrop>>>();
        for (const auto& gearDrop : gearDrops) {
            monster->GearDrops.push_back(*gearDrop);
        }

        return monster;
    }
};

} // namespace RolePlayingGameData
