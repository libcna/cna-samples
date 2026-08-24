// SPDX-License-Identifier: MS-PL

#include "GamePads.hpp"

namespace Spacewar
{
    using Microsoft::Xna::Framework::PlayerIndex;

    GamePads::GamePads()
        : gamePads_{GamePadHelper(PlayerIndex::One), GamePadHelper(PlayerIndex::Two),
                    GamePadHelper(PlayerIndex::Three), GamePadHelper(PlayerIndex::Four)}
    {
    }

    GamePadHelper& GamePads::operator[](PlayerIndex player)
    {
        return gamePads_[static_cast<std::size_t>(player)];
    }

    void GamePads::Update(Microsoft::Xna::Framework::Game& game,
                          const Microsoft::Xna::Framework::Input::KeyboardState& keyState)
    {
        for (auto& gamePad : gamePads_) gamePad.Update(game, keyState);
    }
}
