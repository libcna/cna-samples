// SPDX-License-Identifier: MS-PL
#pragma once

// InputManager.hpp — C++ port of ShipGame/InputManager.cs (XNA 4.0 Ship Game
// Starter Kit). Holds this frame's and last frame's pad/keyboard state so every
// query can be phrased as "pressed this frame" rather than "held".

#include <array>

#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::PlayerIndex;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Input::ButtonState;
using Microsoft::Xna::Framework::Input::GamePad;
using Microsoft::Xna::Framework::Input::GamePadState;
using Microsoft::Xna::Framework::Input::Keyboard;
using Microsoft::Xna::Framework::Input::KeyboardState;
using Microsoft::Xna::Framework::Input::Keys;

// Port of the InputState class in ShipGame/InputManager.cs.
class InputState {
public:
    std::array<GamePadState, 2> padState;
    std::array<KeyboardState, 2> keyState;

    InputState() { GetInput(false); }

    void GetInput(bool singlePlayer) {
        padState[0] = GamePad::GetState(PlayerIndex::One);
        padState[1] = GamePad::GetState(PlayerIndex::Two);
        if (singlePlayer)
            keyState[0] = Keyboard::GetState();
        else
            keyState[1] = Keyboard::GetState();
    }

    void CopyInput(const InputState& state) {
        padState[0] = state.padState[0];
        padState[1] = state.padState[1];
        keyState[0] = state.keyState[0];
        keyState[1] = state.keyState[1];
    }
};

// Port of ShipGame/InputManager.cs.
class InputManager {
public:
    // Create a new input manager
    InputManager() = default;

    // Begin input (aqruire input from all controlls)
    void BeginInputProcessing(bool singlePlayer) { currentState_.GetInput(singlePlayer); }

    // End input (save current input to last frame input)
    void EndInputProcessing() { lastState_.CopyInput(currentState_); }

    // Get the current input state
    const InputState& getCurrentState() const { return currentState_; }

    // Get last frame input state
    const InputState& getLastState() const { return lastState_; }

    // Check if a key is down in current frame for a given player
    bool IsKeyDown(int player, Keys key) const {
        return currentState_.keyState[player].IsKeyDown(key);
    }

    // Check if a key was pressed in this frame for a given player
    // (down in this frame and up in last frame)
    bool IsKeyPressed(int player, Keys key) const {
        return currentState_.keyState[player].IsKeyDown(key) &&
               lastState_.keyState[player].IsKeyUp(key);
    }

    // Return left stick position in a Vector2
    Vector2 LeftStick(int player) const {
        return currentState_.padState[player].getThumbSticksProperty().getLeftProperty();
    }

    // Return right stick position in a Vector2
    Vector2 RightStick(int player) const {
        return currentState_.padState[player].getThumbSticksProperty().getRightProperty();
    }

    // Check if left trigger was pressed in this frame for a given player
    // (positive this frame and zero in last frame)
    bool IsTriggerPressedLeft(int player) const {
        return currentState_.padState[player].getTriggersProperty().getLeftProperty() > 0 &&
               lastState_.padState[player].getTriggersProperty().getLeftProperty() == 0;
    }

    // Check if right trigger was pressed in this frame for a given player
    // (positive this frame and zero in last frame)
    bool IsTriggerPressedRigth(int player) const {
        return currentState_.padState[player].getTriggersProperty().getRightProperty() > 0 &&
               lastState_.padState[player].getTriggersProperty().getRightProperty() == 0;
    }

    // Check if back button was pressed in this frame for a given player
    // (down in this frame and up in last frame)
    bool IsButtonPressedBack(int player) const {
        return currentState_.padState[player].getButtonsProperty().getBackProperty() ==
                   ButtonState::Pressed &&
               lastState_.padState[player].getButtonsProperty().getBackProperty() ==
                   ButtonState::Released;
    }

    // Check if start button was pressed in this frame for a given player
    // (down in this frame and up in last frame)
    bool IsButtonPressedStart(int player) const {
        return currentState_.padState[player].getButtonsProperty().getStartProperty() ==
                   ButtonState::Pressed &&
               lastState_.padState[player].getButtonsProperty().getStartProperty() ==
                   ButtonState::Released;
    }

    // Check if dpad left button was pressed in this frame for a given player
    // (down in this frame and up in last frame)
    bool IsButtonPressedDPadLeft(int player) const {
        return currentState_.padState[player].getDPadProperty().getLeftProperty() ==
                   ButtonState::Pressed &&
               lastState_.padState[player].getDPadProperty().getLeftProperty() ==
                   ButtonState::Released;
    }

    // Check if dpad right button was pressed in this frame for a given player
    // (down in this frame and up in last frame)
    bool IsButtonPressedDPadRight(int player) const {
        return currentState_.padState[player].getDPadProperty().getRightProperty() ==
                   ButtonState::Pressed &&
               lastState_.padState[player].getDPadProperty().getRightProperty() ==
                   ButtonState::Released;
    }

