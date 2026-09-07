#pragma once

// Map.hpp -- C++ port of RolePlayingGameData/Map/Map.cs.

#include <optional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"

#include "../Characters/Player.hpp"
#include "../Characters/QuestNpc.hpp"
#include "../ContentObject.hpp"
#include "../MapEntry.hpp"
#include "Chest.hpp"
#include "FixedCombat.hpp"
#include "Inn.hpp"
#include "Portal.hpp"
#include "RandomCombat.hpp"
#include "Store.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "System/Random.hpp"
namespace RolePlayingGameData {

using Microsoft::Xna::Framework::Point;
using Microsoft::Xna::Framework::Rectangle;

// One section of the world, and all of the data in it.
class Map : public ContentObject {
public:
    std::string Name;

    Point MapDimensions;
    Point TileSize;
    int TilesPerRow = 0;

    // A valid spawn position for this map.
    Point SpawnMapPosition;

    std::string TextureName;
    std::shared_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> Texture;
    std::string CombatTextureName;
    std::shared_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> CombatTexture;

    std::string MusicCueName;
    std::string CombatMusicCueName;

    std::vector<int> BaseLayer;
    std::vector<int> FringeLayer;
    std::vector<int> ObjectLayer;
    std::vector<int> CollisionLayer;

    int GetBaseLayerValue(Point mapPosition) const { return LayerValue(BaseLayer, mapPosition); }
    int GetFringeLayerValue(Point mapPosition) const { return LayerValue(FringeLayer, mapPosition); }
    int GetObjectLayerValue(Point mapPosition) const { return LayerValue(ObjectLayer, mapPosition); }
    int GetCollisionLayerValue(Point mapPosition) const { return LayerValue(CollisionLayer, mapPosition); }

    Rectangle GetBaseLayerSourceRectangle(Point mapPosition) const { return LayerSourceRectangle(BaseLayer, mapPosition); }
    Rectangle GetFringeLayerSourceRectangle(Point mapPosition) const { return LayerSourceRectangle(FringeLayer, mapPosition); }
    Rectangle GetObjectLayerSourceRectangle(Point mapPosition) const { return LayerSourceRectangle(ObjectLayer, mapPosition); }

    bool IsBlocked(Point mapPosition) const {
        if (mapPosition.X < 0 || mapPosition.X >= MapDimensions.X || mapPosition.Y < 0 ||
            mapPosition.Y >= MapDimensions.Y)
            return true;
        return GetCollisionLayerValue(mapPosition) != 0;
    }

    std::vector<std::shared_ptr<Portal>> Portals;
    std::vector<std::shared_ptr<MapEntry<Portal>>> PortalEntries;

