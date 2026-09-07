#pragma once

// MeleeCombatAction.hpp -- C++ port of Combat/Actions/MeleeCombatAction.cs.

#include <algorithm>
#include <memory>

#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "../../AudioManager.hpp"
#include "../../Data/Gear/Weapon.hpp"
#include "../../Data/Int32Range.hpp"
#include "../../Session/Session.hpp"
#include "CombatAction.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Vector2;
using RolePlayingGameData::Int32Range;
using RolePlayingGameData::Weapon;

// A melee-attack combat action, including related data and calculations.
class MeleeCombatAction : public CombatAction {
public:
    // Constructs a new MeleeCombatAction object.
    explicit MeleeCombatAction(Combatant& combatant) : CombatAction(combatant) {}

    // Returns true if the action is offensive, targeting the opponents.
    bool IsOffensive() const override { return true; }

    // Returns true if this action requires a target.
    bool IsTargetNeeded() const override { return true; }

    // The heuristic used to compare actions of this type to similar ones.
    int Heuristic() const override {
        return combatant_->GetCharacter().TargetDamageRange().Average();
    }

    // Updates the action over time.
    void Update(const GameTime& gameTime) override {
        // update the weapon animation
        float elapsedSeconds =
            (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
        std::shared_ptr<Weapon> weapon = GetCombatant().GetCharacter().GetEquippedWeapon();
        if (weapon != nullptr && weapon->Overlay != nullptr) {
            weapon->Overlay->UpdateAnimation(elapsedSeconds);
        }

        // update the action
        CombatAction::Update(gameTime);
    }

    // Draw any elements of the action that are independent of the character.
    void Draw(const GameTime& gameTime, SpriteBatch& spriteBatch) override {
        // draw the weapon overlay (typically blood)
        std::shared_ptr<Weapon> weapon = GetCombatant().GetCharacter().GetEquippedWeapon();
        if (weapon != nullptr && weapon->Overlay != nullptr &&
            !weapon->Overlay->IsPlaybackComplete()) {
            weapon->Overlay->Draw(spriteBatch, Target->Position(), 0.0f);
        }

        CombatAction::Draw(gameTime, spriteBatch);
    }

protected:
    // Starts a new combat stage. Called right after the stage changes.
    void StartStage() override {
        switch (stage_) {
        case CombatActionStage::Preparing: { // called from Start()
            // play the animation
            combatant_->GetCombatSprite().PlayAnimation("Idle");
        } break;

        case CombatActionStage::Advancing: {
            // play the animation
            combatant_->GetCombatSprite().PlayAnimation("Walk");
            // calculate the advancing destination
            if (Target->Position().X > GetCombatant().Position().X) {
                advanceDirection_ =
                    Target->Position() - GetCombatant().OriginalPosition() - AdvanceOffset();
            } else {
                advanceDirection_ =
                    Target->Position() - GetCombatant().OriginalPosition() + AdvanceOffset();
            }
            totalAdvanceDistance_ = advanceDirection_.Length();
            advanceDirection_.Normalize();
            advanceDistanceCovered_ = 0.0f;
        } break;

        case CombatActionStage::Executing: {
            // play the animation
            combatant_->GetCombatSprite().PlayAnimation("Attack");
            // play the audio
            std::shared_ptr<Weapon> weapon = combatant_->GetCharacter().GetEquippedWeapon();
            if (weapon != nullptr) {
                AudioManager::PlayCue(weapon->SwingCueName);
            } else {
                AudioManager::PlayCue("StaffSwing");
            }
        } break;

        case CombatActionStage::Returning: {
            // play the animation
            combatant_->GetCombatSprite().PlayAnimation("Walk");
            // calculate the damage
            Int32Range damageRange = combatant_->GetCharacter().TargetDamageRange() +
                                     combatant_->Statistics().PhysicalOffense;
            Int32Range defenseRange = Target->GetCharacter().HealthDefenseRange() +
                                      Target->Statistics().PhysicalDefense;
            int damage = std::max(0, damageRange.GenerateValue(Session::GetRandom()) -
                                          defenseRange.GenerateValue(Session::GetRandom()));
            // apply the damage
            if (damage > 0) {
                // play the audio
                std::shared_ptr<Weapon> weapon = combatant_->GetCharacter().GetEquippedWeapon();
                if (weapon != nullptr) {
                    AudioManager::PlayCue(weapon->HitCueName);
                } else {
                    AudioManager::PlayCue("StaffHit");
                }
                // damage the target
                Target->DamageHealth(damage, 0);
                if (weapon != nullptr && weapon->Overlay != nullptr) {
                    weapon->Overlay->PlayAnimation(0);
                    weapon->Overlay->ResetAnimation();
                }
            }
        } break;

        case CombatActionStage::Finishing: {
            // play the animation
            combatant_->GetCombatSprite().PlayAnimation("Idle");
        } break;

        case CombatActionStage::Complete: {
            // play the animation
            combatant_->GetCombatSprite().PlayAnimation("Idle");
        } break;

        default:
            break;
        }
    }

    // Update the action for the current stage.
    void UpdateCurrentStage(const GameTime& gameTime) override {
        float elapsedSeconds =
            (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        switch (stage_) {
        case CombatActionStage::Advancing: {
            // move to the destination
            if (advanceDistanceCovered_ < totalAdvanceDistance_) {
                advanceDistanceCovered_ = std::min(
                    advanceDistanceCovered_ + AdvanceSpeed * elapsedSeconds, totalAdvanceDistance_);
            }
            // update the combatant's position
            combatant_->SetPosition(combatant_->OriginalPosition() +
                                    advanceDirection_ * advanceDistanceCovered_);
        } break;

        case CombatActionStage::Returning: {
            // move to the destination
            if (advanceDistanceCovered_ > 0.0f) {
                advanceDistanceCovered_ -= AdvanceSpeed * elapsedSeconds;
            }
            combatant_->SetPosition(combatant_->OriginalPosition() +
                                    advanceDirection_ * advanceDistanceCovered_);
        } break;

        default:
            break;
        }
    }

    // Returns true if the combat action is ready to proceed to the next stage.
    bool IsReadyForNextStage() const override {
        switch (stage_) {
        case CombatActionStage::Preparing: // ready to advance?
            return true;

        case CombatActionStage::Advancing: // ready to execute?
            if (advanceDistanceCovered_ >= totalAdvanceDistance_) {
                advanceDistanceCovered_ = totalAdvanceDistance_;
                combatant_->SetPosition(combatant_->OriginalPosition() +
                                        advanceDirection_ * totalAdvanceDistance_);
                return true;
            } else {
                return false;
            }

        case CombatActionStage::Executing: // ready to return?
            return combatant_->GetCombatSprite().IsPlaybackComplete();

        case CombatActionStage::Returning: // ready to finish?
            if (advanceDistanceCovered_ <= 0.0f) {
                advanceDistanceCovered_ = 0.0f;
                combatant_->SetPosition(combatant_->OriginalPosition());
                return true;
            } else {
                return false;
            }

        case CombatActionStage::Finishing: // ready to complete?
            return true;

        default:
            break;
        }

        // fall through to the base behavior
        return CombatAction::IsReadyForNextStage();
    }

private:
    // The speed at which the advancing character moves, in units per second.
    static constexpr float AdvanceSpeed = 300.0f;

    // The offset from the advance destination to the target position.
    static Vector2 AdvanceOffset() { return Vector2(85.0f, 0.0f); }

    // The direction of the advancement.
    mutable Vector2 advanceDirection_;

    // The distance covered so far by the advance/return action.
    mutable float advanceDistanceCovered_ = 0.0f;

    // The total distance between the original combatant position and the target.
    mutable float totalAdvanceDistance_ = 0.0f;
};

} // namespace RolePlaying
