#pragma once

// SpellCombatAction.hpp -- C++ port of Combat/Actions/SpellCombatAction.cs.

#include <memory>

#include "Microsoft/Xna/Framework/Audio/AudioStopOptions.hpp"
#include "Microsoft/Xna/Framework/Audio/Cue.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/ArgumentNullException.hpp"

#include "../../AudioManager.hpp"
#include "../../Data/Int32Range.hpp"
#include "../../Data/Spell.hpp"
#include "../../Session/Session.hpp"
#include "CombatAction.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Audio::AudioStopOptions;
using Microsoft::Xna::Framework::Audio::Cue;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using RolePlayingGameData::Int32Range;
using RolePlayingGameData::Spell;

// A spell-casting combat action, including related data and calculations.
class SpellCombatAction : public CombatAction {
public:
    // Constructs a new SpellCombatAction object.
    SpellCombatAction(Combatant& combatant, const std::shared_ptr<Spell>& spell)
        : CombatAction(combatant), spell_(spell) {
        // check the parameter
        if (spell == nullptr) {
            throw System::ArgumentNullException("spell");
        }

        adjacentTargets_ = spell_->AdjacentTargets;
    }

    // Returns true if the action is offensive, targeting the opponents.
    bool IsOffensive() const override { return spell_->IsOffensive; }

    // Returns true if the character can use this action.
    bool IsCharacterValidUser() const override {
        return spell_->MagicPointCost <= GetCombatant().Statistics().MagicPoints;
    }

    // Returns true if this action requires a target.
    bool IsTargetNeeded() const override { return true; }

    // The spell used in this action.
    const std::shared_ptr<Spell>& GetSpell() const { return spell_; }

    // The heuristic used to compare actions of this type to similar ones.
    int Heuristic() const override {
        return GetCombatant().Statistics().MagicalOffense +
               GetSpell()->TargetEffectRange().HealthPointsRange.Average();
    }

    // Start executing the combat action.
    void Start() override {
        // play the creation sound effect
        AudioManager::PlayCue(spell_->CreatingCueName);

        CombatAction::Start();
    }

    // Updates the action over time.
    void Update(const GameTime& gameTime) override {
        // update the animations
        float elapsedSeconds =
            (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
        spell_->SpellSprite->UpdateAnimation(elapsedSeconds);
        if (spell_->Overlay != nullptr) {
            spell_->Overlay->UpdateAnimation(elapsedSeconds);
            if (!spell_->Overlay->IsPlaybackComplete() &&
                Target->GetCombatSprite().IsPlaybackComplete()) {
                spell_->Overlay->StopAnimation();
            }
        }

        CombatAction::Update(gameTime);
    }

    // Draw any elements of the action that are independent of the character.
    void Draw(const GameTime& gameTime, SpriteBatch& spriteBatch) override {
        // draw the spell projectile
        if (!spell_->SpellSprite->IsPlaybackComplete()) {
            if (stage_ == CombatActionStage::Advancing) {
                spell_->SpellSprite->Draw(spriteBatch, spellSpritePosition_, 0.0f,
                                          projectileSpriteEffect_);
            } else {
                spell_->SpellSprite->Draw(spriteBatch, spellSpritePosition_, 0.0f);
            }
        }

        // draw the spell overlay
        if (spell_->Overlay != nullptr && !spell_->Overlay->IsPlaybackComplete()) {
            spell_->Overlay->Draw(spriteBatch, Target->Position(), 0.0f);
        }

        CombatAction::Draw(gameTime, spriteBatch);
    }

    // Calculate the spell damage done by the given combatant and spell.
    static StatisticsValue CalculateSpellDamage(Combatant& combatant,
                                                const std::shared_ptr<Spell>& spell) {
        // check the parameter
        if (spell == nullptr) {
            throw System::ArgumentNullException("spell");
        }

        // get the magical offense from the character's class, gear, and bonuses
        // -- note that this includes stat buffs
        int magicalOffense = combatant.Statistics().MagicalOffense;

        // add the magical offense to the spell
        StatisticsValue damage = spell->TargetEffectRange().GenerateValue(Session::GetRandom());
        damage.HealthPoints += (damage.HealthPoints != 0) ? magicalOffense : 0;
        damage.MagicPoints += (damage.MagicPoints != 0) ? magicalOffense : 0;
        damage.PhysicalOffense += (damage.PhysicalOffense != 0) ? magicalOffense : 0;
        damage.PhysicalDefense += (damage.PhysicalDefense != 0) ? magicalOffense : 0;
        damage.MagicalOffense += (damage.MagicalOffense != 0) ? magicalOffense : 0;
        damage.MagicalDefense += (damage.MagicalDefense != 0) ? magicalOffense : 0;

        // add in the spell damage
        return damage;
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
            spellSpritePosition_ = combatant_->OriginalPosition() +
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
                   spell_->SpellSprite->IsPlaybackComplete();

        case CombatActionStage::Advancing: // ready to execute?
            if (spell_->SpellSprite->IsPlaybackComplete() ||
                projectileDistanceCovered_ >= totalProjectileDistance_) {
                projectileDistanceCovered_ = totalProjectileDistance_;
                return true;
            }
            return false;

        case CombatActionStage::Executing: // ready to return?
            return spell_->SpellSprite->IsPlaybackComplete();

        default:
            break;
        }

        // fall through to the base behavior
        return CombatAction::IsReadyForNextStage();
    }

    // Apply the action's spell to the given target. Returns true if there was any effect.
    bool ApplySpell(Combatant& spellTarget) {
        StatisticsValue effectStatistics = CalculateSpellDamage(*combatant_, spell_);
        if (spell_->IsOffensive) {
            // calculate the defense
            Int32Range defenseRange = spellTarget.GetCharacter().MagicDefenseRange() +
                                      spellTarget.Statistics().MagicalDefense;
            int defense = defenseRange.GenerateValue(Session::GetRandom());
            // subtract the defense
            effectStatistics -=
                StatisticsValue(defense, defense, defense, defense, defense, defense);
            // make sure that this only contains damage
            effectStatistics.ApplyMinimum(StatisticsValue());
            // damage the target
            spellTarget.Damage(effectStatistics, spell_->TargetDuration);
        } else {
            // make sure that this only contains healing
            effectStatistics.ApplyMinimum(StatisticsValue());
            // heal the target
            spellTarget.Heal(effectStatistics, spell_->TargetDuration);
        }

        return !effectStatistics.IsZero();
    }

    // The spell used in this action.
    std::shared_ptr<Spell> spell_;

    // The current position of the spell sprite.
    Vector2 spellSpritePosition_;

    // The speed at which the projectile moves, in units per second.
    static constexpr float ProjectileSpeed = 600.0f;

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
