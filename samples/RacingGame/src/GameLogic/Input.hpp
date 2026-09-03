// SPDX-License-Identifier: MS-PL

#pragma once

#include "GameLogic/CarControlState.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"

namespace RacingGame::GameLogic
{
    /** @brief One complete game-level input result captured at a frame boundary. */
    struct ControlFrame
    {
        /** @brief Controls consumed by the car and chase camera. */
        CarControlState car;
        /** @brief True when the desktop Escape or gamepad Back control is down. */
        bool exitRequested = false;
    };

    /** @brief Provides one logical Racing control frame. */
    class ControlSource
    {
    public:
        /** @brief Destroys a game-owned control source through its interface. */
        virtual ~ControlSource() = default;

        /**
         * @brief Captures controls for one update.
         * @param inGame True while the race screen owns mouse capture.
         * @param appActive True while the game window is active.
         * @param displayWidth Current backbuffer width.
         * @param displayHeight Current backbuffer height.
         * @return Captured logical controls.
         */
        [[nodiscard]] virtual ControlFrame Capture(
            bool inGame, bool appActive, int displayWidth,
            int displayHeight) = 0;
    };

    /** @brief Reproduces the original desktop keyboard, mouse and gamepad capture. */
    class Input final : public ControlSource
    {
    public:
        /** @brief Creates the input helper with the XNA default device states. */
        Input();

        /**
         * @brief Captures, filters and maps the current desktop device states.
         * @param inGame True while the race screen owns mouse capture.
         * @param appActive True while the game window is active.
         * @param displayWidth Current backbuffer width.
         * @param displayHeight Current backbuffer height.
         * @return Captured logical controls.
         */
        [[nodiscard]] ControlFrame Capture(
            bool inGame, bool appActive, int displayWidth,
            int displayHeight) override;

    private:
        Microsoft::Xna::Framework::Input::MouseState mouseState;
        float lastMouseXMovement = 0.0f;
        float lastMouseYMovement = 0.0f;
        int mouseWheelValue = 0;
    };
}
