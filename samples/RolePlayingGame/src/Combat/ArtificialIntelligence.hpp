#pragma once

// ArtificialIntelligence.hpp -- C++ port of Combat/ArtificialIntelligence.cs.

#include <algorithm>
#include <climits>
#include <memory>
#include <vector>

#include "System/ArgumentNullException.hpp"

#include "../Data/Spell.hpp"
#include "../Session/Session.hpp"
#include "Actions/CombatAction.hpp"
#include "Actions/DefendCombatAction.hpp"
#include "Actions/MeleeCombatAction.hpp"
#include "Actions/SpellCombatAction.hpp"
#include "CombatantMonster.hpp"

namespace RolePlaying {

// Determines actions for a given monster in combat.
//
// This was separated from the Monster type so the two kinds of code (combat operation and data
// encapsulation) remain clear for easy re-use.
class ArtificialIntelligence {
public:
    // Construct a new ArtificialIntelligence object to control a given combatant.
    explicit ArtificialIntelligence(CombatantMonster& monster) : monster_(&monster) {
        // generate all actions available
        GenerateAllActions();
    }

    // Choose the next action for the monster.
    // Defined out-of-line in CombatEngine.hpp -- target selection reads the engine's rosters.
    std::shared_ptr<CombatAction> ChooseAction();

private:
    // Generate the actions available to this monster.
    void GenerateAllActions() {
        // clear out any pre-existing actions
        offensiveActions_.clear();
        defensiveActions_.clear();

        // generate the melee attack option
        GenerateMeleeAction();

        // generate the spell attack options
        GenerateSpellAttackActions();

        // generate the defend action
        GenerateDefendAction();

        // sort the lists by potential, descending
        std::stable_sort(offensiveActions_.begin(), offensiveActions_.end(),
                         CombatAction::CompareCombatActionsByHeuristic);
        std::stable_sort(defensiveActions_.begin(), defensiveActions_.end(),
                         CombatAction::CompareCombatActionsByHeuristic);
    }

    // Generate the melee attack option for this monster.
    void GenerateMeleeAction() {
        // add a new melee action to the list
        offensiveActions_.push_back(std::make_shared<MeleeCombatAction>(*monster_));
    }

    // Generate the defend option for this monster.
    void GenerateDefendAction() {
        // add a new defend action to the list
        defensiveActions_.push_back(std::make_shared<DefendCombatAction>(*monster_));
    }

    // Generate the spell attack options for this monster.
    void GenerateSpellAttackActions() {
        // retrieve the spells for this monster
        const std::vector<std::shared_ptr<RolePlayingGameData::Spell>>& spells =
            monster_->GetMonster()->Spells();

        // if there are no spells, then there's nothing to do
        if (spells.empty()) {
            return;
        }

        // check each spell for attack actions
        for (const std::shared_ptr<RolePlayingGameData::Spell>& spell : spells) {
            // skip non-offensive spells
            if (!spell->IsOffensive) {
                continue;
            }

            // add the new action to the list
            offensiveActions_.push_back(std::make_shared<SpellCombatAction>(*monster_, spell));
        }
    }

    // Choose which offensive action to perform.
    // Defined out-of-line in CombatEngine.hpp -- it reads CombatEngine::Players().
    std::shared_ptr<CombatAction> ChooseOffensiveAction();

    // Choose which defensive action to perform.
    // Defined out-of-line in CombatEngine.hpp -- it reads CombatEngine::Monsters().
    std::shared_ptr<CombatAction> ChooseDefensiveAction();

    // The monster that this object is choosing actions for.
    CombatantMonster* monster_;

    // The offensive actions available to the monster.
    std::vector<std::shared_ptr<CombatAction>> offensiveActions_;

    // The defensive actions available to the monster.
    std::vector<std::shared_ptr<CombatAction>> defensiveActions_;
};

// ---- CombatantMonster methods that need ArtificialIntelligence complete ----

inline CombatantMonster::CombatantMonster(const std::shared_ptr<Monster>& monster)
    : monster_(monster) {
    // check the parameter
    if (monster == nullptr) {
        throw System::ArgumentNullException("monster");
    }

    statistics_ += monster->CharacterStatistics();
    combatSprite_ = monster->CombatSprite->Clone();
    SetState(Character::CharacterState::Idle);
    GetCombatSprite().PlayAnimation("Idle");

    // create the AI data
    artificialIntelligence_ = std::make_unique<ArtificialIntelligence>(*this);
}

inline CombatantMonster::~CombatantMonster() = default;

} // namespace RolePlaying
