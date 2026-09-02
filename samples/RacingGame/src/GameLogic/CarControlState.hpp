// SPDX-License-Identifier: MS-PL

#pragma once

#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"

namespace RacingGame::GameLogic
{
    /** @brief One game-owned logical snapshot of controls consumed by CarPhysics. */
    struct CarControlState
    {
        /**
         * @brief Maps one captured XNA device-state set to the controls read by CarPhysics.
         * @param keyboard Current keyboard state.
         * @param mouse Current mouse state.
         * @param smoothedMouseXMovement Original Input helper's smoothed X delta.
         * @param currentMouseWheelDelta Original Input helper's wheel delta.
         * @param gamePad Current player-one gamepad state.
         * @return One immutable-for-the-frame logical control snapshot.
         */
        [[nodiscard]] static CarControlState FromXnaInput(
            const Microsoft::Xna::Framework::Input::KeyboardState& keyboard,
            const Microsoft::Xna::Framework::Input::MouseState& mouse,
            float smoothedMouseXMovement, int currentMouseWheelDelta,
            const Microsoft::Xna::Framework::Input::GamePadState& gamePad);

        bool keyboardLeftPressed = false;
        bool keyboardRightPressed = false;
        bool keyA = false;
        bool keyD = false;
        bool keyE = false;
        float mouseXMovement = 0.0f;
        bool gamePadConnected = false;
        float gamePadLeftStickX = 0.0f;
        bool gamePadDPadLeft = false;
        bool gamePadDPadRight = false;

        bool keyPageUp = false;
        bool keyPageDown = false;
        bool gamePadX = false;
        bool gamePadY = false;
        int mouseWheelDelta = 0;

        bool keyboardUpPressed = false;
        bool keyboardDownPressed = false;
        bool keyW = false;
        bool keyS = false;
        bool keyO = false;
        bool mouseLeftButtonPressed = false;
        bool mouseRightButtonPressed = false;
        bool mouseMiddleButtonPressed = false;
        bool gamePadA = false;
        bool gamePadB = false;
        float gamePadLeftTrigger = 0.0f;
        float gamePadRightTrigger = 0.0f;
        bool gamePadDPadUp = false;
        bool gamePadDPadDown = false;
        bool keySpace = false;
    };
}
