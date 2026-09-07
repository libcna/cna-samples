#pragma once

// FixedCombat.hpp -- C++ port of RolePlayingGameData/Map/FixedCombat.cs.

#include <optional>
#include <string>
#include <memory>
#include <vector>

#include "../ContentEntry.hpp"
#include "../Characters/Monster.hpp"
#include "../WorldObject.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

// The description of a fixed combat encounter in the world.
class FixedCombat : public WorldObject {
public:
    std::vector<std::shared_ptr<ContentEntry<Monster>>> Entries;
};

// Reads a FixedCombat object from the content pipeline.
class FixedCombatReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<FixedCombat>> {
public:
    FixedCombatReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<FixedCombat>>(
              "RolePlayingGameData.FixedCombat") {}

protected:
    std::shared_ptr<FixedCombat> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<FixedCombat>> existingInstance) override {
        std::shared_ptr<FixedCombat> fixedCombat =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (fixedCombat == nullptr) {
            fixedCombat = std::make_shared<FixedCombat>();
        }

        WorldObjectReader worldObjectReader;
        input.ReadRawObject<std::shared_ptr<WorldObject>>(
            worldObjectReader, std::static_pointer_cast<WorldObject>(fixedCombat));

        const auto entries =
            input.ReadObject<std::vector<std::shared_ptr<ContentEntry<Monster>>>>();
        fixedCombat->Entries.insert(fixedCombat->Entries.end(), entries.begin(), entries.end());
        for (const auto& fixedCombatEntry : fixedCombat->Entries) {
            fixedCombatEntry->Content =
                input.getContentManagerProperty()->Load<std::shared_ptr<Monster>>(
                    "Characters/Monsters/" + fixedCombatEntry->ContentName);
        }

        return fixedCombat;
    }
};

} // namespace RolePlayingGameData
