#pragma once

// Party.hpp -- C++ port of Session/Party.cs. The group of players, under
// control of the user.

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"

#include "../Data/Characters/Monster.hpp"
#include "../Data/Characters/Player.hpp"
#include "../Data/ContentEntry.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "../Data/Characters/Player.hpp"
#include "../Data/GameStartDescription.hpp"

#include "System/ArgumentException.hpp"

#include "System/ArgumentNullException.hpp"

#include "System/ArgumentOutOfRangeException.hpp"

namespace RolePlaying {

class PartySaveData; // fwd decl -- see PartySaveData.hpp

using RolePlayingGameData::ContentEntry;
using Microsoft::Xna::Framework::Content::ContentManager;
using RolePlayingGameData::Gear;
using RolePlayingGameData::GameStartDescription;
using RolePlayingGameData::Monster;
using RolePlayingGameData::Player;

class Party {
public:
    // The first entry is the leader.
    std::vector<std::shared_ptr<Player>> Players;

    void JoinParty(const std::shared_ptr<Player>& player) {
        // check the parameter
        if (player == nullptr) {
            throw System::ArgumentNullException("player");
        }
        if (std::find(Players.begin(), Players.end(), player) != Players.end()) {
            throw System::ArgumentException("The player was already in the party.");
        }
        // add the new player to the list
        Players.push_back(player);
        partyGold_ += player->Gold;
        player->Gold = 0;
        for (auto& entry : player->Inventory) AddToInventory(entry->Content, entry->Count);
        player->Inventory.clear();
    }

    // Defined out-of-line where Session/LevelUpScreen are available (see
    // GameplayScreen.hpp's cross-referencing definitions).
    void GiveExperience(int experience);

    const std::vector<std::shared_ptr<ContentEntry<Gear>>>& Inventory() const { return inventory_; }

    void AddToInventory(const std::shared_ptr<Gear>& gear, int count) {
        if (!gear || count <= 0) return;
        for (auto& entry : inventory_) {
            if (entry->Content == gear) { entry->Count += count; return; }
        }
        auto entry = std::make_shared<ContentEntry<Gear>>();
        entry->Content = gear;
        entry->Count = count;
        entry->ContentName = gear->AssetName();
        const std::string prefix = "Gear/";
        if (entry->ContentName.rfind(prefix, 0) == 0) entry->ContentName = entry->ContentName.substr(prefix.size());
        inventory_.push_back(entry);
    }

    bool RemoveFromInventory(const std::shared_ptr<Gear>& gear, int count) {
        // check the parameters
        if (gear == nullptr) {
            throw System::ArgumentNullException("gear");
        }
        if (count <= 0) {
            throw System::ArgumentOutOfRangeException("count");
        }
        for (size_t i = 0; i < inventory_.size(); i++) {
            if (inventory_[i]->Content == gear) {
                inventory_[i]->Count -= count;
                bool fullRemoval = inventory_[i]->Count >= 0;
                if (inventory_[i]->Count <= 0) inventory_.erase(inventory_.begin() + i);
                return fullRemoval;
            }
        }
        return false;
    }

    int PartyGold() const { return partyGold_; }
    void SetPartyGold(int v) { partyGold_ = v; }
    void AddPartyGold(int delta) { partyGold_ += delta; }

    const std::unordered_map<std::string, int>& MonsterKills() const { return monsterKills_; }
    void AddMonsterKill(const Monster& monster) {
        auto it = monsterKills_.find(monster.AssetName());
        if (it != monsterKills_.end()) {
            it->second++;
        } else {
            monsterKills_.emplace(monster.AssetName(), 1);
        }
    }
    void ClearMonsterKills() { monsterKills_.clear(); }

    Party() = default;

    Party(const GameStartDescription& gameStartDescription, ContentManager& contentManager) {
        // load the players
        for (auto& contentName : gameStartDescription.PlayerContentNames) {
            JoinParty(contentManager.Load<std::shared_ptr<RolePlayingGameData::Player>>(
                                        "Characters/Players/" + contentName)
                          ->Clone());
        }
    }

    // Restores a party from a save file. The equipment and inventory names come back as strings,
    // so each is loaded again through ContentManager, exactly as the original does.
    Party(const PartySaveData& partyData, ContentManager& contentManager);

private:
    std::vector<std::shared_ptr<ContentEntry<Gear>>> inventory_;
    int partyGold_ = 0;
    std::unordered_map<std::string, int> monsterKills_;
};

} // namespace RolePlaying
