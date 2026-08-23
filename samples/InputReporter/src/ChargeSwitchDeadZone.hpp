// SPDX-License-Identifier: MS-PL

#pragma once

#include "ChargeSwitch.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"

namespace InputReporter
{
    /** @brief Charge switch that changes the active gamepad dead-zone mode. */
    class ChargeSwitchDeadZone final : public ChargeSwitch
    {
    public:
        /**
         * @brief Creates the dead-zone charge switch.
         *
         * @param duration Charge duration in seconds.
         */
        explicit ChargeSwitchDeadZone(float duration)
            : ChargeSwitch(duration)
        {
        }

    protected:
        /**
         * @brief Tests whether the Start button is held.
         *
         * @param gamePadState Current controller state.
         * @return True while Start is pressed.
         */
        [[nodiscard]] bool IsCharging(const GamePadState& gamePadState) const override
        {
            using Microsoft::Xna::Framework::Input::ButtonState;
            return gamePadState.getButtonsProperty().getStartProperty() == ButtonState::Pressed;
        }
    };
}
