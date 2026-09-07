#pragma once

// InputManager.hpp -- C++ port of InputManager.cs. This class handles all
// keyboard and gamepad actions in the game.
//
// CNA unifies GamePad face/shoulder buttons and D-pad directions into a
// single Buttons flags enum (checked via GamePadState::IsButtonDown(Buttons)),
// unlike the original's separate GamePadState.Buttons/.DPad structs -- this
// port's GamePadButtons enum below maps onto that single CNA enum instead of
// mirroring the original's two-struct split (see missing.md).
//
// KeyName() replaces C#'s Keys.ToString(), which the controls chart prints; CNA has no enum
// name table, so the sample carries its own, exactly as Spacewar's Settings.cpp does.

#include <array>
#include <cmath>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Input::ButtonState;
using Microsoft::Xna::Framework::Input::Buttons;
using Microsoft::Xna::Framework::Input::GamePad;
using Microsoft::Xna::Framework::Input::GamePadState;
using Microsoft::Xna::Framework::Input::Keyboard;
using Microsoft::Xna::Framework::Input::KeyboardState;
using Microsoft::Xna::Framework::Input::Keys;
using Microsoft::Xna::Framework::PlayerIndex;

class InputManager {
public:
    enum class Action {
        MainMenu, Ok, Back, CharacterManagement, ExitGame, TakeView, DropUnEquip,
        MoveCharacterUp, MoveCharacterDown, MoveCharacterLeft, MoveCharacterRight,
        CursorUp, CursorDown, DecreaseAmount, IncreaseAmount, PageLeft, PageRight,
        TargetUp, TargetDown, ActiveCharacterLeft, ActiveCharacterRight,
        TotalActionCount,
    };

    static std::string GetActionName(Action action) {
        static const char* names[] = {
            "Main Menu", "Ok", "Back", "Character Management", "Exit Game", "Take / View",
            "Drop / Unequip", "Move Character - Up", "Move Character - Down",
            "Move Character - Left", "Move Character - Right", "Move Cursor - Up",
            "Move Cursor - Down", "Decrease Amount", "Increase Amount", "Page Screen Left",
            "Page Screen Right", "Select Target -Up", "Select Target - Down",
            "Select Active Character - Left", "Select Active Character - Right",
        };
        return names[(int)action];
    }

    enum class GamePadButtons { Start, Back, A, B, X, Y, Up, Down, Left, Right, LeftShoulder, RightShoulder, LeftTrigger, RightTrigger };

    // A combinable set of gamepad buttons and keyboard keys, mapped to one action.
    struct ActionMap {
        // List of GamePad controls to be mapped to a given action.
        std::vector<GamePadButtons> gamePadButtons;

        // List of Keyboard controls to be mapped to a given action.
        std::vector<Keys> keyboardKeys;
    };

    // The action mappings for the game.
    static const std::vector<ActionMap>& ActionMaps() {
        if (actionMaps_.empty()) ResetActionMaps();
        return actionMaps_;
    }

    // The name C# prints for a Keys value; the controls chart shows one per mapped key.
    static std::string KeyName(Keys key) {
        switch (key) {
        case Keys::Tab: return "Tab";
        case Keys::Enter: return "Enter";
        case Keys::Escape: return "Escape";
        case Keys::Space: return "Space";
        case Keys::LeftControl: return "LeftControl";
        case Keys::RightControl: return "RightControl";
        case Keys::LeftShift: return "LeftShift";
        case Keys::RightShift: return "RightShift";
        case Keys::Up: return "Up";
        case Keys::Down: return "Down";
        case Keys::Left: return "Left";
        case Keys::Right: return "Right";
        case Keys::D: return "D";
        case Keys::None: return "None";
        default: return "None";
        }
    }

    static bool IsKeyPressed(Keys key) { return currentKeyboardState_.IsKeyDown(key); }
    static bool IsKeyTriggered(Keys key) {
        return currentKeyboardState_.IsKeyDown(key) && !previousKeyboardState_.IsKeyDown(key);
    }

    // The state of the keyboard as of the last update.
    static const KeyboardState& CurrentKeyboardState() { return currentKeyboardState_; }

    // The state of the gamepad as of the last update.
    static const GamePadState& CurrentGamePadState() { return currentGamePadState_; }

    // -- gamepad convenience wrappers, one per mapped control, as the original has ------------

