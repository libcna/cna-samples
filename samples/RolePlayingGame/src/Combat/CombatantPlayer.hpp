#pragma once

// CombatantPlayer.hpp -- C++ port of Combat/CombatantPlayer.cs.

#include <memory>

#include "System/ArgumentNullException.hpp"

#include "../Data/Characters/Player.hpp"
#include "Combatant.hpp"

namespace RolePlaying {

using RolePlayingGameData::Player;

// Encapsulates all of the combat-runtime data for a particular player combatant.
class CombatantPlayer : public Combatant {
public:
    // Construct a new CombatantPlayer object containing the given player.
    explicit CombatantPlayer(const std::shared_ptr<Player>& player) : player_(player) {
        // check the parameter
        if (player == nullptr) {
            throw System::ArgumentNullException("player");
        }

        // if the player starts dead, make sure the sprite is already "dead"
        if (IsDeadOrDying()) {
            if (Statistics().HealthPoints > 0) {
                SetState(Character::CharacterState::Idle);
            } else {
                GetCombatSprite().PlayAnimation("Die");
                GetCombatSprite().AdvanceToEnd();
            }
        } else {
            SetState(Character::CharacterState::Idle);
            GetCombatSprite().PlayAnimation("Idle");
        }
    }

    // The Player object encapsulated by this object.
    const std::shared_ptr<Player>& GetPlayer() const { return player_; }

    // The character encapsulated by this combatant.
    FightingCharacter& GetCharacter() const override { return *player_; }

    std::shared_ptr<FightingCharacter> GetCharacterShared() const override { return player_; }

    // The current state of this combatant.
    Character::CharacterState GetState() const override { return player_->State; }

    void SetState(Character::CharacterState value) override {
        if (value == player_->State) {
            return;
        }
        player_->State = value;
        switch (player_->State) {
        case Character::CharacterState::Idle:
            GetCombatSprite().PlayAnimation("Idle");
            break;

        case Character::CharacterState::Hit:
            GetCombatSprite().PlayAnimation("Hit");
            break;

        case Character::CharacterState::Dying:
            player_->StatisticsModifiers.HealthPoints =
                -1 * player_->CharacterStatistics().HealthPoints;
            GetCombatSprite().PlayAnimation("Die");
            break;

        default:
            break;
        }
    }

    // Accessor for the combat sprite for this combatant.
    AnimatingSprite& GetCombatSprite() const override { return *player_->CombatSprite; }

    // The current statistics of this combatant.
    StatisticsValue Statistics() const override {
        return player_->CurrentStatistics() + CombatEffects().TotalStatistics();
    }

    // Heals the combatant by the given amount.
    void Heal(const StatisticsValue& healingStatistics, int duration) override {
        if (duration > 0) {
            CombatEffects().AddStatistics(healingStatistics, duration);
        } else {
            player_->StatisticsModifiers += healingStatistics;
            player_->StatisticsModifiers.ApplyMaximum(StatisticsValue());
        }
        Combatant::Heal(healingStatistics, duration);
    }

    // Damages the combatant by the given amount.
    void Damage(const StatisticsValue& damageStatistics, int duration) override {
        if (duration > 0) {
            CombatEffects().AddStatistics(StatisticsValue() - damageStatistics, duration);
        } else {
            player_->StatisticsModifiers -= damageStatistics;
            player_->StatisticsModifiers.ApplyMaximum(StatisticsValue());
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

        // reduce the player's magic points by the spell's cost
        player_->StatisticsModifiers.MagicPoints -= spell->MagicPointCost;

        return true;
    }

private:
    // The Player object encapsulated by this object.
    std::shared_ptr<Player> player_;
};

} // namespace RolePlaying
