// SPDX-License-Identifier: MS-PL

#include "XInputHelper.hpp"

namespace Spacewar
{
    GamePads& XInputHelper::getGamePadsProperty()
    {
        static GamePads gamePads;
        return gamePads;
    }

    void XInputHelper::Update(Microsoft::Xna::Framework::Game& game,
                              const Microsoft::Xna::Framework::Input::KeyboardState& keyState)
    {
        getGamePadsProperty().Update(game, keyState);
    }
}
