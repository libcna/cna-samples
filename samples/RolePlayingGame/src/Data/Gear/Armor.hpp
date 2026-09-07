#pragma once

// Armor.hpp -- C++ port of RolePlayingGameData/Gear/Armor.cs.

#include <memory>
#include <optional>
#include "../Int32Range.hpp"
#include "Equipment.hpp"

#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

// Equipment that can be equipped on a FightingCharacter to improve their defense.
class Armor : public Equipment {
public:
    ~Armor() override = default;

    // Only one piece may fill a slot at the same time.
    enum class ArmorSlot { Helmet, Shield, Torso, Boots };

    ArmorSlot Slot = ArmorSlot::Helmet;

    Int32Range OwnerHealthDefenseRange;
    Int32Range OwnerMagicDefenseRange;

    std::string GetPowerText() const override {
        return "Weapon Defense: " + OwnerHealthDefenseRange.ToString() +
               "\nMagic Defense: " + OwnerMagicDefenseRange.ToString();
    }
};

inline Armor::ArmorSlot ArmorSlotFromString(const std::string& s) {
    if (s == "Helmet") return Armor::ArmorSlot::Helmet;
    if (s == "Shield") return Armor::ArmorSlot::Shield;
    if (s == "Torso") return Armor::ArmorSlot::Torso;
    if (s == "Boots") return Armor::ArmorSlot::Boots;
    return Armor::ArmorSlot::Helmet;
}

// Reads an Armor object from the content pipeline.
// Targets shared_ptr<Gear>, the base the game loads by; see EquipmentReader.
class ArmorReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Gear>> {
public:
    ArmorReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Gear>>(
              "RolePlayingGameData.Armor") {}

protected:
    std::shared_ptr<Gear> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Gear>> existingInstance) override {
        std::shared_ptr<Armor> armor =
            existingInstance.has_value()
                ? std::static_pointer_cast<Armor>(*existingInstance)
                : nullptr;
        if (armor == nullptr) {
            armor = std::make_shared<Armor>();
        }

        // read the gear settings
        EquipmentReader equipmentReader;
        input.ReadRawObject<std::shared_ptr<Gear>>(
            equipmentReader, std::static_pointer_cast<Gear>(armor));

        // read armor settings
        armor->Slot = static_cast<Armor::ArmorSlot>(input.ReadInt32());
        armor->OwnerHealthDefenseRange = input.ReadObject<Int32Range>();
        armor->OwnerMagicDefenseRange = input.ReadObject<Int32Range>();

        return armor;
    }
};

} // namespace RolePlayingGameData
