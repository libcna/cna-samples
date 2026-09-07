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
class EquipmentReader
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Equipment>> {
public:
    explicit EquipmentReader(const std::string& typeName = "RolePlayingGameData.Equipment")
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Equipment>>(
              typeName) {}

protected:
    std::shared_ptr<Equipment> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Equipment>> existingInstance) override {
        std::shared_ptr<Equipment> equipment =
            existingInstance.has_value() ? *existingInstance : nullptr;
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
