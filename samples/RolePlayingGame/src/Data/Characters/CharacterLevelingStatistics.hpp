#pragma once

#include <memory>
#include <optional>
#include <vector>
#include <string>

// CharacterLevelingStatistics.hpp -- C++ port of
// RolePlayingGameData/Characters/CharacterLevelingStatistics.cs.

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

struct CharacterLevelingStatistics {
    int HealthPointsIncrease = 0;
    int LevelsPerHealthPointsIncrease = 0;
    int MagicPointsIncrease = 0;
    int LevelsPerMagicPointsIncrease = 0;
    int PhysicalOffenseIncrease = 0;
    int LevelsPerPhysicalOffenseIncrease = 0;
    int PhysicalDefenseIncrease = 0;
    int LevelsPerPhysicalDefenseIncrease = 0;
    int MagicalOffenseIncrease = 0;
    int LevelsPerMagicalOffenseIncrease = 0;
    int MagicalDefenseIncrease = 0;
    int LevelsPerMagicalDefenseIncrease = 0;
};

// Reads a CharacterLevelingStatistics object from the content pipeline.
class CharacterLevelingStatisticsReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<CharacterLevelingStatistics> {
public:
    CharacterLevelingStatisticsReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<CharacterLevelingStatistics>(
              "RolePlayingGameData.CharacterLevelingStatistics") {}

protected:
    CharacterLevelingStatistics Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<CharacterLevelingStatistics> existingInstance) override {
        CharacterLevelingStatistics stats =
            existingInstance.value_or(CharacterLevelingStatistics());

        stats.HealthPointsIncrease = static_cast<int>(input.ReadInt32());
        stats.MagicPointsIncrease = static_cast<int>(input.ReadInt32());
        stats.PhysicalOffenseIncrease = static_cast<int>(input.ReadInt32());
        stats.PhysicalDefenseIncrease = static_cast<int>(input.ReadInt32());
        stats.MagicalOffenseIncrease = static_cast<int>(input.ReadInt32());
        stats.MagicalDefenseIncrease = static_cast<int>(input.ReadInt32());
        stats.LevelsPerHealthPointsIncrease = static_cast<int>(input.ReadInt32());
        stats.LevelsPerMagicPointsIncrease = static_cast<int>(input.ReadInt32());
        stats.LevelsPerPhysicalOffenseIncrease = static_cast<int>(input.ReadInt32());
        stats.LevelsPerPhysicalDefenseIncrease = static_cast<int>(input.ReadInt32());
        stats.LevelsPerMagicalOffenseIncrease = static_cast<int>(input.ReadInt32());
        stats.LevelsPerMagicalDefenseIncrease = static_cast<int>(input.ReadInt32());

        return stats;
    }
};

} // namespace RolePlayingGameData
