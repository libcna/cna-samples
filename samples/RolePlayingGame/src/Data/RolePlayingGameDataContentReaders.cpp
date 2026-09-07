// RolePlayingGameDataContentReaders.cpp -- CNAEXT. See the header for why this file exists.

#include "RolePlayingGameDataContentReaders.hpp"

#include <memory>
#include <string>
#include <vector>

#include "CNA/Internal/Xnb/CollectionContentTypeReaders.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"

#include "AnimatingSprite.hpp"
#include "Animation.hpp"
#include "ContentEntry.hpp"
// FightingCharacter.hpp carries the out-of-line definition of Gear::CheckRestrictions, which the
// gear vtables need: the declaration in Gear.hpp cannot define it because FightingCharacter is
// still incomplete there.
#include "Characters/FightingCharacter.hpp"
#include "Gear/Armor.hpp"
#include "Gear/Equipment.hpp"
#include "Gear/Gear.hpp"
#include "Gear/GearDrop.hpp"
#include "Gear/Item.hpp"
#include "Gear/Weapon.hpp"
#include "Int32Range.hpp"
#include "Map/Inn.hpp"
#include "MapEntry.hpp"
#include "StatisticsRange.hpp"
#include "StatisticsValue.hpp"
#include "WeightedContentEntry.hpp"
#include "WorldEntry.hpp"
#include "WorldObject.hpp"

namespace RolePlayingGameData {

namespace {

using CNA::Internal::Xnb::ListReader;
using Microsoft::Xna::Framework::Content::ContentTypeReaderManager;

// The .xnb spells a generic reader "Outer`1+OuterReader[[Argument]]" and its list
// "ListReader`1[[Outer`1[[Argument]]]]"; CNA hands the name over with assembly qualifiers already
// stripped, so these are the exact keys the corpus records. Each instantiation is registered as
// its argument type lands in the port.
std::string ListReaderName(const std::string& element) {
    return "Microsoft.Xna.Framework.Content.ListReader`1[[" + element + "]]";
}

std::string ListTypeName(const std::string& element) {
    return "System.Collections.Generic.List`1[[" + element + "]]";
}

} // namespace

void RegisterContentTypeReaders() {
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.WorldObject+WorldObjectReader",
        [] { return std::make_unique<WorldObjectReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Inn+InnReader", [] { return std::make_unique<InnReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Int32Range+Int32RangeReader",
        [] { return std::make_unique<Int32RangeReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.StatisticsValue+StatisticsValueReader",
        [] { return std::make_unique<StatisticsValueReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.StatisticsRange+StatisticsRangeReader",
        [] { return std::make_unique<StatisticsRangeReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Animation+AnimationReader",
        [] { return std::make_unique<AnimationReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.AnimatingSprite+AnimatingSpriteReader",
        [] { return std::make_unique<AnimatingSpriteReader>(); });

    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Gear+GearReader", [] { return std::make_unique<GearReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Equipment+EquipmentReader",
        [] { return std::make_unique<EquipmentReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Armor+ArmorReader", [] { return std::make_unique<ArmorReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Weapon+WeaponReader",
        [] { return std::make_unique<WeaponReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Item+ItemReader", [] { return std::make_unique<ItemReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.GearDrop+GearDropReader",
        [] { return std::make_unique<GearDropReader>(); });

    ContentTypeReaderManager::AddTypeCreator(
        ListReaderName("RolePlayingGameData.GearDrop"), [] {
            return std::make_unique<ListReader<std::shared_ptr<GearDrop>>>(
                ListTypeName("RolePlayingGameData.GearDrop"),
                "RolePlayingGameData.GearDrop+GearDropReader");
        });
    ContentTypeReaderManager::AddTypeCreator(
        ListReaderName("RolePlayingGameData.Animation"), [] {
            return std::make_unique<ListReader<std::shared_ptr<Animation>>>(
                ListTypeName("RolePlayingGameData.Animation"),
                "RolePlayingGameData.Animation+AnimationReader");
        });
}

} // namespace RolePlayingGameData
