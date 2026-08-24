// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Input/Keys.hpp"

#include <unordered_map>

namespace Spacewar
{
    enum class GamePadKeys
    {
        Start,
        Back,
        A,
        B,
        X,
        Y,
        Up,
        Down,
        Left,
        Right,
        LeftTrigger,
        RightTrigger,
        ThumbstickLeftXMin,
        ThumbstickLeftXMax,
        ThumbstickLeftYMin,
        ThumbstickLeftYMax,
        ThumbstickRightXMin,
        ThumbstickRightXMax,
        ThumbstickRightYMin,
        ThumbstickRightYMax,
    };

    class Keymap
    {
    public:
        void Add(GamePadKeys gamePadKey, Microsoft::Xna::Framework::Input::Keys key);
        [[nodiscard]] Microsoft::Xna::Framework::Input::Keys Get(GamePadKeys gamePadKey) const;

    private:
        std::unordered_map<GamePadKeys, Microsoft::Xna::Framework::Input::Keys> bindings_;
    };
}
