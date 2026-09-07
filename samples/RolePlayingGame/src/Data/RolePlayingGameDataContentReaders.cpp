// RolePlayingGameDataContentReaders.cpp -- CNAEXT. See the header for why this file exists.

#include "RolePlayingGameDataContentReaders.hpp"

#include <cstdint>
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
#include "Characters/CharacterClass.hpp"
#include "Characters/Monster.hpp"
#include "Characters/Player.hpp"
#include "Characters/QuestNpc.hpp"
#include "Characters/FightingCharacter.hpp"
#include "Quests/Quest.hpp"
#include "Quests/QuestLine.hpp"
#include "Quests/QuestRequirement.hpp"
#include "Spell.hpp"
#include "Gear/Armor.hpp"
#include "Gear/Equipment.hpp"
#include "Gear/Gear.hpp"
#include "Gear/GearDrop.hpp"
#include "Gear/Item.hpp"
#include "Gear/Weapon.hpp"
#include "Int32Range.hpp"
#include "GameStartDescription.hpp"
#include "Map/Chest.hpp"
#include "Map/FixedCombat.hpp"
#include "Map/Inn.hpp"
#include "Map/Map.hpp"
#include "Map/Portal.hpp"
#include "Map/RandomCombat.hpp"
#include "Map/Store.hpp"
#include "Map/StoreCategory.hpp"
#include "MapEntry.hpp"
#include "StatisticsRange.hpp"
#include "StatisticsValue.hpp"
#include "WeightedContentEntry.hpp"
#include "WorldEntry.hpp"
#include "WorldObject.hpp"

