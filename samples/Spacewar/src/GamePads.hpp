// SPDX-License-Identifier: MS-PL
#pragma once

#include "GamePadHelper.hpp"

#include <array>

namespace Spacewar
{
    class GamePads
    {
    public:
        GamePads();
        [[nodiscard]] GamePadHelper& operator[](Microsoft::Xna::Framework::PlayerIndex player);
        void Update(Microsoft::Xna::Framework::Game& game,
                    const Microsoft::Xna::Framework::Input::KeyboardState& keyState);

    private:
        std::array<GamePadHelper, 4> gamePads_;
    };
}
