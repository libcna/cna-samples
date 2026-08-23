// SPDX-License-Identifier: MS-PL

#pragma once

#include "ChargeSwitch.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"

namespace InputReporter
{
    /** @brief Charge switch that exits the game. */
    class ChargeSwitchExit final : public ChargeSwitch
    {
    public:
        /**
         * @brief Creates the exit charge switch.
         *
         * @param duration Charge duration in seconds.
         */
        explicit ChargeSwitchExit(float duration)
            : ChargeSwitch(duration)
        {
        }

    protected:
        /**
         * @brief Tests whether the Back button is held.
         *
         * @param gamePadState Current controller state.
         * @return True while Back is pressed.
         */
        [[nodiscard]] bool IsCharging(const GamePadState& gamePadState) const override
        {
            using Microsoft::Xna::Framework::Input::ButtonState;
            return gamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed;
        }
    };
}