namespace RolePlayingGameData {

namespace {

using CNA::Internal::Xnb::ArrayReader;
using CNA::Internal::Xnb::ListReader;
using Microsoft::Xna::Framework::Content::ContentTypeReaderManager;

// The .xnb spells a generic reader "Outer`1+OuterReader[[Argument]]" and its list
// "ListReader`1[[Outer`1[[Argument]]]]"; CNA hands the name over with assembly qualifiers already
// stripped, so these are the exact keys the corpus records. Each instantiation is registered as
// its argument type lands in the port.
std::string GenericReader(const std::string& outer, const std::string& argument) {
    const std::string::size_type dot = outer.rfind('.');
    const std::string leaf = dot == std::string::npos ? outer : outer.substr(dot + 1);
    return outer + "`1+" + leaf + "Reader[[" + argument + "]]";
}

std::string GenericType(const std::string& outer, const std::string& argument) {
    return outer + "`1[[" + argument + "]]";
}

std::string ListReaderName(const std::string& element) {
    return "Microsoft.Xna.Framework.Content.ListReader`1[[" + element + "]]";
}

std::string ListTypeName(const std::string& element) {
    return "System.Collections.Generic.List`1[[" + element + "]]";
}

// A map entry family: the entry reader itself, and the list of entries a map holds.
template <typename T>
void RegisterMapEntryFamily(const std::string& argument) {
    const std::string mapEntry = "RolePlayingGameData.MapEntry";
    ContentTypeReaderManager::AddTypeCreator(
        GenericReader(mapEntry, argument),
        [mapEntry] { return std::make_unique<MapEntryReader<T>>(mapEntry); });
    ContentTypeReaderManager::AddTypeCreator(
        ListReaderName(GenericType(mapEntry, argument)), [mapEntry, argument] {
            return std::make_unique<ListReader<std::shared_ptr<MapEntry<T>>>>(
                ListTypeName(GenericType(mapEntry, argument)), GenericReader(mapEntry, argument));
        });
}

// A world-entry family: a map entry that also names the map it lives on.
template <typename T>
void RegisterWorldEntryFamily(const std::string& argument) {
    const std::string worldEntry = "RolePlayingGameData.WorldEntry";
    ContentTypeReaderManager::AddTypeCreator(
        GenericReader(worldEntry, argument),
        [worldEntry] { return std::make_unique<WorldEntryReader<T>>(worldEntry); });
    ContentTypeReaderManager::AddTypeCreator(
        ListReaderName(GenericType(worldEntry, argument)), [worldEntry, argument] {
            return std::make_unique<ListReader<std::shared_ptr<WorldEntry<T>>>>(
                ListTypeName(GenericType(worldEntry, argument)),
                GenericReader(worldEntry, argument));
        });
}

// A quest-requirement family, with the content directory the original's typeof(T) branch picks.
template <typename T>
void RegisterQuestRequirementFamily(const std::string& argument,
                                    const std::string& contentDirectory) {
    const std::string requirement = "RolePlayingGameData.QuestRequirement";
    ContentTypeReaderManager::AddTypeCreator(
        GenericReader(requirement, argument), [requirement, contentDirectory] {
            return std::make_unique<QuestRequirementReader<T>>(requirement, contentDirectory);
        });
    ContentTypeReaderManager::AddTypeCreator(
        ListReaderName(GenericType(requirement, argument)), [requirement, argument] {
            return std::make_unique<ListReader<std::shared_ptr<QuestRequirement<T>>>>(
                ListTypeName(GenericType(requirement, argument)),
                GenericReader(requirement, argument));
        });
}

// A content-entry family: the entry reader, and the list a container holds.
template <typename T>
void RegisterContentEntryFamily(const std::string& argument) {
    const std::string contentEntry = "RolePlayingGameData.ContentEntry";
    ContentTypeReaderManager::AddTypeCreator(
        GenericReader(contentEntry, argument),
        [contentEntry] { return std::make_unique<ContentEntryReader<T>>(contentEntry); });
    ContentTypeReaderManager::AddTypeCreator(
        ListReaderName(GenericType(contentEntry, argument)), [contentEntry, argument] {
            return std::make_unique<ListReader<std::shared_ptr<ContentEntry<T>>>>(
                ListTypeName(GenericType(contentEntry, argument)),
                GenericReader(contentEntry, argument));
        });
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
        "RolePlayingGameData.Spell+SpellReader", [] { return std::make_unique<SpellReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.CharacterClass+CharacterClassReader",
        [] { return std::make_unique<CharacterClassReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.CharacterLevelDescription+CharacterLevelDescriptionReader",
        [] { return std::make_unique<CharacterLevelDescriptionReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.CharacterLevelingStatistics+CharacterLevelingStatisticsReader",
        [] { return std::make_unique<CharacterLevelingStatisticsReader>(); });

    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Character+CharacterReader",
        [] { return std::make_unique<CharacterReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.FightingCharacter+FightingCharacterReader",
        [] { return std::make_unique<FightingCharacterReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Monster+MonsterReader",
        [] { return std::make_unique<MonsterReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Player+PlayerReader",
        [] { return std::make_unique<PlayerReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.QuestNpc+QuestNpcReader",
        [] { return std::make_unique<QuestNpcReader>(); });

    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Chest+ChestReader", [] { return std::make_unique<ChestReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Store+StoreReader", [] { return std::make_unique<StoreReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.StoreCategory+StoreCategoryReader",
        [] { return std::make_unique<StoreCategoryReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Portal+PortalReader", [] { return std::make_unique<PortalReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.FixedCombat+FixedCombatReader",
        [] { return std::make_unique<FixedCombatReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.RandomCombat+RandomCombatReader",
        [] { return std::make_unique<RandomCombatReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Map+MapReader", [] { return std::make_unique<MapReader>(); });

    // The content-entry families: gear an owner carries, and the monsters a combat draws from.
    RegisterContentEntryFamily<Gear>("RolePlayingGameData.Gear");
    RegisterContentEntryFamily<Monster>("RolePlayingGameData.Monster");
    RegisterContentEntryFamily<Chest>("RolePlayingGameData.Chest");
    RegisterContentEntryFamily<FixedCombat>("RolePlayingGameData.FixedCombat");
    RegisterContentEntryFamily<Store>("RolePlayingGameData.Store");
    RegisterContentEntryFamily<Portal>("RolePlayingGameData.Portal");
    RegisterContentEntryFamily<Inn>("RolePlayingGameData.Inn");
    RegisterContentEntryFamily<QuestNpc>("RolePlayingGameData.QuestNpc");
    RegisterContentEntryFamily<Player>("RolePlayingGameData.Player");

    // The map-entry families: everything a map places on its grid.
    RegisterMapEntryFamily<Chest>("RolePlayingGameData.Chest");
    RegisterMapEntryFamily<FixedCombat>("RolePlayingGameData.FixedCombat");
    RegisterMapEntryFamily<Store>("RolePlayingGameData.Store");
    RegisterMapEntryFamily<Portal>("RolePlayingGameData.Portal");
    RegisterMapEntryFamily<Inn>("RolePlayingGameData.Inn");
    RegisterMapEntryFamily<QuestNpc>("RolePlayingGameData.QuestNpc");
    RegisterMapEntryFamily<Player>("RolePlayingGameData.Player");

    // Weighted monster entries, which only a random combat holds.
    {
        const std::string weighted = "RolePlayingGameData.WeightedContentEntry";
        const std::string monster = "RolePlayingGameData.Monster";
        ContentTypeReaderManager::AddTypeCreator(
            GenericReader(weighted, monster), [weighted] {
                return std::make_unique<WeightedContentEntryReader<Monster>>(weighted);
            });
        ContentTypeReaderManager::AddTypeCreator(
            ListReaderName(GenericType(weighted, monster)), [weighted, monster] {
                return std::make_unique<ListReader<std::shared_ptr<WeightedContentEntry<Monster>>>>(
                    ListTypeName(GenericType(weighted, monster)),
                    GenericReader(weighted, monster));
            });
    }

    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.Quest+QuestReader", [] { return std::make_unique<QuestReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.QuestLine+QuestLineReader",
        [] { return std::make_unique<QuestLineReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        "RolePlayingGameData.GameStartDescription+GameStartDescriptionReader",
        [] { return std::make_unique<GameStartDescriptionReader>(); });

    // A quest's world entries, which carry a map content name alongside the map position.
    RegisterWorldEntryFamily<Chest>("RolePlayingGameData.Chest");
    RegisterWorldEntryFamily<FixedCombat>("RolePlayingGameData.FixedCombat");

    // Quest requirements. The original picks the content directory by branching on typeof(T);
    // here each instantiation is registered with the directory that branch would have chosen.
    RegisterQuestRequirementFamily<Gear>("RolePlayingGameData.Gear", "Gear");
    RegisterQuestRequirementFamily<Monster>("RolePlayingGameData.Monster",
                                            "Characters/Monsters");

    // The map layers: C# int[], read by ArrayReader<int>.
    ContentTypeReaderManager::AddTypeCreator(
        "Microsoft.Xna.Framework.Content.ArrayReader`1[[System.Int32]]", [] {
            return std::make_unique<ArrayReader<std::int32_t>>(
                "System.Int32[]", "Microsoft.Xna.Framework.Content.Int32Reader");
        });

    // The lists of whole objects a map holds directly.
    ContentTypeReaderManager::AddTypeCreator(
        ListReaderName("RolePlayingGameData.Portal"), [] {
            return std::make_unique<ListReader<std::shared_ptr<Portal>>>(
                ListTypeName("RolePlayingGameData.Portal"),
                "RolePlayingGameData.Portal+PortalReader");
        });
    ContentTypeReaderManager::AddTypeCreator(
        ListReaderName("RolePlayingGameData.StoreCategory"), [] {
            return std::make_unique<ListReader<std::shared_ptr<StoreCategory>>>(
                ListTypeName("RolePlayingGameData.StoreCategory"),
                "RolePlayingGameData.StoreCategory+StoreCategoryReader");
        });

    ContentTypeReaderManager::AddTypeCreator(
        ListReaderName("RolePlayingGameData.CharacterLevelDescription"), [] {
            return std::make_unique<ListReader<std::shared_ptr<CharacterLevelDescription>>>(
                ListTypeName("RolePlayingGameData.CharacterLevelDescription"),
                "RolePlayingGameData.CharacterLevelDescription+CharacterLevelDescriptionReader");
        });
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
