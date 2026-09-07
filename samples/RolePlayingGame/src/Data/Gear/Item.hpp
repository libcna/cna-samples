#pragma once

// Item.hpp -- C++ port of RolePlayingGameData/Gear/Item.cs.

#include <optional>
#include <memory>
#include <string>

#include "../AnimatingSprite.hpp"
#include "../StatisticsRange.hpp"
#include "Gear.hpp"

#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

// A usable piece of gear that has a spell-like effect.
class Item : public Gear {
public:
    ~Item() override = default;

    // Flags that specify when an item may be used.
    enum ItemUsage { Combat = 1, NonCombat = 2 };

    // Defaults to "either", with both values.
    int Usage = Combat | NonCombat;

    // If true, the statistics change are used as a debuff (subtracted).
    // Otherwise, the statistics change is used as a buff (added).
    bool IsOffensive = false;

    // If the duration is zero, then the effects last for the rest of the battle.
    int TargetDuration = 0;

    // This is a debuff if IsOffensive is true, otherwise it's a buff.
    StatisticsRange TargetEffectRange;

    int AdjacentTargets = 0;

    std::string UsingCueName;
    std::string TravelingCueName;
    std::string ImpactCueName;
    std::string BlockCueName;

    // Optional. If null, then a Using or Creating animation in SpellSprite is used.
    std::shared_ptr<AnimatingSprite> CreationSprite;
    std::shared_ptr<AnimatingSprite> SpellSprite;
    std::shared_ptr<AnimatingSprite> Overlay;

    std::string GetPowerText() const override { return TargetEffectRange.GetModifierString(); }
};

// Reads an Item object from the content pipeline.
//
// Targets shared_ptr<Gear>, the base the game loads by; see EquipmentReader.
class ItemReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Gear>> {
public:
    ItemReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Gear>>(
              "RolePlayingGameData.Item") {}

protected:
    std::shared_ptr<Gear> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Gear>> existingInstance) override {
        std::shared_ptr<Item> item =
            existingInstance.has_value()
                ? std::static_pointer_cast<Item>(*existingInstance)
                : nullptr;
        if (item == nullptr) {
            item = std::make_shared<Item>();
        }

        // read gear settings
        GearReader gearReader;
        input.ReadRawObject<std::shared_ptr<Gear>>(
            gearReader, std::static_pointer_cast<Gear>(item));

        // read item settings
        item->Usage = static_cast<int>(input.ReadInt32());
        item->IsOffensive = input.ReadBoolean();
        item->TargetDuration = static_cast<int>(input.ReadInt32());
        item->TargetEffectRange = input.ReadObject<StatisticsRange>();
        item->AdjacentTargets = static_cast<int>(input.ReadInt32());
        item->UsingCueName = input.ReadString();
        item->TravelingCueName = input.ReadString();
        item->ImpactCueName = input.ReadString();
        item->BlockCueName = input.ReadString();

        // Each of the three sprites is re-anchored to the bottom centre of its frame, exactly as
        // the original does after reading it.
        const auto anchorToBottomCentre = [](const std::shared_ptr<AnimatingSprite>& sprite) {
            sprite->SourceOffset = Microsoft::Xna::Framework::Vector2(
                static_cast<float>(sprite->FrameDimensions().X / 2),
                static_cast<float>(sprite->FrameDimensions().Y));
        };
        item->CreationSprite = input.ReadObject<std::shared_ptr<AnimatingSprite>>();
        anchorToBottomCentre(item->CreationSprite);
        item->SpellSprite = input.ReadObject<std::shared_ptr<AnimatingSprite>>();
        anchorToBottomCentre(item->SpellSprite);
        item->Overlay = input.ReadObject<std::shared_ptr<AnimatingSprite>>();
        anchorToBottomCentre(item->Overlay);

        return item;
    }
};

} // namespace RolePlayingGameData
