// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "System/EventArgs.hpp"

namespace GameStateManagement
{
    /** @brief Event arguments containing the player that triggered an action. */
    class PlayerIndexEventArgs final : public System::EventArgs
    {
    public:
        /** @brief Constructs event arguments. @param playerIndex Triggering player. */
        explicit PlayerIndexEventArgs(Microsoft::Xna::Framework::PlayerIndex playerIndex)
            : playerIndex_(playerIndex) {}
        /** @brief Gets the triggering player. @return Triggering player index. */
        [[nodiscard]] Microsoft::Xna::Framework::PlayerIndex getPlayerIndexProperty() const
        { return playerIndex_; }
    private:
        Microsoft::Xna::Framework::PlayerIndex playerIndex_;
    };
}
