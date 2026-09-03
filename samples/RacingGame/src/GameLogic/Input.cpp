// SPDX-License-Identifier: MS-PL

#include "GameLogic/Input.hpp"

#include <algorithm>
#include <cctype>

#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

namespace RacingGame::GameLogic
{
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Point;
    using namespace Microsoft::Xna::Framework::Input;

    namespace
    {
        bool IsTextKey(const Keys key)
        {
            const int value = static_cast<int>(key);
            return (value >= static_cast<int>(Keys::A) &&
                    value <= static_cast<int>(Keys::Z)) ||
                   (value >= static_cast<int>(Keys::D0) &&
                    value <= static_cast<int>(Keys::D9)) ||
                   key == Keys::Space || key == Keys::OemTilde ||
                   key == Keys::OemMinus || key == Keys::OemPipe ||
                   key == Keys::OemOpenBrackets ||
                   key == Keys::OemCloseBrackets ||
                   key == Keys::OemQuotes || key == Keys::OemQuestion ||
                   key == Keys::OemPlus || key == Keys::OemSemicolon ||
                   key == Keys::OemComma || key == Keys::OemPeriod;
        }

        char KeyToChar(const Keys key, const bool shift)
        {
            const int value = static_cast<int>(key);
            if (value >= static_cast<int>(Keys::A) &&
                value <= static_cast<int>(Keys::Z))
            {
                const char letter = static_cast<char>('A' +
                    value - static_cast<int>(Keys::A));
                return shift ? letter :
                    static_cast<char>(std::tolower(letter));
            }
            if (value >= static_cast<int>(Keys::D0) &&
                value <= static_cast<int>(Keys::D9))
            {
                static constexpr char ShiftDigits[]{')', '!', '@', '#', '$',
                                                      '%', '^', '&', '*', '('};
                const int digit = value - static_cast<int>(Keys::D0);
                return shift ? ShiftDigits[digit]
                             : static_cast<char>('0' + digit);
            }
            if (key == Keys::Space) return ' ';
            if (key == Keys::OemTilde) return shift ? '~' : '`';
            if (key == Keys::OemMinus) return shift ? '_' : '-';
            if (key == Keys::OemPipe) return shift ? '|' : '\\';
            if (key == Keys::OemOpenBrackets) return shift ? '{' : '[';
            if (key == Keys::OemCloseBrackets) return shift ? '}' : ']';
            if (key == Keys::OemQuotes) return shift ? '"' : '\'';
            if (key == Keys::OemQuestion) return shift ? '?' : '/';
            if (key == Keys::OemPlus) return shift ? '+' : '=';
            if (key == Keys::OemSemicolon) return shift ? ':' : ';';
            if (key == Keys::OemComma) return shift ? '<' : '.';
            if (key == Keys::OemPeriod) return shift ? '>' : ',';
            return ' ';
        }
    }

    Input::Input() = default;

    void Input::ResetMouseMotion()
    {
        lastMouseXMovement = 0.0f;
        lastMouseYMovement = 0.0f;
    }

