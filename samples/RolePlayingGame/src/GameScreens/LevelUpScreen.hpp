#pragma once

// LevelUpScreen.hpp -- simplified adaptation of GameScreens/LevelUpScreen.cs
// -- announces which party members leveled up as a dismissable DialogueScreen
// (the original shows a per-stat before/after comparison grid).

#include <memory>
#include <string>
#include <vector>

#include "../Data/Characters/Player.hpp"
#include "DialogueScreen.hpp"
#include "System/Int32.hpp"

namespace RolePlaying {

class LevelUpScreen : public DialogueScreen {
public:
    explicit LevelUpScreen(const std::vector<std::shared_ptr<RolePlayingGameData::Player>>& leveledUpPlayers) {
        TitleText = "Level Up!";
        std::string text;
        for (auto& player : leveledUpPlayers) {
            text += player->Name() + " reached level " + System::Int32::ToString(player->CharacterLevel()) + "!\n";
        }
        DialogueText = text;
        BackText.clear();
    }
};

} // namespace RolePlaying
