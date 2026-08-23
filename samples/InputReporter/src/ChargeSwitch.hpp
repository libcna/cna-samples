// SPDX-License-Identifier: MS-PL

#pragma once

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "System/ArgumentOutOfRangeException.hpp"
#include "System/MulticastAction.hpp"

namespace InputReporter
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Input::GamePadState;

    /** @brief GamePad-controlled switch that fires after charging for a set duration. */
    class ChargeSwitch
    {
        float duration = 3.0f;
        float remaining = 0.0f;
        bool active = false;

    public:
        /** @brief Event raised when the switch finishes charging. */
        System::MulticastAction<> Fire;

        /**
         * @brief Gets whether the switch is currently charging.
         *
         * @return True while the controlling input is held.
         */
        [[nodiscard]] bool getActiveProperty() const
        {
            return active;
        }

        /**
         * @brief Creates a charge switch with the specified duration.
         *
         * @param duration Charge duration in seconds.
         */
        explicit ChargeSwitch(float duration)
        {
            Reset(duration);
        }

        /** @brief Destroys the charge switch. */
        virtual ~ChargeSwitch() = default;

        /**
         * @brief Updates charging state and fires the event when the duration elapses.
         *
         * @param gameTime Current timing snapshot.
         * @param gamePadState Current state of the selected controller.
         */
        void Update(const GameTime& gameTime, const GamePadState& gamePadState)
        {
            active = IsCharging(gamePadState);
            if (active)
            {
                if (remaining > 0.0f)
                {
                    remaining -= static_cast<float>(
                        gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());
                    if (remaining <= 0.0f && Fire)
                    {
                        Fire();
                    }
                }
            }
            else
            {
                Reset(duration);
            }
        }

        /**
         * @brief Resets the switch to the specified charge duration.
         *
         * @param duration Charge duration in seconds.
         */
        void Reset(float duration)
        {
            if (duration < 0.0f)
            {
                throw System::ArgumentOutOfRangeException("duration");
            }
            remaining = this->duration = duration;
        }

    protected:
        /**
         * @brief Tests whether the switch-specific controller input is held.
         *
         * @param gamePadState Current state of the selected controller.
         * @return True while the switch is charging.
         */
        [[nodiscard]] virtual bool IsCharging(const GamePadState& gamePadState) const = 0;
    };
}