    ControlFrame Input::Capture(
        const bool inGame, const bool appActive,
        const int displayWidth, const int displayHeight)
    {
        const MouseState mouseStateLastFrame = mouseState;
        const KeyboardState keyboardStateLastFrame = keyboardState;
        const GamePadState gamePadStateLastFrame = gamePadState;
        mouseState = Mouse::GetState();

        lastMouseXMovement += static_cast<float>(
            mouseState.getXProperty() - mouseStateLastFrame.getXProperty());
        lastMouseYMovement += static_cast<float>(
            mouseState.getYProperty() - mouseStateLastFrame.getYProperty());
        const float mouseXMovement = lastMouseXMovement / 2.0f;
        const float mouseYMovement = lastMouseYMovement / 2.0f;
        lastMouseXMovement -= lastMouseXMovement / 2.0f;
        lastMouseYMovement -= lastMouseYMovement / 2.0f;

        const int mouseWheelDelta =
            mouseState.getScrollWheelValueProperty() - mouseWheelValue;
        mouseWheelValue = mouseState.getScrollWheelValueProperty();

        const Point mousePosition(
            mouseState.getXProperty(), mouseState.getYProperty());
        const bool mouseLeftPressed =
            mouseState.getLeftButtonProperty() == ButtonState::Pressed;
        const bool mouseLeftJustPressed = mouseLeftPressed &&
            mouseStateLastFrame.getLeftButtonProperty() ==
                ButtonState::Released;

        if (inGame && appActive)
        {
            Mouse::SetPosition(displayWidth / 2, displayHeight / 2);
            mouseState = Mouse::GetState();
        }

        keyboardState = Keyboard::GetState();
        gamePadState = GamePad::GetState(PlayerIndex::One);

        const auto keyJustPressed = [&](const Keys key)
        {
            return keyboardState.IsKeyDown(key) &&
                   keyboardStateLastFrame.IsKeyUp(key);
        };
        const auto buttonPressed = [](const ButtonState state)
        {
            return state == ButtonState::Pressed;
        };
        const auto gamePadDirection = [&](const bool left, const bool current)
        {
            const auto& dpad = current
                ? gamePadState.getDPadProperty()
                : gamePadStateLastFrame.getDPadProperty();
            const float stick = (current
                ? gamePadState.getThumbSticksProperty()
                : gamePadStateLastFrame.getThumbSticksProperty())
                    .getLeftProperty().X;
            return buttonPressed(left ? dpad.getLeftProperty()
                                      : dpad.getRightProperty()) ||
                   (left ? stick < -0.75f : stick > 0.75f);
        };
        const auto gamePadVertical = [&](const bool up, const bool current)
        {
            const auto& dpad = current
                ? gamePadState.getDPadProperty()
                : gamePadStateLastFrame.getDPadProperty();
            const float stick = (current
                ? gamePadState.getThumbSticksProperty()
                : gamePadStateLastFrame.getThumbSticksProperty())
                    .getLeftProperty().Y;
            return buttonPressed(up ? dpad.getUpProperty()
                                    : dpad.getDownProperty()) ||
                   (up ? stick > 0.75f : stick < -0.75f);
        };

        ControlFrame result;
        result.mousePosition = mousePosition;
        result.hasMouseMoved = mouseXMovement > 1.0f || mouseYMovement > 1.0f;
        result.mouseLeftPressed = mouseLeftPressed;
        result.mouseLeftJustPressed = mouseLeftJustPressed;
        const bool gamePadLeft = gamePadDirection(true, true);
        const bool gamePadRight = gamePadDirection(false, true);
        const bool gamePadUp = gamePadVertical(true, true);
        const bool gamePadDown = gamePadVertical(false, true);
        result.leftPressed = keyboardState.IsKeyDown(Keys::Left) || gamePadLeft;
        result.rightPressed = keyboardState.IsKeyDown(Keys::Right) || gamePadRight;
        result.leftJustPressed = keyJustPressed(Keys::Left) ||
            (gamePadLeft && !gamePadDirection(true, false));
        result.rightJustPressed = keyJustPressed(Keys::Right) ||
            (gamePadRight && !gamePadDirection(false, false));
        result.upJustPressed = keyJustPressed(Keys::Up) ||
            (gamePadUp && !gamePadVertical(true, false));
        result.downJustPressed = keyJustPressed(Keys::Down) ||
            (gamePadDown && !gamePadVertical(false, false));
        result.acceptJustPressed = keyJustPressed(Keys::Space) ||
            (buttonPressed(gamePadState.getButtonsProperty().getAProperty()) &&
             !buttonPressed(gamePadStateLastFrame.getButtonsProperty()
                                .getAProperty()));
        result.cancelJustPressed =
            buttonPressed(gamePadState.getButtonsProperty().getBProperty()) &&
            !buttonPressed(gamePadStateLastFrame.getButtonsProperty()
                               .getBProperty());
        result.xJustPressed =
            buttonPressed(gamePadState.getButtonsProperty().getXProperty()) &&
            !buttonPressed(gamePadStateLastFrame.getButtonsProperty()
                               .getXProperty());
        result.backJustPressed = keyJustPressed(Keys::Escape) ||
            (buttonPressed(gamePadState.getButtonsProperty().getBackProperty()) &&
             !buttonPressed(gamePadStateLastFrame.getButtonsProperty()
                                .getBackProperty()));
        result.startPressed = buttonPressed(
            gamePadState.getButtonsProperty().getStartProperty());
        result.backspaceJustPressed = keyJustPressed(Keys::Back);
        result.f1JustPressed = keyJustPressed(Keys::F1);
        result.printScreenJustPressed = keyJustPressed(Keys::PrintScreen);
        result.gamePadYJustPressed =
            buttonPressed(gamePadState.getButtonsProperty().getYProperty()) &&
            !buttonPressed(gamePadStateLastFrame.getButtonsProperty()
                               .getYProperty());
        result.gamePadLeftShoulderPressed = buttonPressed(
            gamePadState.getButtonsProperty().getLeftShoulderProperty());
        const bool shift = keyboardState.IsKeyDown(Keys::LeftShift) ||
                           keyboardState.IsKeyDown(Keys::RightShift);
        auto pressedKeys = keyboardState.GetPressedKeys();
        std::sort(pressedKeys.begin(), pressedKeys.end(),
                  [](const Keys left, const Keys right)
                  {
                      return static_cast<int>(left) < static_cast<int>(right);
                  });
        for (const Keys key : pressedKeys)
        {
            if (keyJustPressed(key) && IsTextKey(key))
                result.typedText += KeyToChar(key, shift);
        }
        result.car = CarControlState::FromXnaInput(
            keyboardState, mouseState, mouseXMovement, mouseYMovement,
            mouseWheelDelta, gamePadState);
        return result;
    }
}
