#pragma once

// RandomCombat.hpp -- C++ port of RolePlayingGameData/Map/RandomCombat.cs.

#include <optional>
#include <string>
#include <memory>
#include <vector>

#include "../Characters/Monster.hpp"
#include "../Int32Range.hpp"
#include "../WeightedContentEntry.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

// Description of possible random combats in a particular map.
class RandomCombat {
public:
    // The chance of a random combat starting with each step, from 1 to 100.
    int CombatProbability = 0;
    // The chance of a successful escape from a random combat, from 1 to 100.
    int FleeProbability = 0;
    Int32Range MonsterCountRange;
    std::vector<std::shared_ptr<WeightedContentEntry<Monster>>> Entries;
};

// Reads a RandomCombat object from the content pipeline.
class RandomCombatReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<RandomCombat>> {
public:
    RandomCombatReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<RandomCombat>>(
              "RolePlayingGameData.RandomCombat") {}

protected:
    std::shared_ptr<RandomCombat> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<RandomCombat>> existingInstance) override {
        std::shared_ptr<RandomCombat> randomCombat =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (randomCombat == nullptr) {
            randomCombat = std::make_shared<RandomCombat>();
        }

        randomCombat->CombatProbability = static_cast<int>(input.ReadInt32());
        randomCombat->FleeProbability = static_cast<int>(input.ReadInt32());
        randomCombat->MonsterCountRange = input.ReadObject<Int32Range>();
        const auto entries =
            input.ReadObject<std::vector<std::shared_ptr<WeightedContentEntry<Monster>>>>();
        randomCombat->Entries.insert(randomCombat->Entries.end(), entries.begin(), entries.end());
        for (const auto& randomCombatEntry : randomCombat->Entries) {
            randomCombatEntry->Content =
                input.getContentManagerProperty()->Load<std::shared_ptr<Monster>>(
                    "Characters/Monsters/" + randomCombatEntry->ContentName);
        }

        return randomCombat;
    }
};

} // namespace RolePlayingGameData
