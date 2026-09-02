// SPDX-License-Identifier: MS-PL

#include "GameLogic/CarControlState.hpp"

#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace RacingGame::GameLogic
{
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePadState;
    using Microsoft::Xna::Framework::Input::KeyboardState;
    using Microsoft::Xna::Framework::Input::Keys;
    using Microsoft::Xna::Framework::Input::MouseState;

    CarControlState CarControlState::FromXnaInput(
        const KeyboardState& keyboard, const MouseState& mouse,
        const float smoothedMouseXMovement,
        const int currentMouseWheelDelta, const GamePadState& gamePad)
    {
        const auto pressed = [](const ButtonState value)
        {
            return value == ButtonState::Pressed;
        };

        CarControlState result;
        result.keyboardLeftPressed = keyboard.IsKeyDown(Keys::Left);
        result.keyboardRightPressed = keyboard.IsKeyDown(Keys::Right);
        result.keyA = keyboard.IsKeyDown(Keys::A);
        result.keyD = keyboard.IsKeyDown(Keys::D);
        result.keyE = keyboard.IsKeyDown(Keys::E);
        result.mouseXMovement = smoothedMouseXMovement;
        result.gamePadConnected = gamePad.getIsConnectedProperty();
        result.gamePadLeftStickX =
            gamePad.getThumbSticksProperty().getLeftProperty().X;
        result.gamePadDPadLeft = pressed(
            gamePad.getDPadProperty().getLeftProperty());
        result.gamePadDPadRight = pressed(
            gamePad.getDPadProperty().getRightProperty());

        result.keyPageUp = keyboard.IsKeyDown(Keys::PageUp);
        result.keyPageDown = keyboard.IsKeyDown(Keys::PageDown);
        result.gamePadX = pressed(
            gamePad.getButtonsProperty().getXProperty());
        result.gamePadY = pressed(
            gamePad.getButtonsProperty().getYProperty());
        result.mouseWheelDelta = currentMouseWheelDelta;

        result.keyboardUpPressed = keyboard.IsKeyDown(Keys::Up);
        result.keyboardDownPressed = keyboard.IsKeyDown(Keys::Down);
        result.keyW = keyboard.IsKeyDown(Keys::W);
        result.keyS = keyboard.IsKeyDown(Keys::S);
        result.keyO = keyboard.IsKeyDown(Keys::O);
        result.mouseLeftButtonPressed = pressed(
            mouse.getLeftButtonProperty());
        result.mouseRightButtonPressed = pressed(
            mouse.getRightButtonProperty());
        result.mouseMiddleButtonPressed = pressed(
            mouse.getMiddleButtonProperty());
        result.gamePadA = pressed(
            gamePad.getButtonsProperty().getAProperty());
        result.gamePadB = pressed(
            gamePad.getButtonsProperty().getBProperty());
        result.gamePadLeftTrigger =
            gamePad.getTriggersProperty().getLeftProperty();
        result.gamePadRightTrigger =
            gamePad.getTriggersProperty().getRightProperty();
        result.gamePadDPadUp = pressed(
            gamePad.getDPadProperty().getUpProperty());
        result.gamePadDPadDown = pressed(
            gamePad.getDPadProperty().getDownProperty());
        result.keySpace = keyboard.IsKeyDown(Keys::Space);
        return result;
    }
}
