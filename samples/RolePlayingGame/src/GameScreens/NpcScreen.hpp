#pragma once

// NpcScreen.hpp -- C++ port of GameScreens/NpcScreen.cs.

#include <memory>

#include "System/ArgumentNullException.hpp"

#include "../Data/Characters/Character.hpp"
#include "../Data/MapEntry.hpp"
#include "DialogueScreen.hpp"

namespace RolePlaying {

using RolePlayingGameData::Character;
using RolePlayingGameData::MapEntry;

// Display of conversation dialog between the player and the npc.
template <typename T>
class NpcScreen : public DialogueScreen {
public:
    // Create a new NpcScreen object.
    explicit NpcScreen(const std::shared_ptr<MapEntry<T>>& mapEntry) : mapEntry_(mapEntry) {
        if (mapEntry == nullptr) {
            throw System::ArgumentNullException("mapEntry");
        }
        character_ = std::static_pointer_cast<Character>(mapEntry->Content);
        if (character_ == nullptr) {
            throw System::ArgumentNullException(
                "NpcScreen requires a MapEntry with a character.");
        }
        SetTitleText(character_->Name());
    }

protected:
    std::shared_ptr<MapEntry<T>> mapEntry_;
    std::shared_ptr<Character> character_;
};

} // namespace RolePlaying
