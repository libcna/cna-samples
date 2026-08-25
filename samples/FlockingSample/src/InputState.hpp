// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// InputState.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace Flocking
{
    using namespace Microsoft::Xna::Framework::Input;

    /**
     * @brief Reads the keyboard and gamepad and answers the questions the game asks.
     */
    class InputState
    {
    public:
        /** @brief This frame's keyboard state. */
        KeyboardState CurrentKeyState;
        /** @brief This frame's gamepad state. */
        GamePadState CurrentPadState;
        /** @brief Last frame's keyboard state. */
        KeyboardState LastKeyState;
        /** @brief Last frame's gamepad state. */
        GamePadState LastPadState;

        /**
         * @brief How far the cat should move vertically; negative moves up.
         * @return The vertical movement, -1 to 1.
         */
        [[nodiscard]] float getMoveCatYProperty() const;

        /**
         * @brief How far the cat should move horizontally.
         * @return The horizontal movement, -1 to 1.
         */
        [[nodiscard]] float getMoveCatXProperty() const;

        /**
         * @brief How far the selected slider should move this frame.
         * @return The slider movement.
         */
        [[nodiscard]] float getSliderMoveProperty() const;

        /**
         * @brief Whether the player asked to exit.
         * @return True on a new Escape or Back press.
         */
        [[nodiscard]] bool getExitProperty() const;

        /**
         * @brief Whether the player asked to reset the distances.
         * @return True on a new B press.
         */
        [[nodiscard]] bool getResetDistancesProperty() const;

        /**
         * @brief Whether the player asked to reset the flock.
         * @return True on a new X press.
         */
        [[nodiscard]] bool getResetFlockProperty() const;

        /**
         * @brief Whether the player asked to select the slider above.
         * @return True on a new Up press.
         */
        [[nodiscard]] bool getUpProperty() const;

        /**
         * @brief Whether the player asked to select the slider below.
         * @return True on a new Down press.
         */
        [[nodiscard]] bool getDownProperty() const;

        /**
         * @brief Whether the player asked to add or remove the cat.
         * @return True on a new Y press.
         */
        [[nodiscard]] bool getToggleCatButtonProperty() const;

        /** @brief Reads this frame's keyboard and gamepad, keeping last frame's. */
        void Update();

    private:
        [[nodiscard]] bool IsNewKeyPress(Keys key) const;
    };
}
