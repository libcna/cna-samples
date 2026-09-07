#pragma once

// PartySaveRestore.hpp -- the out-of-line half of Party's save-data constructor.
//
// It lives beside Party rather than inside it because PartySaveData is built from a Party, so the
// two headers cannot include each other. The body is the original's, statement for statement.

#include <memory>
#include <string>

#include "System/ArgumentNullException.hpp"

#include "../Data/Gear/Equipment.hpp"
#include "../Data/Gear/Gear.hpp"
#include "Party.hpp"
#include "PartySaveData.hpp"

namespace RolePlaying {

inline Party::Party(const PartySaveData& partyData, ContentManager& contentManager) {
    for (const PlayerSaveData& playerData : partyData.players) {
        auto player = contentManager
                          .Load<std::shared_ptr<RolePlayingGameData::Player>>(playerData.assetName)
                          ->Clone();
        player->SetCharacterLevel(playerData.characterLevel);
        player->SetExperience(playerData.experience);
        player->EquippedEquipment().clear();
        for (const std::string& equipmentAssetName : playerData.equipmentAssetNames) {
            // Every gear reader targets the Gear base, so the narrowing C# performs at the
            // Load<Equipment> call site is written out here; see FightingCharacterReader.
            player->Equip(std::dynamic_pointer_cast<RolePlayingGameData::Equipment>(
                contentManager.Load<std::shared_ptr<RolePlayingGameData::Gear>>(
                    equipmentAssetName)));
        }
        player->StatisticsModifiers = playerData.statisticsModifiers;
        JoinParty(player);
    }

    inventory_.clear();
    for (const auto& entry : partyData.inventory) {
        inventory_.push_back(
            std::make_shared<RolePlayingGameData::ContentEntry<RolePlayingGameData::Gear>>(entry));
    }
    for (const auto& entry : inventory_) {
        entry->Content = contentManager.Load<std::shared_ptr<RolePlayingGameData::Gear>>(
            "Gear/" + entry->ContentName);
    }

    partyGold_ = partyData.partyGold;

    for (std::size_t i = 0; i < partyData.monsterKillNames.size(); i++) {
        monsterKills_.emplace(partyData.monsterKillNames[i], partyData.monsterKillCounts[i]);
    }
}

} // namespace RolePlaying
