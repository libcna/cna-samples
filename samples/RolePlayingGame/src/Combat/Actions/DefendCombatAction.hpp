#pragma once

// DefendCombatAction.hpp -- C++ port of Combat/Actions/DefendCombatAction.cs.

#include "CombatAction.hpp"

namespace RolePlaying {

// A defend combat action, including related data and calculations.
class DefendCombatAction : public CombatAction {
public:
    // Constructs a new DefendCombatAction object.
    explicit DefendCombatAction(Combatant& combatant) : CombatAction(combatant) {}

    // Returns true if the action is offensive, targeting the opponents.
    bool IsOffensive() const override { return false; }

    // Returns true if this action requires a target.
    bool IsTargetNeeded() const override { return false; }

    // The heuristic used to compare actions of this type to similar ones.
    int Heuristic() const override { return 0; }

protected:
    // Starts a new combat stage. Called right after the stage changes.
    void StartStage() override {
        switch (stage_) {
        case CombatActionStage::Preparing: // called from Start()
            GetCombatant().GetCombatSprite().PlayAnimation("Defend");
            break;

        case CombatActionStage::Executing:
            GetCombatant().CombatEffects().AddStatistics(
                StatisticsValue(0, 0, 0,
                                GetCombatant().GetCharacter().CharacterStatistics().PhysicalDefense,
                                0,
                                GetCombatant().GetCharacter().CharacterStatistics().MagicalDefense),
                1);
            break;

        default:
            break;
        }
    }
};

} // namespace RolePlaying
