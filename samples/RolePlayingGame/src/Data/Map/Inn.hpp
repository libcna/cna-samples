#pragma once

// Inn.hpp -- C++ port of RolePlayingGameData/Map/Inn.cs.

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "../WorldObject.hpp"

namespace RolePlayingGameData {

// An inn in the game world, where the party can rest and restore themselves.
class Inn : public WorldObject {
public:
    // The amount of money that the innkeeper charges per member of the party.
    int ChargePerPlayer = 0;

    // The message shown when the player enters the inn.
    std::string WelcomeMessage;

    // The message shown when the player pays for the inn.
    std::string PaidMessage;

    // The message shown when the player cannot pay for the inn.
    std::string NotEnoughGoldMessage;

    // The content name of the texture for the shopkeeper.
    std::string ShopkeeperTextureName;

    // The texture for the shopkeeper. [ContentSerializerIgnore] on the original property; it is
    // loaded by the reader below rather than written into the .xnb.
    std::shared_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> ShopkeeperTexture;
};

// Reads an Inn object from the content pipeline.
class InnReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Inn>> {
public:
    InnReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Inn>>(
              "RolePlayingGameData.Inn") {}

protected:
    std::shared_ptr<Inn> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Inn>> existingInstance) override {
        std::shared_ptr<Inn> inn =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (inn == nullptr) {
            inn = std::make_shared<Inn>();
        }

        // The original calls input.ReadRawObject<WorldObject>(inn as WorldObject), which XNA
        // resolves to the WorldObject reader by reflecting over the type. C++ has no reflection,
        // so the reader is named explicitly; ReadRawObject consumes no dispatch index either way,
        // and the base part is read into this very instance exactly as the original reads it.
        WorldObjectReader worldObjectReader;
        input.ReadRawObject<std::shared_ptr<WorldObject>>(
            worldObjectReader, std::static_pointer_cast<WorldObject>(inn));

        inn->ChargePerPlayer = static_cast<int>(input.ReadInt32());
        inn->WelcomeMessage = input.ReadString();
        inn->PaidMessage = input.ReadString();
        inn->NotEnoughGoldMessage = input.ReadString();
        inn->ShopkeeperTextureName = input.ReadString();
        inn->ShopkeeperTexture =
            std::make_shared<Microsoft::Xna::Framework::Graphics::Texture2D>(
                input.getContentManagerProperty()
                    ->Load<Microsoft::Xna::Framework::Graphics::Texture2D>(
                        "Textures/Characters/Portraits/" + inn->ShopkeeperTextureName));

        return inn;
    }
};

} // namespace RolePlayingGameData
