#pragma once

// Combatant.hpp -- C++ port of Combat/Combatant.cs.

#include <memory>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "../AudioManager.hpp"
#include "../Data/AnimatingSprite.hpp"
#include "../Data/Characters/Character.hpp"
#include "../Data/Characters/FightingCharacter.hpp"
#include "../Data/Spell.hpp"
#include "../Data/StatisticsValue.hpp"
#include "../Data/StatisticsValueStack.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using RolePlayingGameData::AnimatingSprite;
using RolePlayingGameData::Character;
using RolePlayingGameData::FightingCharacter;
using RolePlayingGameData::Spell;
using RolePlayingGameData::StatisticsValue;
using RolePlayingGameData::StatisticsValueStack;

class CombatAction; // fwd decl -- see Actions/CombatAction.hpp

// Base class for all combatants.
class Combatant {
public:
    virtual ~Combatant() = default;

    // The character encapsulated by this combatant.
    virtual FightingCharacter& GetCharacter() const = 0;

    // The same character as a shared_ptr, for the screens that take ownership of one. C# hands
    // the reference itself around; C++ needs the owning pointer the combatant already holds.
    virtual std::shared_ptr<FightingCharacter> GetCharacterShared() const = 0;

    // The current state of this combatant.
    virtual Character::CharacterState GetState() const = 0;
    virtual void SetState(Character::CharacterState value) = 0;

    // Returns true if the character is dead or dying.
    bool IsDeadOrDying() const {
        return GetState() == Character::CharacterState::Dying ||
               GetState() == Character::CharacterState::Dead;
    }

    // If true, the combatant has taken their turn this round.
    bool IsTurnTaken() const { return isTurnTaken_; }
    void SetIsTurnTaken(bool value) { isTurnTaken_ = value; }

    // Accessor for the combat sprite for this combatant.
    virtual AnimatingSprite& GetCombatSprite() const = 0;

    // The current position on screen for this combatant.
    Vector2 Position() const { return position_; }
    void SetPosition(const Vector2& value) { position_ = value; }

    // The original position on screen for this combatant.
    Vector2 OriginalPosition() const { return originalPosition_; }
    void SetOriginalPosition(const Vector2& value) { originalPosition_ = value; }

    // The current statistics of this combatant.
    virtual StatisticsValue Statistics() const = 0;

    // Heals the combatant's health by the given amount.
    void HealHealth(int healthHealingAmount, int duration) {
        Heal(StatisticsValue(healthHealingAmount, 0, 0, 0, 0, 0), duration);
    }

    // Heal the combatant by the given amount.
    // Defined out-of-line in CombatEngine.hpp -- it reports to the engine's effect list.
    virtual void Heal(const StatisticsValue& healingStatistics, int duration);

    // Damages the combatant's health by the given amount.
    void DamageHealth(int healthDamageAmount, int duration) {
        Damage(StatisticsValue(healthDamageAmount, 0, 0, 0, 0, 0), duration);
    }

    // Damages the combatant by the given amount.
    // Defined out-of-line in CombatEngine.hpp -- it reports to the engine's effect list.
    virtual void Damage(const StatisticsValue& damageStatistics, int duration);

    // Pay the cost for the given spell. Returns true if the cost could be paid.
    virtual bool PayCostForSpell(const std::shared_ptr<Spell>& spell) {
        (void)spell;
        return false;
    }

    // The current combat action for this combatant.
    const std::shared_ptr<CombatAction>& GetCombatAction() const { return combatAction_; }
    void SetCombatAction(const std::shared_ptr<CombatAction>& value) { combatAction_ = value; }

    // Statistics stack of the combat effects that are applied to this combatant.
    StatisticsValueStack& CombatEffects() { return combatEffects_; }
    const StatisticsValueStack& CombatEffects() const { return combatEffects_; }

    // Update the combatant for this frame.
    // Defined out-of-line in CombatEngine.hpp -- it drives CombatAction, which needs Combatant.
    virtual void Update(const GameTime& gameTime);

    // Advance the combatant state for one combat round.
    virtual void AdvanceRound() {
        // advance the combat effects stack
        combatEffects_.Advance();
    }

    // Draw the combatant for this frame.
    // Defined out-of-line in CombatEngine.hpp -- it draws through Session's ScreenManager.
    virtual void Draw(const GameTime& gameTime);

protected:
    // Constructs a new Combatant object.
    Combatant() = default;

private:
    bool isTurnTaken_ = false;
    Vector2 position_;
    Vector2 originalPosition_;
    std::shared_ptr<CombatAction> combatAction_;
    StatisticsValueStack combatEffects_;
};

} // namespace RolePlaying
