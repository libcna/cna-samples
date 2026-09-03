// SPDX-License-Identifier: MS-PL

#include "GameLogic/Input.hpp"

#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

namespace RacingGame::GameLogic
{
    using Microsoft::Xna::Framework::PlayerIndex;
    using namespace Microsoft::Xna::Framework::Input;

    Input::Input() = default;

    ControlFrame Input::Capture(
        const bool inGame, const bool appActive,
        const int displayWidth, const int displayHeight)
    {
        const MouseState mouseStateLastFrame = mouseState;
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

        if (inGame && appActive)
        {
            Mouse::SetPosition(displayWidth / 2, displayHeight / 2);
            mouseState = Mouse::GetState();
        }

        const KeyboardState keyboard = Keyboard::GetState();
        const GamePadState gamePad = GamePad::GetState(PlayerIndex::One);
        ControlFrame result;
        result.car = CarControlState::FromXnaInput(
            keyboard, mouseState, mouseXMovement, mouseYMovement,
            mouseWheelDelta, gamePad);
        result.exitRequested = keyboard.IsKeyDown(Keys::Escape) ||
            gamePad.getButtonsProperty().getBackProperty() ==
                ButtonState::Pressed;
        return result;
    }
}
