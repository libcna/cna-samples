#pragma once

// Weapon.hpp -- C++ port of RolePlayingGameData/Gear/Weapon.cs.

#include <optional>
#include <memory>
#include <string>

#include "../AnimatingSprite.hpp"
#include "../Int32Range.hpp"
#include "Equipment.hpp"

#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

// Equipment that can be equipped on a FightingCharacter to improve their physical damage.
class Weapon : public Equipment {
public:
    ~Weapon() override = default;

    // Damage range values are positive, and will be subtracted.
    Int32Range TargetDamageRange;

    std::string SwingCueName;
    std::string HitCueName;
    std::string BlockCueName;

    std::shared_ptr<AnimatingSprite> Overlay;

    std::string GetPowerText() const override {
        return "Weapon Attack: " + TargetDamageRange.ToString();
    }
};

// Reads a Weapon object from the content pipeline.
// Targets shared_ptr<Gear>, the base the game loads by; see EquipmentReader.
class WeaponReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Gear>> {
public:
    WeaponReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Gear>>(
              "RolePlayingGameData.Weapon") {}

protected:
    std::shared_ptr<Gear> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Gear>> existingInstance) override {
        std::shared_ptr<Weapon> weapon =
            existingInstance.has_value()
                ? std::static_pointer_cast<Weapon>(*existingInstance)
                : nullptr;
        if (weapon == nullptr) {
            weapon = std::make_shared<Weapon>();
        }

        // read the gear settings
        EquipmentReader equipmentReader;
        input.ReadRawObject<std::shared_ptr<Gear>>(
            equipmentReader, std::static_pointer_cast<Gear>(weapon));

        // read the weapon settings
        weapon->TargetDamageRange = input.ReadObject<Int32Range>();
        weapon->SwingCueName = input.ReadString();
        weapon->HitCueName = input.ReadString();
        weapon->BlockCueName = input.ReadString();
        weapon->Overlay = input.ReadObject<std::shared_ptr<AnimatingSprite>>();
        weapon->Overlay->SourceOffset = Microsoft::Xna::Framework::Vector2(
            static_cast<float>(weapon->Overlay->FrameDimensions().X / 2),
            static_cast<float>(weapon->Overlay->FrameDimensions().Y));

        return weapon;
    }
};

} // namespace RolePlayingGameData