    static bool IsGamePadStartPressed() { return IsGamePadButtonPressed(GamePadButtons::Start); }
    static bool IsGamePadBackPressed() { return IsGamePadButtonPressed(GamePadButtons::Back); }
    static bool IsGamePadAPressed() { return IsGamePadButtonPressed(GamePadButtons::A); }
    static bool IsGamePadBPressed() { return IsGamePadButtonPressed(GamePadButtons::B); }
    static bool IsGamePadXPressed() { return IsGamePadButtonPressed(GamePadButtons::X); }
    static bool IsGamePadYPressed() { return IsGamePadButtonPressed(GamePadButtons::Y); }
    static bool IsGamePadLeftShoulderPressed() {
        return IsGamePadButtonPressed(GamePadButtons::LeftShoulder);
    }
    static bool IsGamePadRightShoulderPressed() {
        return IsGamePadButtonPressed(GamePadButtons::RightShoulder);
    }
    static bool IsGamePadDPadUpPressed() { return IsGamePadButtonPressed(GamePadButtons::Up); }
    static bool IsGamePadDPadDownPressed() { return IsGamePadButtonPressed(GamePadButtons::Down); }
    static bool IsGamePadDPadLeftPressed() { return IsGamePadButtonPressed(GamePadButtons::Left); }
    static bool IsGamePadDPadRightPressed() {
        return IsGamePadButtonPressed(GamePadButtons::Right);
    }
    static bool IsGamePadLeftTriggerPressed() {
        return IsGamePadButtonPressed(GamePadButtons::LeftTrigger);
    }
    static bool IsGamePadRightTriggerPressed() {
        return IsGamePadButtonPressed(GamePadButtons::RightTrigger);
    }
    static bool IsGamePadLeftStickUpPressed() { return IsGamePadButtonPressed(GamePadButtons::Up); }
    static bool IsGamePadLeftStickDownPressed() {
        return IsGamePadButtonPressed(GamePadButtons::Down);
    }
    static bool IsGamePadLeftStickLeftPressed() {
        return IsGamePadButtonPressed(GamePadButtons::Left);
    }
    static bool IsGamePadLeftStickRightPressed() {
        return IsGamePadButtonPressed(GamePadButtons::Right);
    }

    static bool IsGamePadStartTriggered() {
        return IsGamePadButtonTriggered(GamePadButtons::Start);
    }
    static bool IsGamePadBackTriggered() { return IsGamePadButtonTriggered(GamePadButtons::Back); }
    static bool IsGamePadATriggered() { return IsGamePadButtonTriggered(GamePadButtons::A); }
    static bool IsGamePadBTriggered() { return IsGamePadButtonTriggered(GamePadButtons::B); }
    static bool IsGamePadXTriggered() { return IsGamePadButtonTriggered(GamePadButtons::X); }
    static bool IsGamePadYTriggered() { return IsGamePadButtonTriggered(GamePadButtons::Y); }
    static bool IsGamePadLeftShoulderTriggered() {
        return IsGamePadButtonTriggered(GamePadButtons::LeftShoulder);
    }
    static bool IsGamePadRightShoulderTriggered() {
        return IsGamePadButtonTriggered(GamePadButtons::RightShoulder);
    }
    static bool IsGamePadDPadUpTriggered() { return IsGamePadButtonTriggered(GamePadButtons::Up); }
    static bool IsGamePadDPadDownTriggered() {
        return IsGamePadButtonTriggered(GamePadButtons::Down);
    }
    static bool IsGamePadDPadLeftTriggered() {
        return IsGamePadButtonTriggered(GamePadButtons::Left);
    }
    static bool IsGamePadDPadRightTriggered() {
        return IsGamePadButtonTriggered(GamePadButtons::Right);
    }
    static bool IsGamePadLeftTriggerTriggered() {
        return IsGamePadButtonTriggered(GamePadButtons::LeftTrigger);
    }
    static bool IsGamePadRightTriggerTriggered() {
        return IsGamePadButtonTriggered(GamePadButtons::RightTrigger);
    }
    static bool IsGamePadLeftStickUpTriggered() {
        return IsGamePadButtonTriggered(GamePadButtons::Up);
    }
    static bool IsGamePadLeftStickDownTriggered() {
        return IsGamePadButtonTriggered(GamePadButtons::Down);
    }
    static bool IsGamePadLeftStickLeftTriggered() {
        return IsGamePadButtonTriggered(GamePadButtons::Left);
    }
    static bool IsGamePadLeftStickRightTriggered() {
        return IsGamePadButtonTriggered(GamePadButtons::Right);
    }

    static bool IsActionPressed(Action action) { return IsActionMapPressed(action); }
    static bool IsActionTriggered(Action action) { return IsActionMapTriggered(action); }

    static void Initialize() { ResetActionMaps(); }

    static void Update() {
        previousKeyboardState_ = currentKeyboardState_;
        currentKeyboardState_ = Keyboard::GetState();
        previousGamePadState_ = currentGamePadState_;
        currentGamePadState_ = GamePad::GetState(PlayerIndex::One);
    }

private:
    static constexpr float AnalogLimit = 0.5f;

