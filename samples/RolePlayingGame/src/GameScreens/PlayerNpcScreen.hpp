#pragma once

// PlayerNpcScreen.hpp -- C++ port of GameScreens/PlayerNpcScreen.cs.

#include <memory>

#include "System/ArgumentException.hpp"

#include "../Data/Characters/Player.hpp"
#include "../InputManager.hpp"
#include "../Session/Session.hpp"
#include "NpcScreen.hpp"

namespace RolePlaying {

using RolePlayingGameData::Player;

class StatisticsScreen; // fwd decl -- opened by the view action

// Displays the Player NPC screen, shown when encountering a player on the map. Typically, the
// user has an opportunity to invite the Player into the party.
class PlayerNpcScreen : public NpcScreen<Player> {
public:
    // Constructs a new PlayerNpcScreen object.
    explicit PlayerNpcScreen(const std::shared_ptr<MapEntry<Player>>& mapEntry)
        : NpcScreen<Player>(mapEntry) {
        // assign and check the parameter
        std::shared_ptr<Player> playerNpc = std::dynamic_pointer_cast<Player>(character_);
        if (playerNpc == nullptr) {
            throw System::ArgumentException("PlayerNpcScreen requires a MapEntry with a Player");
        }

        SetDialogueText(playerNpc->IntroductionDialogue);
        SetBackText("Reject");
        SetSelectText("Accept");
        isIntroduction_ = true;
    }

    // Handles user input.
    // Defined out-of-line in StatisticsScreen.hpp -- the view action opens that screen.
    void HandleInput() override;

private:
    // If true, the NPC's introduction dialogue is shown.
    bool isIntroduction_ = true;
};

} // namespace RolePlaying
