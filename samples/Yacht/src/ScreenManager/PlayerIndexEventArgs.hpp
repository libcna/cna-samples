#pragma once

// PlayerIndexEventArgs.hpp -- C++ port of Yacht/ScreenManager/PlayerIndexEventArgs.cs.

#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "System/EventArgs.hpp"

namespace GameStateManagement {

using Microsoft::Xna::Framework::PlayerIndex;

/**
 * @brief Custom event argument which includes the index of the player who triggered the event.
 *
 * This is used by the MenuEntry.Selected event.
 */
class PlayerIndexEventArgs : public System::EventArgs {
public:
    /**
     * @brief Constructor.
     *
     * @param playerIndex The player who triggered the event.
     */
    explicit PlayerIndexEventArgs(PlayerIndex playerIndex) : playerIndex_(playerIndex) {}

    /**
     * @brief Gets the index of the player who triggered this event.
     *
     * @return The player's index.
     */
    [[nodiscard]] PlayerIndex getPlayerIndexProperty() const { return playerIndex_; }

private:
    PlayerIndex playerIndex_;
};

} // namespace GameStateManagement