    // Check if dpad up button was pressed in this frame for a given player
    // (down in this frame and up in last frame)
    bool IsButtonPressedDPadUp(int player) const {
        return currentState_.padState[player].getDPadProperty().getUpProperty() ==
                   ButtonState::Pressed &&
               lastState_.padState[player].getDPadProperty().getUpProperty() ==
                   ButtonState::Released;
    }

    // Check if dpad down button was pressed in this frame for a given player
    // (down in this frame and up in last frame)
    bool IsButtonPressedDPadDown(int player) const {
        return currentState_.padState[player].getDPadProperty().getDownProperty() ==
                   ButtonState::Pressed &&
               lastState_.padState[player].getDPadProperty().getDownProperty() ==
                   ButtonState::Released;
    }

    // Check if A button was pressed in this frame for a given player
    // (down in this frame and up in last frame)
    bool IsButtonPressedA(int player) const {
        return currentState_.padState[player].getButtonsProperty().getAProperty() ==
                   ButtonState::Pressed &&
               lastState_.padState[player].getButtonsProperty().getAProperty() ==
                   ButtonState::Released;
    }

    // Check if B button was pressed in this frame for a given player
    // (down in this frame and up in last frame)
    bool IsButtonPressedB(int player) const {
        return currentState_.padState[player].getButtonsProperty().getBProperty() ==
                   ButtonState::Pressed &&
               lastState_.padState[player].getButtonsProperty().getBProperty() ==
                   ButtonState::Released;
    }

    // Check if X button was pressed in this frame for a given player
    // (down in this frame and up in last frame)
    bool IsButtonPressedX(int player) const {
        return currentState_.padState[player].getButtonsProperty().getXProperty() ==
                   ButtonState::Pressed &&
               lastState_.padState[player].getButtonsProperty().getXProperty() ==
                   ButtonState::Released;
    }

    // Check if A button was pressed in this frame for a given player
    // (down in this frame and up in last frame)
    bool IsButtonPressedY(int player) const {
        return currentState_.padState[player].getButtonsProperty().getYProperty() ==
                   ButtonState::Pressed &&
               lastState_.padState[player].getButtonsProperty().getYProperty() ==
                   ButtonState::Released;
    }

    // Check if left shoulder button was pressed in this frame for a given player
    // (down in this frame and up in last frame)
    bool IsButtonPressedLeftShoulder(int player) const {
        return (currentState_.padState[player].getButtonsProperty().getLeftShoulderProperty() ==
                ButtonState::Pressed) &&
               (lastState_.padState[player].getButtonsProperty().getLeftShoulderProperty() ==
                ButtonState::Released);
    }

    // Check if right shoulder button was pressed in this frame for a given player
    // (down in this frame and up in last frame)
    bool IsButtonPressedRightShoulder(int player) const {
        return (currentState_.padState[player].getButtonsProperty().getRightShoulderProperty() ==
                ButtonState::Pressed) &&
               (lastState_.padState[player].getButtonsProperty().getRightShoulderProperty() ==
                ButtonState::Released);
    }

    // Check if left stick was pressed in this frame for a given player
    // (down in this frame and up in last frame)
    bool IsButtonPressedLeftStick(int player) const {
        return (currentState_.padState[player].getButtonsProperty().getLeftStickProperty() ==
                ButtonState::Pressed) &&
               (lastState_.padState[player].getButtonsProperty().getLeftStickProperty() ==
                ButtonState::Released);
    }

    // Check if right stick was pressed in this frame for a given player
    // (down in this frame and up in last frame)
    bool IsButtonPressedRightStick(int player) const {
        return (currentState_.padState[player].getButtonsProperty().getRightStickProperty() ==
                ButtonState::Pressed) &&
               (lastState_.padState[player].getButtonsProperty().getRightStickProperty() ==
                ButtonState::Released);
    }

    // Check left stick as a button for up press
    bool IsButtonPressedLeftStickUp(int player) const {
        return currentState_.padState[player].getThumbSticksProperty().getLeftProperty().Y > 0.5f &&
               lastState_.padState[player].getThumbSticksProperty().getLeftProperty().Y <= 0.5f;
    }

    // Check left stick as a button for down press
    bool IsButtonPressedLeftStickDown(int player) const {
        return currentState_.padState[player].getThumbSticksProperty().getLeftProperty().Y <
                   -0.5f &&
               lastState_.padState[player].getThumbSticksProperty().getLeftProperty().Y >= -0.5f;
    }

    // Check left stick as a button for left press
    bool IsButtonPressedLeftStickLeft(int player) const {
        return currentState_.padState[player].getThumbSticksProperty().getLeftProperty().X <
                   -0.5f &&
               lastState_.padState[player].getThumbSticksProperty().getLeftProperty().X >= -0.5f;
    }

    // Check left stick as a button for right press
    bool IsButtonPressedLeftStickRight(int player) const {
        return currentState_.padState[player].getThumbSticksProperty().getLeftProperty().X > 0.5f &&
               lastState_.padState[player].getThumbSticksProperty().getLeftProperty().X <= 0.5f;
    }

private:
    InputState currentState_; // current frame input
    InputState lastState_;    // last frame input
};

} // namespace ShipGame
