// SPDX-License-Identifier: MS-PL
#pragma once

#include "Keymap.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"

namespace Spacewar
{
    class GamePadHelper
    {
    public:
        explicit GamePadHelper(Microsoft::Xna::Framework::PlayerIndex player);

        [[nodiscard]] const Microsoft::Xna::Framework::Input::GamePadState& getStateProperty() const;
        [[nodiscard]] float getThumbStickLeftXProperty() const;
        [[nodiscard]] float getThumbStickLeftYProperty() const;
        [[nodiscard]] float getThumbStickRightXProperty() const;
        [[nodiscard]] float getThumbStickRightYProperty() const;
        [[nodiscard]] bool getLeftTriggerPressedProperty();
        [[nodiscard]] bool getRightTriggerPressedProperty();
        [[nodiscard]] bool getAPressedProperty();
        [[nodiscard]] bool getBPressedProperty();
        [[nodiscard]] bool getYPressedProperty();
        [[nodiscard]] bool getXPressedProperty();
        [[nodiscard]] bool getStartPressedProperty();
        [[nodiscard]] bool getBackPressedProperty();
        [[nodiscard]] bool getUpPressedProperty();
        [[nodiscard]] bool getDownPressedProperty();
        [[nodiscard]] bool getLeftPressedProperty();
        [[nodiscard]] bool getRightPressedProperty();

        void Update(Microsoft::Xna::Framework::Game& game,
                    const Microsoft::Xna::Framework::Input::KeyboardState& keyState);

    private:
        Microsoft::Xna::Framework::PlayerIndex player_;
        Keymap keyMapping_;
        Microsoft::Xna::Framework::Input::KeyboardState keyState_;
        Microsoft::Xna::Framework::Game* game_ = nullptr;
        Microsoft::Xna::Framework::Input::GamePadState state_;

        bool aWasReleased_ = false;
        bool bWasReleased_ = false;
        bool yWasReleased_ = false;
        bool xWasReleased_ = false;
        bool startWasReleased_ = false;
        bool backWasReleased_ = false;
        bool upWasReleased_ = false;
        bool downWasReleased_ = false;
        bool leftWasReleased_ = false;
        bool rightWasReleased_ = false;
        bool leftTriggerWasReleased_ = false;
        bool rightTriggerWasReleased_ = false;

        bool kbAWasReleased_ = false;
        bool kbBWasReleased_ = false;
        bool kbYWasReleased_ = false;
        bool kbXWasReleased_ = false;
        bool kbStartWasReleased_ = false;
        bool kbBackWasReleased_ = false;
        bool kbUpWasReleased_ = false;
        bool kbDownWasReleased_ = false;
        bool kbLeftWasReleased_ = false;
        bool kbRightWasReleased_ = false;
        bool kbLeftTriggerWasReleased_ = false;
        bool kbRightTriggerWasReleased_ = false;

        [[nodiscard]] bool CheckPressed(Microsoft::Xna::Framework::Input::ButtonState state,
                                        bool& controlWasReleased);
        [[nodiscard]] bool CheckPressed(float state, bool& controlWasReleased);
        [[nodiscard]] bool CheckPressed(bool pressed, bool& controlWasReleased);
    };
}
