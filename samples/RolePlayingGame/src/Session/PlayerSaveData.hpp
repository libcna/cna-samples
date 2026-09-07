#pragma once

// PlayerSaveData.hpp -- C++ port of RolePlayingGame/Session/PlayerSaveData.cs.

#include <string>
#include <vector>

#include "System/ArgumentNullException.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"

#include "../Data/Characters/Player.hpp"
#include "../Data/StatisticsValue.hpp"

namespace RolePlaying {

// The data needed to persist a player between sessions.
//
// The original is serialized by XmlSerializer, which reflects over these public fields. C++ has no
// reflection, so the member list is declared; the names, order and types are the original's, and
// the document XmlSerializer writes is the same one. See diff.md.
class PlayerSaveData {
public:
    std::string assetName;
    int characterLevel = 0;
    int experience = 0;
    std::vector<std::string> equipmentAssetNames;
    RolePlayingGameData::StatisticsValue statisticsModifiers;

    SHARP_XML_SERIALIZABLE(PlayerSaveData, "PlayerSaveData",
                           SHARP_XML_M(PlayerSaveData, assetName),
                           SHARP_XML_M(PlayerSaveData, characterLevel),
                           SHARP_XML_M(PlayerSaveData, experience),
                           SHARP_XML_M(PlayerSaveData, equipmentAssetNames),
                           SHARP_XML_M(PlayerSaveData, statisticsModifiers))

    PlayerSaveData() = default;

    explicit PlayerSaveData(const std::shared_ptr<RolePlayingGameData::Player>& player) {
        // check the parameter
        if (player == nullptr) {
            throw System::ArgumentNullException("player");
        }

        assetName = player->AssetName();
        characterLevel = player->CharacterLevel();
        experience = player->Experience();
        for (const auto& equipment : player->EquippedEquipment()) {
            equipmentAssetNames.push_back(equipment->AssetName());
        }
        statisticsModifiers = player->StatisticsModifiers;
    }
};

} // namespace RolePlaying
