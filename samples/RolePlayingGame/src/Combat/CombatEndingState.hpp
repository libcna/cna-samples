#pragma once

// CombatEndingState.hpp -- C++ port of Combat/CombatEndingState.cs.

namespace RolePlaying {

enum class CombatEndingState {
    // All of the monsters died in combat.
    Victory,

    // The party successfully fled from combat.
    Fled,

    // All of the players died in combat.
    Loss,
};

} // namespace RolePlaying
