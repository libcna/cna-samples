#pragma once

// StoreCategory.hpp -- C++ port of RolePlayingGameData/Map/StoreCategory.cs.

#include <optional>
#include <memory>
#include <string>
#include <vector>

#include "../Gear/Gear.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

// A category of gear for sale in a store.
class StoreCategory {
public:
    std::string Name;
    std::vector<std::string> AvailableContentNames;
    std::vector<std::shared_ptr<Gear>> AvailableGear;
};

// Reads a StoreCategory object from the content pipeline.
class StoreCategoryReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<
          std::shared_ptr<StoreCategory>> {
public:
    StoreCategoryReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<StoreCategory>>(
              "RolePlayingGameData.StoreCategory") {}

protected:
    std::shared_ptr<StoreCategory> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<StoreCategory>> existingInstance) override {
        std::shared_ptr<StoreCategory> storeCategory =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (storeCategory == nullptr) {
            storeCategory = std::make_shared<StoreCategory>();
        }

        storeCategory->Name = input.ReadString();
        const auto availableContentNames = input.ReadObject<std::vector<std::string>>();
        storeCategory->AvailableContentNames.insert(
            storeCategory->AvailableContentNames.end(),
            availableContentNames.begin(), availableContentNames.end());

        // populate the gear list based on the content names
        for (const std::string& gearName : storeCategory->AvailableContentNames) {
            storeCategory->AvailableGear.push_back(
                input.getContentManagerProperty()->Load<std::shared_ptr<Gear>>(
                    "Gear/" + gearName));
        }

        return storeCategory;
    }
};

} // namespace RolePlayingGameData
