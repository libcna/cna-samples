#pragma once

// Player.hpp -- C++ port of RolePlayingGameData/Characters/Player.cs.

#include <optional>
#include <vector>
#include <memory>
#include <string>

#include "../StatisticsValue.hpp"
#include "FightingCharacter.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
namespace RolePlayingGameData {

// A member of the player's party, also represented in the world before joining.
// There is only one of a given Player in the game world at a time, and their
// current statistics persist after combat -- tracked here.
class Player : public FightingCharacter {
public:
    // The current set of persistent statistics modifiers -- damage, etc.
    StatisticsValue StatisticsModifiers;

    StatisticsValue CurrentStatistics() const { return CharacterStatistics() + StatisticsModifiers; }

    int Gold = 0;

    std::string IntroductionDialogue;
    std::string JoinAcceptedDialogue;
    std::string JoinRejectedDialogue;

    std::string ActivePortraitTextureName;
    std::shared_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> ActivePortraitTexture;
    std::string InactivePortraitTextureName;
    std::shared_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> InactivePortraitTexture;
    std::string UnselectablePortraitTextureName;
    std::shared_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> UnselectablePortraitTexture;

    std::shared_ptr<Player> Clone() const {
        auto player = std::make_shared<Player>();

        player->ActivePortraitTexture = ActivePortraitTexture;
        player->ActivePortraitTextureName = ActivePortraitTextureName;
        player->SetAssetName(AssetName());
        player->SetCharacterClass(GetCharacterClass());
        player->CharacterClassContentName = CharacterClassContentName;
        player->SetCharacterLevel(CharacterLevel());
        player->CombatAnimationInterval = CombatAnimationInterval;
        player->CombatSprite = CombatSprite ? CombatSprite->Clone() : nullptr;
        player->CharacterDirection = CharacterDirection;
        player->EquippedEquipment() = EquippedEquipment();
        player->SetExperience(Experience());
        player->Gold = Gold;
        player->InactivePortraitTexture = InactivePortraitTexture;
        player->InactivePortraitTextureName = InactivePortraitTextureName;
        player->InitialEquipmentContentNames = InitialEquipmentContentNames;
        player->IntroductionDialogue = IntroductionDialogue;
        player->Inventory = Inventory;
        player->JoinAcceptedDialogue = JoinAcceptedDialogue;
        player->JoinRejectedDialogue = JoinRejectedDialogue;
        player->MapIdleAnimationInterval = MapIdleAnimationInterval;
        player->MapPosition = MapPosition;
        player->MapSprite = MapSprite ? MapSprite->Clone() : nullptr;
        player->MapWalkingAnimationInterval = MapWalkingAnimationInterval;
        player->SetName(Name());
        player->ShadowTexture = ShadowTexture;
        player->State = State;
        player->UnselectablePortraitTexture = UnselectablePortraitTexture;
        player->UnselectablePortraitTextureName = UnselectablePortraitTextureName;
        player->WalkingSprite = WalkingSprite ? WalkingSprite->Clone() : nullptr;

        player->RecalculateEquipmentStatistics();
        player->RecalculateTotalDefenseRanges();
        player->RecalculateTotalTargetDamageRange();
        player->ResetAnimation(false);
        player->ResetBaseStatistics();

        return player;
    }
};

// Reads a Player object from the content pipeline.
class PlayerReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Player>> {
public:
    PlayerReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Player>>(
              "RolePlayingGameData.Player") {}

protected:
    std::shared_ptr<Player> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Player>> existingInstance) override {
        std::shared_ptr<Player> player = existingInstance.has_value() ? *existingInstance : nullptr;
        if (player == nullptr) {
            player = std::make_shared<Player>();
        }

        FightingCharacterReader fightingCharacterReader;
        input.ReadRawObject<std::shared_ptr<FightingCharacter>>(
            fightingCharacterReader, std::static_pointer_cast<FightingCharacter>(player));

        player->Gold = static_cast<int>(input.ReadInt32());
        player->IntroductionDialogue = input.ReadString();
        player->JoinAcceptedDialogue = input.ReadString();
        player->JoinRejectedDialogue = input.ReadString();

        const auto loadPortrait = [&input](const std::string& name) {
            return std::make_shared<Microsoft::Xna::Framework::Graphics::Texture2D>(
                input.getContentManagerProperty()
                    ->Load<Microsoft::Xna::Framework::Graphics::Texture2D>(
                        "Textures/Characters/Portraits/" + name));
        };
        player->ActivePortraitTextureName = input.ReadString();
        player->ActivePortraitTexture = loadPortrait(player->ActivePortraitTextureName);
        player->InactivePortraitTextureName = input.ReadString();
        player->InactivePortraitTexture = loadPortrait(player->InactivePortraitTextureName);
        player->UnselectablePortraitTextureName = input.ReadString();
        player->UnselectablePortraitTexture = loadPortrait(player->UnselectablePortraitTextureName);

        return player;
    }
};

} // namespace RolePlayingGameData
