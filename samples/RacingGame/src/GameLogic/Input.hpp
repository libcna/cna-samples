// SPDX-License-Identifier: MS-PL

#pragma once

#include <string>

#include "GameLogic/CarControlState.hpp"
#include "GameLogic/MobileControls.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"

namespace RacingGame::GameLogic
{
    /** @brief One complete game-level input result captured at a frame boundary. */
    struct ControlFrame
    {
        /** @brief Controls consumed by the car and chase camera. */
        CarControlState car;
        /** @brief Current mobile touch geometry and overlay state. */
        MobileControlState mobile;
        /** @brief Current mouse position before in-race recentering. */
        Microsoft::Xna::Framework::Point mousePosition;
        /** @brief True when the mouse moved in the original positive-axis threshold test. */
        bool hasMouseMoved = false;
        /** @brief True while the left mouse button is held. */
        bool mouseLeftPressed = false;
        /** @brief True only on the frame the left mouse button becomes pressed. */
        bool mouseLeftJustPressed = false;
        /** @brief True while the logical left direction is held. */
        bool leftPressed = false;
        /** @brief True while the logical right direction is held. */
        bool rightPressed = false;
        /** @brief True only when the logical left direction becomes pressed. */
        bool leftJustPressed = false;
        /** @brief True only when the logical right direction becomes pressed. */
        bool rightJustPressed = false;
        /** @brief True only when the logical up direction becomes pressed. */
        bool upJustPressed = false;
        /** @brief True only when the logical down direction becomes pressed. */
        bool downJustPressed = false;
        /** @brief True only when keyboard Space or gamepad A becomes pressed. */
        bool acceptJustPressed = false;
        /** @brief True only when Escape or gamepad Back becomes pressed. */
        bool backJustPressed = false;
        /** @brief True only when gamepad B becomes pressed. */
        bool cancelJustPressed = false;
        /** @brief True only when gamepad X becomes pressed. */
        bool xJustPressed = false;
        /** @brief True while gamepad Start is held, matching the splash screen. */
        bool startPressed = false;
        /** @brief Original QWERTY characters newly typed during this frame. */
        std::string typedText;
        /** @brief True only when Backspace becomes pressed. */
        bool backspaceJustPressed = false;
        /** @brief True only when F1 becomes pressed. */
        bool f1JustPressed = false;
        /** @brief True only when PrintScreen becomes pressed. */
        bool printScreenJustPressed = false;
        /** @brief True only when gamepad Y becomes pressed. */
        bool gamePadYJustPressed = false;
        /** @brief True while gamepad LeftShoulder is held. */
        bool gamePadLeftShoulderPressed = false;
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

        /**
         * @brief Supplies the platform safe area used by providers with touch controls.
         * @param safeArea Safe interactive rectangle in backbuffer coordinates.
         */
        virtual void SetSafeArea(
            Microsoft::Xna::Framework::Rectangle safeArea)
        {
            (void)safeArea;
        }
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

        /** @brief Clears pending smoothed mouse motion after a touch-owned frame. */
        void ResetMouseMotion();

    private:
        Microsoft::Xna::Framework::Input::MouseState mouseState;
        Microsoft::Xna::Framework::Input::KeyboardState keyboardState;
        Microsoft::Xna::Framework::Input::GamePadState gamePadState;
        float lastMouseXMovement = 0.0f;
        float lastMouseYMovement = 0.0f;
        int mouseWheelValue = 0;
    };
}
