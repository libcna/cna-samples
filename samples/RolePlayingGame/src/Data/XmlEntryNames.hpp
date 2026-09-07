#pragma once

// XmlEntryNames.hpp -- CNAEXT.
//
// The save file names a generic instantiation the way .NET does: List<WorldEntry<Chest>> becomes
// <ArrayOfWorldEntryOfChest>, whose items are <WorldEntryOfChest>. C++ cannot derive those names
// from a template, so each instantiation the save touches states its own -- the same thing the
// original gets from typeof(T).Name at run time. Measured against the XNA 4.0 runtime; see
// diff.md.

namespace RolePlayingGameData {

template <typename T>
struct XmlEntryNames;

#define RPG_XML_ENTRY_NAMES(TypeName)                                          \
    class TypeName;                                                            \
    template <>                                                                \
    struct XmlEntryNames<TypeName> {                                           \
        static constexpr const char* ContentEntry = "ContentEntryOf" #TypeName; \
        static constexpr const char* MapEntry = "MapEntryOf" #TypeName;         \
        static constexpr const char* WorldEntry = "WorldEntryOf" #TypeName;     \
        static constexpr const char* WeightedContentEntry =                     \
            "WeightedContentEntryOf" #TypeName;                                 \
        static constexpr const char* QuestRequirement = "QuestRequirementOf" #TypeName; \
    };

RPG_XML_ENTRY_NAMES(Gear)
RPG_XML_ENTRY_NAMES(Chest)
RPG_XML_ENTRY_NAMES(FixedCombat)
RPG_XML_ENTRY_NAMES(Player)
RPG_XML_ENTRY_NAMES(Monster)
RPG_XML_ENTRY_NAMES(Portal)
RPG_XML_ENTRY_NAMES(Inn)
RPG_XML_ENTRY_NAMES(Store)
RPG_XML_ENTRY_NAMES(QuestNpc)

#undef RPG_XML_ENTRY_NAMES

} // namespace RolePlayingGameData
