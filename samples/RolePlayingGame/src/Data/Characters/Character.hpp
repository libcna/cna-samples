#pragma once

// Character.hpp -- C++ port of RolePlayingGameData/Characters/Character.cs.

#include <optional>
#include <vector>
#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"

#include "../AnimatingSprite.hpp"
#include "../Direction.hpp"
#include "../WorldObject.hpp"

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "System/ArgumentNullException.hpp"
namespace RolePlayingGameData {

// A character in the game world.
class Character : public WorldObject {
public:
    ~Character() override = default;

    enum class CharacterState { Idle, Walking, Defending, Dodging, Hit, Dying, Dead };

    CharacterState State = CharacterState::Idle;

    bool IsDeadOrDying() const {
        return State == CharacterState::Dying || State == CharacterState::Dead;
    }

    Microsoft::Xna::Framework::Point MapPosition;
    // Renamed from the original's "Direction" property -- clashes with the
    // RolePlayingGameData::Direction enum type name (see missing.md).
    Direction CharacterDirection = Direction::South;

    std::shared_ptr<AnimatingSprite> MapSprite;
    // If null, then the animations are taken from MapSprite.
    std::shared_ptr<AnimatingSprite> WalkingSprite;

    virtual void ResetAnimation(bool isWalking) {
        State = isWalking ? CharacterState::Walking : CharacterState::Idle;
        if (MapSprite) {
            if (isWalking && MapSprite->Find("Walk" + std::string(ToString(CharacterDirection))))
                MapSprite->PlayAnimation("Walk", CharacterDirection);
            else
                MapSprite->PlayAnimation("Idle", CharacterDirection);
        }
        if (WalkingSprite) {
            if (isWalking && WalkingSprite->Find("Walk" + std::string(ToString(CharacterDirection))))
                WalkingSprite->PlayAnimation("Walk", CharacterDirection);
            else
                WalkingSprite->PlayAnimation("Idle", CharacterDirection);
        }
    }

    std::shared_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> ShadowTexture;

    int MapIdleAnimationInterval = 200;
    int MapWalkingAnimationInterval = 80;

    void AddStandardCharacterIdleAnimations() {
        if (!MapSprite) return;
        MapSprite->AddAnimation(std::make_shared<Animation>("IdleSouth", 1, 6, MapIdleAnimationInterval, true));
        MapSprite->AddAnimation(std::make_shared<Animation>("IdleSouthwest", 7, 12, MapIdleAnimationInterval, true));
        MapSprite->AddAnimation(std::make_shared<Animation>("IdleWest", 13, 18, MapIdleAnimationInterval, true));
        MapSprite->AddAnimation(std::make_shared<Animation>("IdleNorthwest", 19, 24, MapIdleAnimationInterval, true));
        MapSprite->AddAnimation(std::make_shared<Animation>("IdleNorth", 25, 30, MapIdleAnimationInterval, true));
        MapSprite->AddAnimation(std::make_shared<Animation>("IdleNortheast", 31, 36, MapIdleAnimationInterval, true));
        MapSprite->AddAnimation(std::make_shared<Animation>("IdleEast", 37, 42, MapIdleAnimationInterval, true));
        MapSprite->AddAnimation(std::make_shared<Animation>("IdleSoutheast", 43, 48, MapIdleAnimationInterval, true));
    }

    void AddStandardCharacterWalkingAnimations() {
        AnimatingSprite* sprite = WalkingSprite ? WalkingSprite.get() : MapSprite.get();
        if (!sprite) return;
        sprite->AddAnimation(std::make_shared<Animation>("WalkSouth", 1, 6, MapWalkingAnimationInterval, true));
        sprite->AddAnimation(std::make_shared<Animation>("WalkSouthwest", 7, 12, MapWalkingAnimationInterval, true));
        sprite->AddAnimation(std::make_shared<Animation>("WalkWest", 13, 18, MapWalkingAnimationInterval, true));
        sprite->AddAnimation(std::make_shared<Animation>("WalkNorthwest", 19, 24, MapWalkingAnimationInterval, true));
        sprite->AddAnimation(std::make_shared<Animation>("WalkNorth", 25, 30, MapWalkingAnimationInterval, true));
        sprite->AddAnimation(std::make_shared<Animation>("WalkNortheast", 31, 36, MapWalkingAnimationInterval, true));
        sprite->AddAnimation(std::make_shared<Animation>("WalkEast", 37, 42, MapWalkingAnimationInterval, true));
        sprite->AddAnimation(std::make_shared<Animation>("WalkSoutheast", 43, 48, MapWalkingAnimationInterval, true));
    }
};

// Reads a Character object from the content pipeline.
//
// Character is abstract in the original, so the concrete reader owns the instance and passes it
// down; this reader never creates one.
class CharacterReader
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Character>> {
public:
    explicit CharacterReader(const std::string& typeName = "RolePlayingGameData.Character")
        : Microsoft::Xna::Framework::Content::ContentTypeReader<std::shared_ptr<Character>>(
              typeName) {}

protected:
    std::shared_ptr<Character> Read(
        Microsoft::Xna::Framework::Content::ContentReader& input,
        std::optional<std::shared_ptr<Character>> existingInstance) override {
        std::shared_ptr<Character> character =
            existingInstance.has_value() ? *existingInstance : nullptr;
        if (character == nullptr) {
            throw System::ArgumentNullException("existingInstance");
        }

        WorldObjectReader worldObjectReader;
        input.ReadRawObject<std::shared_ptr<WorldObject>>(
            worldObjectReader, std::static_pointer_cast<WorldObject>(character));

        character->MapIdleAnimationInterval = static_cast<int>(input.ReadInt32());
        character->MapSprite = input.ReadObject<std::shared_ptr<AnimatingSprite>>();
        if (character->MapSprite != nullptr) {
            character->MapSprite->SourceOffset = Microsoft::Xna::Framework::Vector2(
                character->MapSprite->SourceOffset.X - 32.0f,
                character->MapSprite->SourceOffset.Y - 32.0f);
        }
        character->AddStandardCharacterIdleAnimations();

        character->MapWalkingAnimationInterval = static_cast<int>(input.ReadInt32());
        character->WalkingSprite = input.ReadObject<std::shared_ptr<AnimatingSprite>>();
        if (character->WalkingSprite != nullptr) {
            character->WalkingSprite->SourceOffset = Microsoft::Xna::Framework::Vector2(
                character->WalkingSprite->SourceOffset.X - 32.0f,
                character->WalkingSprite->SourceOffset.Y - 32.0f);
        }
        character->AddStandardCharacterWalkingAnimations();

        character->ResetAnimation(false);

        character->ShadowTexture = std::make_shared<Microsoft::Xna::Framework::Graphics::Texture2D>(
            input.getContentManagerProperty()
                ->Load<Microsoft::Xna::Framework::Graphics::Texture2D>(
                    "Textures/Characters/CharacterShadow"));

        return character;
    }
};

} // namespace RolePlayingGameData
