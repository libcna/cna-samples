#pragma once

// ItemCombatAction.hpp -- C++ port of Combat/Actions/ItemCombatAction.cs.

#include <memory>

#include "Microsoft/Xna/Framework/Audio/AudioStopOptions.hpp"
#include "Microsoft/Xna/Framework/Audio/Cue.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/ArgumentException.hpp"
#include "System/ArgumentNullException.hpp"

#include "../../AudioManager.hpp"
#include "../../Data/Gear/Item.hpp"
#include "../../Data/Int32Range.hpp"
#include "../../Session/Session.hpp"
#include "CombatAction.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Audio::AudioStopOptions;
using Microsoft::Xna::Framework::Audio::Cue;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using RolePlayingGameData::Int32Range;
using RolePlayingGameData::Item;

// An item-using combat action, including related data and calculations.
class ItemCombatAction : public CombatAction {
public:
    // Constructs a new ItemCombatAction object.
    ItemCombatAction(Combatant& combatant, const std::shared_ptr<Item>& item)
        : CombatAction(combatant), item_(item) {
        // check the parameter
        if (item == nullptr) {
            throw System::ArgumentNullException("item");
        }
        if ((item->Usage & Item::ItemUsage::Combat) == 0) {
            throw System::ArgumentException("Combat items must have Combat usage.");
        }

        adjacentTargets_ = item_->AdjacentTargets;
    }

    // Returns true if the action is offensive, targeting the opponents.
    bool IsOffensive() const override { return item_->IsOffensive; }

    // Returns true if the character can use this action.
    bool IsCharacterValidUser() const override { return true; }

    // Returns true if this action requires a target.
    bool IsTargetNeeded() const override { return true; }

    // The item used in this action.
    const std::shared_ptr<Item>& GetItem() const { return item_; }

    // The heuristic used to compare actions of this type to similar ones.
    int Heuristic() const override {
        return GetItem()->TargetEffectRange.HealthPointsRange.Average();
    }

    // Start executing the combat action.
    void Start() override {
        // play the creation sound effect
        AudioManager::PlayCue(item_->UsingCueName);

        CombatAction::Start();
    }

    // Updates the action over time.
    void Update(const GameTime& gameTime) override {
        // update the animations
        float elapsedSeconds =
            (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
        item_->SpellSprite->UpdateAnimation(elapsedSeconds);
        if (item_->Overlay != nullptr) {
            item_->Overlay->UpdateAnimation(elapsedSeconds);
            if (!item_->Overlay->IsPlaybackComplete() &&
                Target->GetCombatSprite().IsPlaybackComplete()) {
                item_->Overlay->StopAnimation();
            }
        }

        CombatAction::Update(gameTime);
    }

    // Draw any elements of the action that are independent of the character.
    void Draw(const GameTime& gameTime, SpriteBatch& spriteBatch) override {
        // draw the item projectile
        if (!item_->SpellSprite->IsPlaybackComplete()) {
            if (stage_ == CombatActionStage::Advancing) {
                item_->SpellSprite->Draw(spriteBatch, itemSpritePosition_, 0.0f,
                                         projectileSpriteEffect_);
            } else {
                item_->SpellSprite->Draw(spriteBatch, itemSpritePosition_, 0.0f);
            }
        }

        // draw the item overlay
        if (item_->Overlay != nullptr && !item_->Overlay->IsPlaybackComplete()) {
            item_->Overlay->Draw(spriteBatch, Target->Position(), 0.0f);
        }

        CombatAction::Draw(gameTime, spriteBatch);
    }

    // Calculate the item damage done by the given combatant and item.
    static StatisticsValue CalculateItemDamage(Combatant& combatant,
                                               const std::shared_ptr<Item>& item) {
        (void)combatant;
        // check the parameter
        if (item == nullptr) {
            throw System::ArgumentNullException("item");
        }

        // generate a new effect value - no stats are involved for items
        return item->TargetEffectRange.GenerateValue(Session::GetRandom());
    }

protected:
    // Starts a new combat stage. Called right after the stage changes.
    // Defined out-of-line in CombatEngine.hpp -- the Executing stage walks the engine's
    // secondary-target list.
    void StartStage() override;

    // Update the action for the current stage.
    void UpdateCurrentStage(const GameTime& gameTime) override {
        switch (stage_) {
        case CombatActionStage::Advancing:
            if (projectileDistanceCovered_ < totalProjectileDistance_) {
                projectileDistanceCovered_ +=
                    ProjectileSpeed *
                    (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
            }
            itemSpritePosition_ = combatant_->OriginalPosition() +
                                  projectileDirection_ * projectileDistanceCovered_;
            break;

        default:
            break;
        }
    }

    // Returns true if the combat action is ready to proceed to the next stage.
    bool IsReadyForNextStage() const override {
        switch (stage_) {
        case CombatActionStage::Preparing: // ready to advance?
            return combatant_->GetCombatSprite().IsPlaybackComplete() &&
                   item_->SpellSprite->IsPlaybackComplete();

        case CombatActionStage::Advancing: // ready to execute?
            if (item_->SpellSprite->IsPlaybackComplete() ||
                projectileDistanceCovered_ >= totalProjectileDistance_) {
                projectileDistanceCovered_ = totalProjectileDistance_;
                return true;
            }
            return false;

        case CombatActionStage::Executing: // ready to return?
            return item_->SpellSprite->IsPlaybackComplete();

        default:
            break;
        }

        // fall through to the base behavior
        return CombatAction::IsReadyForNextStage();
    }

    // Apply the action's item to the given target. Returns true if there was any effect.
    bool ApplyItem(Combatant& itemTarget) {
        StatisticsValue effectStatistics = CalculateItemDamage(*combatant_, item_);
        if (item_->IsOffensive) {
            // calculate the defense
            Int32Range defenseRange = itemTarget.GetCharacter().MagicDefenseRange() +
                                      itemTarget.Statistics().MagicalDefense;
            int defense = defenseRange.GenerateValue(Session::GetRandom());
            // subtract the defense
            effectStatistics -=
                StatisticsValue(defense, defense, defense, defense, defense, defense);
            // make sure that this only contains damage
            effectStatistics.ApplyMinimum(StatisticsValue());
            // damage the target
            itemTarget.Damage(effectStatistics, item_->TargetDuration);
        } else {
            // make sure that this only contains healing
            effectStatistics.ApplyMinimum(StatisticsValue());
            // heal the target
            itemTarget.Heal(effectStatistics, item_->TargetDuration);
        }
        return !effectStatistics.IsZero();
    }

    // The item used in this action.
    std::shared_ptr<Item> item_;

    // The current position of the item sprite.
    Vector2 itemSpritePosition_;

    // The speed at which the projectile moves, in units per second.
    static constexpr float ProjectileSpeed = 300.0f;

    // The direction of the projectile.
    Vector2 projectileDirection_;

    // The distance covered so far by the projectile.
    mutable float projectileDistanceCovered_ = 0.0f;

    // The total distance between the original combatant position and the target.
    float totalProjectileDistance_ = 0.0f;

    // The sprite effect on the projectile, if any.
    SpriteEffects projectileSpriteEffect_ = SpriteEffects::None;

    // The sound effect cue for the traveling projectile.
    Cue* projectileCue_ = nullptr;
};

} // namespace RolePlaying
