#pragma once

// CombatantMonster.hpp -- C++ port of Combat/CombatantMonster.cs.

#include <memory>

#include "System/ArgumentNullException.hpp"

#include "../Data/Characters/Monster.hpp"
#include "Actions/CombatAction.hpp"
#include "Combatant.hpp"

namespace RolePlaying {

using RolePlayingGameData::Monster;

class ArtificialIntelligence; // fwd decl -- see ArtificialIntelligence.hpp

// Encapsulates all of the combat-runtime data for a particular monster combatant.
//
// There may be many of a particular Monster in combat. This class adds the statistics and AI
// data that are particular to this particular combatant.
class CombatantMonster : public Combatant {
public:
    // Create a new CombatantMonster object containing the given monster.
    // Defined out-of-line in ArtificialIntelligence.hpp -- it constructs the AI, which needs
    // this type complete.
    explicit CombatantMonster(const std::shared_ptr<Monster>& monster);

    ~CombatantMonster() override;

    // The monster content object that this combatant uses.
    const std::shared_ptr<Monster>& GetMonster() const { return monster_; }

    // The character encapsulated by this combatant.
    FightingCharacter& GetCharacter() const override { return *monster_; }

    std::shared_ptr<FightingCharacter> GetCharacterShared() const override { return monster_; }

    // The current state of this combatant.
    Character::CharacterState GetState() const override { return state_; }

    void SetState(Character::CharacterState value) override {
        if (value == state_) {
            return;
        }
        state_ = value;
        switch (state_) {
        case Character::CharacterState::Idle:
            GetCombatSprite().PlayAnimation("Idle");
            break;

        case Character::CharacterState::Hit:
            GetCombatSprite().PlayAnimation("Hit");
            break;

        case Character::CharacterState::Dying:
            statistics_.HealthPoints = 0;
            GetCombatSprite().PlayAnimation("Die");
            break;

        default:
            break;
        }
    }

    // Accessor for the combat sprite for this combatant.
    AnimatingSprite& GetCombatSprite() const override { return *combatSprite_; }

    // The current statistics of this combatant.
    StatisticsValue Statistics() const override {
        return statistics_ + CombatEffects().TotalStatistics();
    }

    // Heals the combatant by the given amount.
    void Heal(const StatisticsValue& healingStatistics, int duration) override {
        if (duration > 0) {
            CombatEffects().AddStatistics(healingStatistics, duration);
        } else {
            statistics_ += healingStatistics;
            statistics_.ApplyMaximum(monster_->CharacterStatistics());
        }
        Combatant::Heal(healingStatistics, duration);
    }

    // Damages the combatant by the given amount.
    void Damage(const StatisticsValue& damageStatistics, int duration) override {
        if (duration > 0) {
            CombatEffects().AddStatistics(StatisticsValue() - damageStatistics, duration);
        } else {
            statistics_ -= damageStatistics;
            statistics_.ApplyMaximum(monster_->CharacterStatistics());
        }
        Combatant::Damage(damageStatistics, duration);
    }

    // Pay the cost for the given spell. Returns true if the cost could be paid.
    bool PayCostForSpell(const std::shared_ptr<Spell>& spell) override {
        // check the parameter.
        if (spell == nullptr) {
            throw System::ArgumentNullException("spell");
        }

        // check the requirements
        if (Statistics().MagicPoints < spell->MagicPointCost) {
            return false;
        }

        // reduce the monster's magic points by the spell's cost
        statistics_.MagicPoints -= spell->MagicPointCost;

        return true;
    }

    // The artificial intelligence data for this particular combatant.
    ArtificialIntelligence& GetArtificialIntelligence() const { return *artificialIntelligence_; }

    // Update the monster for this frame.
    void Update(const GameTime& gameTime) override {
        // start any waiting action immediately
        if (GetCombatAction() != nullptr &&
            GetCombatAction()->Stage() == CombatAction::CombatActionStage::NotStarted) {
            GetCombatAction()->Start();
        }

        Combatant::Update(gameTime);
    }

private:
    // The monster content object that this combatant uses.
    std::shared_ptr<Monster> monster_;

    // The current state of this combatant.
    Character::CharacterState state_ = Character::CharacterState::Idle;

    // The combat sprite for this combatant, copied from the monster.
    std::shared_ptr<AnimatingSprite> combatSprite_;

    // The statistics for this particular combatant.
    StatisticsValue statistics_;

    // The artificial intelligence data for this particular combatant.
    std::unique_ptr<ArtificialIntelligence> artificialIntelligence_;
};

} // namespace RolePlaying