    static bool IsGamePadButtonDown(GamePadButtons key, const GamePadState& state) {
        switch (key) {
            case GamePadButtons::Start: return state.IsButtonDown(Buttons::Start);
            case GamePadButtons::Back: return state.IsButtonDown(Buttons::Back);
            case GamePadButtons::A: return state.IsButtonDown(Buttons::A);
            case GamePadButtons::B: return state.IsButtonDown(Buttons::B);
            case GamePadButtons::X: return state.IsButtonDown(Buttons::X);
            case GamePadButtons::Y: return state.IsButtonDown(Buttons::Y);
            case GamePadButtons::LeftShoulder: return state.IsButtonDown(Buttons::LeftShoulder);
            case GamePadButtons::RightShoulder: return state.IsButtonDown(Buttons::RightShoulder);
            case GamePadButtons::LeftTrigger: return state.getTriggersProperty().getLeftProperty() > AnalogLimit;
            case GamePadButtons::RightTrigger: return state.getTriggersProperty().getRightProperty() > AnalogLimit;
            case GamePadButtons::Up:
                return state.IsButtonDown(Buttons::DPadUp) || state.getThumbSticksProperty().getLeftProperty().Y > AnalogLimit;
            case GamePadButtons::Down:
                return state.IsButtonDown(Buttons::DPadDown) || -state.getThumbSticksProperty().getLeftProperty().Y > AnalogLimit;
            case GamePadButtons::Left:
                return state.IsButtonDown(Buttons::DPadLeft) || -state.getThumbSticksProperty().getLeftProperty().X > AnalogLimit;
            case GamePadButtons::Right:
                return state.IsButtonDown(Buttons::DPadRight) || state.getThumbSticksProperty().getLeftProperty().X > AnalogLimit;
        }
        return false;
    }

    static bool IsGamePadButtonPressed(GamePadButtons key) {
        return IsGamePadButtonDown(key, currentGamePadState_);
    }

    static bool IsGamePadButtonTriggered(GamePadButtons key) {
        return IsGamePadButtonDown(key, currentGamePadState_) && !IsGamePadButtonDown(key, previousGamePadState_);
    }

    // Reset the action maps to their default values.
    static void ResetActionMaps() {
        actionMaps_.assign((std::size_t)Action::TotalActionCount, ActionMap());

        Map(Action::MainMenu, Keys::Tab, GamePadButtons::Start);
        Map(Action::Ok, Keys::Enter, GamePadButtons::A);
        Map(Action::Back, Keys::Escape, GamePadButtons::B);
        Map(Action::CharacterManagement, Keys::Space, GamePadButtons::Y);
        Map(Action::ExitGame, Keys::Escape, GamePadButtons::Back);
        Map(Action::TakeView, Keys::LeftControl, GamePadButtons::Y);
        Map(Action::DropUnEquip, Keys::D, GamePadButtons::X);
        Map(Action::MoveCharacterUp, Keys::Up, GamePadButtons::Up);
        Map(Action::MoveCharacterDown, Keys::Down, GamePadButtons::Down);
        Map(Action::MoveCharacterLeft, Keys::Left, GamePadButtons::Left);
        Map(Action::MoveCharacterRight, Keys::Right, GamePadButtons::Right);
        Map(Action::CursorUp, Keys::Up, GamePadButtons::Up);
        Map(Action::CursorDown, Keys::Down, GamePadButtons::Down);
        Map(Action::DecreaseAmount, Keys::Left, GamePadButtons::Left);
        Map(Action::IncreaseAmount, Keys::Right, GamePadButtons::Right);
        Map(Action::PageLeft, Keys::LeftShift, GamePadButtons::LeftTrigger);
        Map(Action::PageRight, Keys::RightShift, GamePadButtons::RightTrigger);
        Map(Action::TargetUp, Keys::Up, GamePadButtons::Up);
        Map(Action::TargetDown, Keys::Down, GamePadButtons::Down);
        Map(Action::ActiveCharacterLeft, Keys::Left, GamePadButtons::Left);
        Map(Action::ActiveCharacterRight, Keys::Right, GamePadButtons::Right);
    }

    static void Map(Action action, Keys key, GamePadButtons button) {
        actionMaps_[(std::size_t)action].keyboardKeys.push_back(key);
        actionMaps_[(std::size_t)action].gamePadButtons.push_back(button);
    }

    static bool IsActionMapPressed(Action action) {
        const ActionMap& actionMap = ActionMaps()[(std::size_t)action];
        for (Keys key : actionMap.keyboardKeys) {
            if (IsKeyPressed(key)) return true;
        }
        if (currentGamePadState_.getIsConnectedProperty()) {
            for (GamePadButtons button : actionMap.gamePadButtons) {
                if (IsGamePadButtonDown(button, currentGamePadState_)) return true;
            }
        }
        return false;
    }
    static bool IsActionMapTriggered(Action action) {
        const ActionMap& actionMap = ActionMaps()[(std::size_t)action];
        for (Keys key : actionMap.keyboardKeys) {
            if (IsKeyTriggered(key)) return true;
        }
        if (currentGamePadState_.getIsConnectedProperty()) {
            for (GamePadButtons button : actionMap.gamePadButtons) {
                if (IsGamePadButtonTriggered(button)) return true;
            }
        }
        return false;
    }

    static inline std::vector<ActionMap> actionMaps_;

    static inline KeyboardState currentKeyboardState_{};
    static inline KeyboardState previousKeyboardState_{};
    static inline GamePadState currentGamePadState_{};
    static inline GamePadState previousGamePadState_{};
};

} // namespace RolePlaying
