// SPDX-License-Identifier: MS-PL
#pragma once

#include "GamePads.hpp"

namespace Spacewar
{
    class XInputHelper
    {
    public:
        [[nodiscard]] static GamePads& getGamePadsProperty();
        static void Update(Microsoft::Xna::Framework::Game& game,
                           const Microsoft::Xna::Framework::Input::KeyboardState& keyState);
    };
}
