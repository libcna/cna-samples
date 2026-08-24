// SPDX-License-Identifier: MS-PL

#include "Keymap.hpp"

namespace Spacewar
{
    void Keymap::Add(GamePadKeys gamePadKey, Microsoft::Xna::Framework::Input::Keys key)
    {
        bindings_.insert_or_assign(gamePadKey, key);
    }

    Microsoft::Xna::Framework::Input::Keys Keymap::Get(GamePadKeys gamePadKey) const
    {
        return bindings_.at(gamePadKey);
    }
}