    std::shared_ptr<MapEntry<Portal>> FindPortal(const std::string& name) const {
        for (auto& entry : PortalEntries) {
            if (entry->ContentName == name) return entry;
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<MapEntry<Chest>>> ChestEntries;
    std::vector<std::shared_ptr<MapEntry<FixedCombat>>> FixedCombatEntries;
    std::shared_ptr<RandomCombat> RandomCombatData;
    std::vector<std::shared_ptr<MapEntry<QuestNpc>>> QuestNpcEntries;
    std::vector<std::shared_ptr<MapEntry<Player>>> PlayerNpcEntries;
    std::vector<std::shared_ptr<MapEntry<Inn>>> InnEntries;
    std::vector<std::shared_ptr<MapEntry<Store>>> StoreEntries;

    std::shared_ptr<Map> Clone() const {
        auto map = std::make_shared<Map>();
        map->SetAssetName(AssetName());
        map->BaseLayer = BaseLayer;
        for (auto& chestEntry : ChestEntries) {
            auto mapEntry = std::make_shared<MapEntry<Chest>>();
            mapEntry->Content = chestEntry->Content ? chestEntry->Content->Clone() : nullptr;
            mapEntry->ContentName = chestEntry->ContentName;
            mapEntry->Count = chestEntry->Count;
            mapEntry->EntryDirection = chestEntry->EntryDirection;
            mapEntry->MapPosition = chestEntry->MapPosition;
            map->ChestEntries.push_back(mapEntry);
        }
        map->CollisionLayer = CollisionLayer;
        map->CombatMusicCueName = CombatMusicCueName;
        map->CombatTexture = CombatTexture;
        map->CombatTextureName = CombatTextureName;
        map->FixedCombatEntries = FixedCombatEntries;
        map->FringeLayer = FringeLayer;
        map->InnEntries = InnEntries;
        map->MapDimensions = MapDimensions;
        map->MusicCueName = MusicCueName;
        map->Name = Name;
        map->ObjectLayer = ObjectLayer;
        map->PlayerNpcEntries = PlayerNpcEntries;
        map->Portals = Portals;
        map->PortalEntries = PortalEntries;
        map->QuestNpcEntries = QuestNpcEntries;
        map->RandomCombatData = std::make_shared<RandomCombat>();
        if (RandomCombatData) {
            map->RandomCombatData->CombatProbability = RandomCombatData->CombatProbability;
            map->RandomCombatData->Entries = RandomCombatData->Entries;
            map->RandomCombatData->FleeProbability = RandomCombatData->FleeProbability;
            map->RandomCombatData->MonsterCountRange = RandomCombatData->MonsterCountRange;
        }
        map->SpawnMapPosition = SpawnMapPosition;
        map->StoreEntries = StoreEntries;
        map->Texture = Texture;
        map->TextureName = TextureName;
        map->TileSize = TileSize;
        map->TilesPerRow = TilesPerRow;
        return map;
    }

private:
    int LayerValue(const std::vector<int>& layer, Point mapPosition) const {
        if (mapPosition.X < 0 || mapPosition.X >= MapDimensions.X || mapPosition.Y < 0 ||
            mapPosition.Y >= MapDimensions.Y)
            throw std::out_of_range("mapPosition");
        return layer[mapPosition.Y * MapDimensions.X + mapPosition.X];
    }

    Rectangle LayerSourceRectangle(const std::vector<int>& layer, Point mapPosition) const {
        if (mapPosition.X < 0 || mapPosition.X >= MapDimensions.X || mapPosition.Y < 0 ||
            mapPosition.Y >= MapDimensions.Y)
            return Rectangle::Empty;
        int value = LayerValue(layer, mapPosition);
        if (value < 0) return Rectangle::Empty;
        return Rectangle((value % TilesPerRow) * TileSize.X, (value / TilesPerRow) * TileSize.Y, TileSize.X,
                          TileSize.Y);
    }
};

// Reads a Map object from the content pipeline.
class MapReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Map>> {
public:
    MapReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Map>>(
              "RolePlayingGameData.Map") {}

protected:
    std::shared_ptr<Map> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Map>> existingInstance) override {
        std::shared_ptr<Map> map = existingInstance.has_value() ? *existingInstance : nullptr;
        if (map == nullptr) {
            map = std::make_shared<Map>();
        }
        auto* content = input.getContentManagerProperty();

        map->SetAssetName(input.getAssetNameProperty());
        map->Name = input.ReadString();
        map->MapDimensions = input.ReadObject<Point>();
        map->TileSize = input.ReadObject<Point>();
        map->SpawnMapPosition = input.ReadObject<Point>();

        map->TextureName = input.ReadString();
        map->Texture = std::make_shared<Microsoft::Xna::Framework::Graphics::Texture2D>(
            content->Load<Microsoft::Xna::Framework::Graphics::Texture2D>(
                "Textures/Maps/NonCombat/" + map->TextureName));
        map->TilesPerRow = map->Texture->getWidthProperty() / map->TileSize.X;
        map->CombatTextureName = input.ReadString();
        map->CombatTexture = std::make_shared<Microsoft::Xna::Framework::Graphics::Texture2D>(
            content->Load<Microsoft::Xna::Framework::Graphics::Texture2D>(
                "Textures/Maps/Combat/" + map->CombatTextureName));

        map->MusicCueName = input.ReadString();
        map->CombatMusicCueName = input.ReadString();

        map->BaseLayer = input.ReadObject<std::vector<int>>();
        map->FringeLayer = input.ReadObject<std::vector<int>>();
        map->ObjectLayer = input.ReadObject<std::vector<int>>();
        map->CollisionLayer = input.ReadObject<std::vector<int>>();

        const auto portals = input.ReadObject<std::vector<std::shared_ptr<Portal>>>();
        map->Portals.insert(map->Portals.end(), portals.begin(), portals.end());
        const auto portalEntries =
            input.ReadObject<std::vector<std::shared_ptr<MapEntry<Portal>>>>();
        map->PortalEntries.insert(
            map->PortalEntries.end(), portalEntries.begin(), portalEntries.end());
        for (const auto& portalEntry : map->PortalEntries) {
            for (const auto& portal : map->Portals) {
                if (portal->Name == portalEntry->ContentName) {
                    portalEntry->Content = portal;
                    break;
                }
            }
        }

        const auto chestEntries =
            input.ReadObject<std::vector<std::shared_ptr<MapEntry<Chest>>>>();
        map->ChestEntries.insert(
            map->ChestEntries.end(), chestEntries.begin(), chestEntries.end());
        for (const auto& chestEntry : map->ChestEntries) {
            chestEntry->Content =
                content->Load<std::shared_ptr<Chest>>("Maps/Chests/" + chestEntry->ContentName)
                    ->Clone();
        }

        // load the fixed combat entries
        System::Random random;
        const auto fixedCombatEntries =
            input.ReadObject<std::vector<std::shared_ptr<MapEntry<FixedCombat>>>>();
        map->FixedCombatEntries.insert(
            map->FixedCombatEntries.end(), fixedCombatEntries.begin(), fixedCombatEntries.end());
        for (const auto& fixedCombatEntry : map->FixedCombatEntries) {
            fixedCombatEntry->Content = content->Load<std::shared_ptr<FixedCombat>>(
                "Maps/FixedCombats/" + fixedCombatEntry->ContentName);
            // clone the map sprite in the entry, as there may be many entries per FixedCombat
            fixedCombatEntry->MapSprite =
                fixedCombatEntry->Content->Entries[0]->Content->MapSprite->Clone();
            // play the idle animation
            fixedCombatEntry->MapSprite->PlayAnimation("Idle", fixedCombatEntry->EntryDirection);
            // advance in a random amount so the animations aren't synchronized
            fixedCombatEntry->MapSprite->UpdateAnimation(
                4.0f * static_cast<float>(random.NextDouble()));
        }

        map->RandomCombatData = input.ReadObject<std::shared_ptr<RandomCombat>>();

        const auto questNpcEntries =
            input.ReadObject<std::vector<std::shared_ptr<MapEntry<QuestNpc>>>>();
        map->QuestNpcEntries.insert(
            map->QuestNpcEntries.end(), questNpcEntries.begin(), questNpcEntries.end());
        for (const auto& questNpcEntry : map->QuestNpcEntries) {
            questNpcEntry->Content = content->Load<std::shared_ptr<QuestNpc>>(
                "Characters/QuestNpcs/" + questNpcEntry->ContentName);
            questNpcEntry->Content->MapPosition = questNpcEntry->MapPosition;
            questNpcEntry->Content->CharacterDirection = questNpcEntry->EntryDirection;
        }

        const auto playerNpcEntries =
            input.ReadObject<std::vector<std::shared_ptr<MapEntry<Player>>>>();
        map->PlayerNpcEntries.insert(
            map->PlayerNpcEntries.end(), playerNpcEntries.begin(), playerNpcEntries.end());
        for (const auto& playerNpcEntry : map->PlayerNpcEntries) {
            playerNpcEntry->Content =
                content->Load<std::shared_ptr<Player>>(
                           "Characters/Players/" + playerNpcEntry->ContentName)
                    ->Clone();
            playerNpcEntry->Content->MapPosition = playerNpcEntry->MapPosition;
            playerNpcEntry->Content->CharacterDirection = playerNpcEntry->EntryDirection;
        }

        const auto innEntries = input.ReadObject<std::vector<std::shared_ptr<MapEntry<Inn>>>>();
        map->InnEntries.insert(map->InnEntries.end(), innEntries.begin(), innEntries.end());
        for (const auto& innEntry : map->InnEntries) {
            innEntry->Content =
                content->Load<std::shared_ptr<Inn>>("Maps/Inns/" + innEntry->ContentName);
        }

        const auto storeEntries =
            input.ReadObject<std::vector<std::shared_ptr<MapEntry<Store>>>>();
        map->StoreEntries.insert(
            map->StoreEntries.end(), storeEntries.begin(), storeEntries.end());
        for (const auto& storeEntry : map->StoreEntries) {
            storeEntry->Content =
                content->Load<std::shared_ptr<Store>>("Maps/Stores/" + storeEntry->ContentName);
        }

        return map;
    }
};

} // namespace RolePlayingGameData
