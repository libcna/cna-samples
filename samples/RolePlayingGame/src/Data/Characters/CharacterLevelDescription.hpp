#pragma once

// CharacterLevelDescription.hpp -- C++ port of
// RolePlayingGameData/Characters/CharacterLevelDescription.cs.

#include <optional>
#include <memory>
#include <string>
#include <vector>

#include "../Spell.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

class CharacterLevelDescription {
public:
    int ExperiencePoints = 0;
    std::vector<std::string> SpellContentNames;
    std::vector<std::shared_ptr<Spell>> Spells;
};

// Reads a CharacterLevelDescription object from the content pipeline.
class CharacterLevelDescriptionReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<
          std::shared_ptr<CharacterLevelDescription>> {
public:
    CharacterLevelDescriptionReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<
              std::shared_ptr<CharacterLevelDescription>>(
              "RolePlayingGameData.CharacterLevelDescription") {}

protected:
    std::shared_ptr<CharacterLevelDescription> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<CharacterLevelDescription>> existingInstance) override {
        std::shared_ptr<CharacterLevelDescription> desc =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (desc == nullptr) {
            desc = std::make_shared<CharacterLevelDescription>();
        }

        desc->ExperiencePoints = static_cast<int>(input.ReadInt32());
        const auto spellContentNames = input.ReadObject<std::vector<std::string>>();
        desc->SpellContentNames.insert(
            desc->SpellContentNames.end(), spellContentNames.begin(), spellContentNames.end());

        // load all of the spells immediately
        for (const std::string& spellContentName : desc->SpellContentNames) {
            desc->Spells.push_back(
                input.getContentManagerProperty()->Load<std::shared_ptr<Spell>>(
                    "Spells/" + spellContentName));
        }

        return desc;
    }
};

} // namespace RolePlayingGameData
