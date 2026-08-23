// SPDX-License-Identifier: MS-PL

#pragma once

#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace InputSequenceSample::Direction
{
    using Microsoft::Xna::Framework::Input::Buttons;
    using Microsoft::Xna::Framework::Input::GamePadState;
    using Microsoft::Xna::Framework::Input::KeyboardState;
    using Microsoft::Xna::Framework::Input::Keys;

    /** @brief Bit mask representing no direction. */
    inline constexpr Buttons None = static_cast<Buttons>(0);
    /** @brief Bit mask representing up on the D-pad or left thumbstick. */
    inline constexpr Buttons Up = Buttons::DPadUp | Buttons::LeftThumbstickUp;
    /** @brief Bit mask representing down on the D-pad or left thumbstick. */
    inline constexpr Buttons Down = Buttons::DPadDown | Buttons::LeftThumbstickDown;
    /** @brief Bit mask representing left on the D-pad or left thumbstick. */
    inline constexpr Buttons Left = Buttons::DPadLeft | Buttons::LeftThumbstickLeft;
    /** @brief Bit mask representing right on the D-pad or left thumbstick. */
    inline constexpr Buttons Right = Buttons::DPadRight | Buttons::LeftThumbstickRight;
    /** @brief Bit mask representing the up-left diagonal. */
    inline constexpr Buttons UpLeft = Up | Left;
    /** @brief Bit mask representing the up-right diagonal. */
    inline constexpr Buttons UpRight = Up | Right;
    /** @brief Bit mask representing the down-left diagonal. */
    inline constexpr Buttons DownLeft = Down | Left;
    /** @brief Bit mask representing the down-right diagonal. */
    inline constexpr Buttons DownRight = Down | Right;
    /** @brief Bit mask containing every direction bit. */
    inline constexpr Buttons Any = Up | Down | Left | Right;

    /**
     * @brief Gets the current eight-way direction from a gamepad and keyboard.
     *
     * @param gamePad Current gamepad state.
     * @param keyboard Current keyboard state.
     * @return Combined vertical and horizontal direction flags.
     */
    [[nodiscard]] inline Buttons FromInput(
        const GamePadState& gamePad,
        const KeyboardState& keyboard)
    {
        Buttons direction = None;

        if (gamePad.IsButtonDown(Buttons::DPadUp) ||
            gamePad.IsButtonDown(Buttons::LeftThumbstickUp) ||
            keyboard.IsKeyDown(Keys::Up))
        {
            direction |= Up;
        }
        else if (gamePad.IsButtonDown(Buttons::DPadDown) ||
                 gamePad.IsButtonDown(Buttons::LeftThumbstickDown) ||
                 keyboard.IsKeyDown(Keys::Down))
        {
            direction |= Down;
        }

        if (gamePad.IsButtonDown(Buttons::DPadLeft) ||
            gamePad.IsButtonDown(Buttons::LeftThumbstickLeft) ||
            keyboard.IsKeyDown(Keys::Left))
        {
            direction |= Left;
        }
        else if (gamePad.IsButtonDown(Buttons::DPadRight) ||
                 gamePad.IsButtonDown(Buttons::LeftThumbstickRight) ||
                 keyboard.IsKeyDown(Keys::Right))
        {
            direction |= Right;
        }

        return direction;
    }

    /**
     * @brief Removes non-direction button flags.
     *
     * @param buttons Complete button flag set.
     * @return Only the direction flags.
     */
    [[nodiscard]] inline Buttons FromButtons(Buttons buttons)
    {
        return buttons & Any;
    }
}
