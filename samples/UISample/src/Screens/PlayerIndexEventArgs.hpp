// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "System/EventArgs.hpp"

namespace UserInterfaceSample {

class PlayerIndexEventArgs final : public System::EventArgs {
public:
    explicit PlayerIndexEventArgs(Microsoft::Xna::Framework::PlayerIndex playerIndex)
        : playerIndex_(playerIndex) {}

    [[nodiscard]] Microsoft::Xna::Framework::PlayerIndex getPlayerIndexProperty() const {
        return playerIndex_;
    }

private:
    Microsoft::Xna::Framework::PlayerIndex playerIndex_;
};

} // namespace UserInterfaceSample
