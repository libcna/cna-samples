#pragma once

// Store.hpp -- C++ port of RolePlayingGameData/Map/Store.cs.

#include <optional>
#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "../WorldObject.hpp"
#include "StoreCategory.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

// A gear store, where the party can buy and sell gear, organized into categories.
class Store : public WorldObject {
public:
    float BuyMultiplier = 1.0f;
    float SellMultiplier = 1.0f;
    std::vector<StoreCategory> StoreCategories;

    std::string WelcomeMessage;

    std::string ShopkeeperTextureName;
    std::shared_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> ShopkeeperTexture;
};

// Reads a Store object from the content pipeline.
class StoreReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Store>> {
public:
    StoreReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Store>>(
              "RolePlayingGameData.Store") {}

protected:
    std::shared_ptr<Store> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Store>> existingInstance) override {
        std::shared_ptr<Store> store = existingInstance.has_value() ? *existingInstance : nullptr;
        if (store == nullptr) {
            store = std::make_shared<Store>();
        }

        WorldObjectReader worldObjectReader;
        input.ReadRawObject<std::shared_ptr<WorldObject>>(
            worldObjectReader, std::static_pointer_cast<WorldObject>(store));

        store->BuyMultiplier = input.ReadSingle();
        store->SellMultiplier = input.ReadSingle();
        // The .xnb holds a list of references, as C# does; this port stores the categories by
        // value, so each is copied out of the reference the protocol produces.
        const auto storeCategories =
            input.ReadObject<std::vector<std::shared_ptr<StoreCategory>>>();
        for (const auto& category : storeCategories) {
            store->StoreCategories.push_back(*category);
        }
        store->WelcomeMessage = input.ReadString();
        store->ShopkeeperTextureName = input.ReadString();
        store->ShopkeeperTexture = std::make_shared<Microsoft::Xna::Framework::Graphics::Texture2D>(
            input.getContentManagerProperty()
                ->Load<Microsoft::Xna::Framework::Graphics::Texture2D>(
                    "Textures/Characters/Portraits/" + store->ShopkeeperTextureName));

        return store;
    }
};

} // namespace RolePlayingGameData
