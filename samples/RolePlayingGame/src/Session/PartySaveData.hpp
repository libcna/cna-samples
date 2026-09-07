#pragma once

// PartySaveData.hpp -- C++ port of RolePlayingGame/Session/PartySaveData.cs.

#include <memory>
#include <string>
#include <vector>

#include "System/ArgumentNullException.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"

#include "../Data/ContentEntry.hpp"
#include "../Data/Gear/Gear.hpp"
#include "Party.hpp"
#include "PlayerSaveData.hpp"

namespace RolePlaying {

// The data needed to persist the party between sessions.
//
// As with PlayerSaveData, the member list is declared because C++ has no reflection; the names,
// order and types are the original's. See diff.md.
class PartySaveData {
public:
    std::vector<PlayerSaveData> players;
    std::vector<RolePlayingGameData::ContentEntry<RolePlayingGameData::Gear>> inventory;
    int partyGold = 0;
    std::vector<std::string> monsterKillNames;
    std::vector<int> monsterKillCounts;

    SHARP_XML_SERIALIZABLE(PartySaveData, "PartySaveData",
                           SHARP_XML_M(PartySaveData, players),
                           SHARP_XML_M(PartySaveData, inventory),
                           SHARP_XML_M(PartySaveData, partyGold),
                           SHARP_XML_M(PartySaveData, monsterKillNames),
                           SHARP_XML_M(PartySaveData, monsterKillCounts))

    PartySaveData() = default;

    explicit PartySaveData(const Party* party) {
        // check the parameter
        if (party == nullptr) {
            throw System::ArgumentNullException("party");
        }

        // create and add the serializable player data
        for (const auto& player : party->Players) {
            players.emplace_back(player);
        }

        // add the items
        //
        // The original's list holds references to the party's own entries; this port stores them
        // by value, because the save document carries the values and nothing reads the copy back
        // into the live party.
        for (const auto& entry : party->Inventory()) {
            inventory.push_back(*entry);
        }

        // store the amount of gold held by the party
        partyGold = party->PartyGold();

        // add the monster kill data for the active quest
        for (const auto& [monsterName, count] : party->MonsterKills()) {
            monsterKillNames.push_back(monsterName);
            monsterKillCounts.push_back(count);
        }
    }
};

} // namespace RolePlaying
