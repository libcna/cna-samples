// SPDX-License-Identifier: MS-PL

#include "ScreenManager/InputState.hpp"

#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"

namespace GameStateManagement
{
    using Microsoft::Xna::Framework::PlayerIndex;
    using namespace Microsoft::Xna::Framework::Input;
    using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

    void InputState::Update()
    {
        for (int i = 0; i < MaxInputs; ++i)
        {
            LastKeyboardStates[i] = CurrentKeyboardStates[i];
            LastGamePadStates[i] = CurrentGamePadStates[i];
            CurrentKeyboardStates[i] = Keyboard::GetState(static_cast<PlayerIndex>(i));
            CurrentGamePadStates[i] = GamePad::GetState(static_cast<PlayerIndex>(i));
            if (CurrentGamePadStates[i].getIsConnectedProperty())
                GamePadWasConnected[i] = true;
        }
        TouchState = TouchPanel::GetState();
        Gestures.clear();
        while (TouchPanel::getIsGestureAvailableProperty())
            Gestures.push_back(TouchPanel::ReadGesture());
    }

    bool InputState::IsNewKeyPress(Keys key, std::optional<PlayerIndex> controllingPlayer,
                                   PlayerIndex& playerIndex)
    {
        if (controllingPlayer.has_value())
        {
            playerIndex = controllingPlayer.value();
            const int i = static_cast<int>(playerIndex);
            return CurrentKeyboardStates[i].IsKeyDown(key) && LastKeyboardStates[i].IsKeyUp(key);
        }
        return IsNewKeyPress(key, PlayerIndex::One, playerIndex) ||
               IsNewKeyPress(key, PlayerIndex::Two, playerIndex) ||
               IsNewKeyPress(key, PlayerIndex::Three, playerIndex) ||
               IsNewKeyPress(key, PlayerIndex::Four, playerIndex);
    }

    bool InputState::IsNewButtonPress(Buttons button, std::optional<PlayerIndex> controllingPlayer,
                                      PlayerIndex& playerIndex)
    {
        if (controllingPlayer.has_value())
        {
            playerIndex = controllingPlayer.value();
            const int i = static_cast<int>(playerIndex);
            return CurrentGamePadStates[i].IsButtonDown(button) &&
                   LastGamePadStates[i].IsButtonUp(button);
        }
        return IsNewButtonPress(button, PlayerIndex::One, playerIndex) ||
               IsNewButtonPress(button, PlayerIndex::Two, playerIndex) ||
               IsNewButtonPress(button, PlayerIndex::Three, playerIndex) ||
               IsNewButtonPress(button, PlayerIndex::Four, playerIndex);
    }

    bool InputState::IsMenuSelect(std::optional<PlayerIndex> controllingPlayer,
                                  PlayerIndex& playerIndex)
    {
        return IsNewKeyPress(Keys::Space, controllingPlayer, playerIndex) ||
               IsNewKeyPress(Keys::Enter, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::A, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::Start, controllingPlayer, playerIndex);
    }

    bool InputState::IsMenuCancel(std::optional<PlayerIndex> controllingPlayer,
                                  PlayerIndex& playerIndex)
    {
        return IsNewKeyPress(Keys::Escape, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::B, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::Back, controllingPlayer, playerIndex);
    }

    bool InputState::IsMenuUp(std::optional<PlayerIndex> controllingPlayer)
    {
        PlayerIndex playerIndex;
        return IsNewKeyPress(Keys::Up, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::DPadUp, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::LeftThumbstickUp, controllingPlayer, playerIndex);
    }

    bool InputState::IsMenuDown(std::optional<PlayerIndex> controllingPlayer)
    {
        PlayerIndex playerIndex;
        return IsNewKeyPress(Keys::Down, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::DPadDown, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::LeftThumbstickDown, controllingPlayer, playerIndex);
    }

    bool InputState::IsPauseGame(std::optional<PlayerIndex> controllingPlayer)
    {
        PlayerIndex playerIndex;
        return IsNewKeyPress(Keys::Escape, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::Back, controllingPlayer, playerIndex) ||
               IsNewButtonPress(Buttons::Start, controllingPlayer, playerIndex);
    }
}
