#pragma once

// Chest.hpp -- C++ port of RolePlayingGameData/Map/Chest.cs.

#include <optional>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "../ContentEntry.hpp"
#include "../Gear/Gear.hpp"
#include "../WorldObject.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

// A treasure chest in the game world.
class Chest : public WorldObject {
public:
    int Gold = 0;
    std::vector<std::shared_ptr<ContentEntry<Gear>>> Entries;

    bool IsEmpty() const { return Gold <= 0 && Entries.empty(); }

    std::string TextureName;
    std::shared_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> Texture;

    // The game has to handle chests that have had some contents removed
    // without modifying the original chest (and all chests that come after).
    std::shared_ptr<Chest> Clone() const {
        auto chest = std::make_shared<Chest>();
        chest->Gold = Gold;
        chest->SetName(Name());
        chest->Texture = Texture;
        chest->TextureName = TextureName;
        for (auto& originalEntry : Entries) {
            auto newEntry = std::make_shared<ContentEntry<Gear>>();
            newEntry->Count = originalEntry->Count;
            newEntry->ContentName = originalEntry->ContentName;
            newEntry->Content = originalEntry->Content;
            chest->Entries.push_back(newEntry);
        }
        return chest;
    }
};

// Reads a Chest object from the content pipeline.
class ChestReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Chest>> {
public:
    ChestReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Chest>>(
              "RolePlayingGameData.Chest") {}

protected:
    std::shared_ptr<Chest> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Chest>> existingInstance) override {
        std::shared_ptr<Chest> chest = existingInstance.has_value() ? *existingInstance : nullptr;
        if (chest == nullptr) {
            chest = std::make_shared<Chest>();
        }

        WorldObjectReader worldObjectReader;
        input.ReadRawObject<std::shared_ptr<WorldObject>>(
            worldObjectReader, std::static_pointer_cast<WorldObject>(chest));

        chest->Gold = static_cast<int>(input.ReadInt32());
        const auto entries =
            input.ReadObject<std::vector<std::shared_ptr<ContentEntry<Gear>>>>();
        chest->Entries.insert(chest->Entries.end(), entries.begin(), entries.end());
        for (const auto& contentEntry : chest->Entries) {
            contentEntry->Content =
                input.getContentManagerProperty()->Load<std::shared_ptr<Gear>>(
                    "Gear/" + contentEntry->ContentName);
        }

        chest->TextureName = input.ReadString();
        chest->Texture = std::make_shared<Microsoft::Xna::Framework::Graphics::Texture2D>(
            input.getContentManagerProperty()
                ->Load<Microsoft::Xna::Framework::Graphics::Texture2D>(
                    "Textures/Chests/" + chest->TextureName));

        return chest;
    }
};

} // namespace RolePlayingGameData
