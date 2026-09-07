#pragma once

// Equipment.hpp -- C++ port of RolePlayingGameData/Gear/Equipment.cs.

#include <memory>
#include <optional>
#include <vector>
#include "../StatisticsValue.hpp"
#include "Gear.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "System/ArgumentException.hpp"
namespace RolePlayingGameData {

// Gear that may be equipped onto a FightingCharacter.
class Equipment : public Gear {
public:
    ~Equipment() override = default;

    // Buff values are positive, and will be added.
    StatisticsValue OwnerBuffStatistics;
};

// Reads an Equipment object from the content pipeline.
// Every reader in the gear hierarchy targets shared_ptr<Gear>, the base the game loads by:
// std::any_cast needs an exact type match, unlike C#'s implicit concrete-to-base cast at the
// Load<T> call site. This is the same shape CNA's own five stock-effect readers use for
// ModelMeshPart's polymorphic Effect slot (plans/plan_xnb.md XNB-40).
class EquipmentReader
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Gear>> {
public:
    explicit EquipmentReader(const std::string& typeName = "RolePlayingGameData.Equipment")
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Gear>>(
              typeName) {}

protected:
    std::shared_ptr<Gear> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Gear>> existingInstance) override {
        std::shared_ptr<Equipment> equipment =
            existingInstance.has_value()
                ? std::static_pointer_cast<Equipment>(*existingInstance)
                : nullptr;
        if (equipment == nullptr) {
            throw System::ArgumentException("Unable to create new Equipment objects.");
        }

        // read the gear settings
        GearReader gearReader;
        input.ReadRawObject<std::shared_ptr<Gear>>(
            gearReader, std::static_pointer_cast<Gear>(equipment));

        // read the equipment settings
        equipment->OwnerBuffStatistics = input.ReadObject<StatisticsValue>();

        return equipment;
    }
};

} // namespace RolePlayingGameData
