#pragma once

// ModifiedChestEntry.hpp -- C++ port of Session/ModifiedChestEntry.cs.

#include <memory>
#include <vector>

#include "../Data/ContentEntry.hpp"
#include "../Data/Gear/Gear.hpp"
#include "../Data/Map/Chest.hpp"
#include "../Data/WorldEntry.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"

namespace RolePlaying {

class ModifiedChestEntry {
public:
    RolePlayingGameData::WorldEntry<RolePlayingGameData::Chest> WorldEntry;
    std::vector<RolePlayingGameData::ContentEntry<RolePlayingGameData::Gear>> ChestEntries;
    int Gold = 0;

    SHARP_XML_SERIALIZABLE(ModifiedChestEntry, "ModifiedChestEntry",
                           SHARP_XML_M(ModifiedChestEntry, WorldEntry),
                           SHARP_XML_M(ModifiedChestEntry, ChestEntries),
                           SHARP_XML_M(ModifiedChestEntry, Gold))
};

} // namespace RolePlaying
