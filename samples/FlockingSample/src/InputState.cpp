// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// InputState.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "InputState.hpp"

#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"

namespace Flocking
{
    using Microsoft::Xna::Framework::PlayerIndex;

    float InputState::getMoveCatYProperty() const
    {
        if (CurrentKeyState.IsKeyDown(Keys::W))
        {
            return -1.0f;
        }
        else if (CurrentKeyState.IsKeyDown(Keys::S))
        {
            return 1.0f;
        }
        else
        {
            // negative = move up
            return -(CurrentPadState.getThumbSticksProperty().getLeftProperty().Y);
        }
    }

    float InputState::getMoveCatXProperty() const
    {
        if (CurrentKeyState.IsKeyDown(Keys::A))
        {
            return -1.0f;
        }
        else if (CurrentKeyState.IsKeyDown(Keys::D))
        {
            return 1.0f;
        }
        else
        {
            return CurrentPadState.getThumbSticksProperty().getLeftProperty().X;
        }
    }

    float InputState::getSliderMoveProperty() const
    {
        if (CurrentKeyState.IsKeyDown(Keys::Left) ||
            CurrentPadState.IsButtonDown(Buttons::DPadLeft))
        {
            return -1.0f;
        }
        else if (CurrentKeyState.IsKeyDown(Keys::Right) ||
            CurrentPadState.IsButtonDown(Buttons::DPadRight))
        {
            return 1.0f;
        }
        return -CurrentPadState.getTriggersProperty().getLeftProperty() +
                CurrentPadState.getTriggersProperty().getRightProperty();
    }

    bool InputState::getExitProperty() const
    {
        return IsNewKeyPress(Keys::Escape) ||
               (CurrentPadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed &&
                LastPadState.getButtonsProperty().getBackProperty() == ButtonState::Released);
    }

    bool InputState::getResetDistancesProperty() const
    {
        return IsNewKeyPress(Keys::B) ||
               (CurrentPadState.getButtonsProperty().getBProperty() == ButtonState::Pressed &&
                LastPadState.getButtonsProperty().getBProperty() == ButtonState::Released);
    }

    bool InputState::getResetFlockProperty() const
    {
        return IsNewKeyPress(Keys::X) ||
               (CurrentPadState.getButtonsProperty().getXProperty() == ButtonState::Pressed &&
                LastPadState.getButtonsProperty().getXProperty() == ButtonState::Released);
    }

    bool InputState::getUpProperty() const
    {
        return IsNewKeyPress(Keys::Up) ||
               (CurrentPadState.getDPadProperty().getUpProperty() == ButtonState::Pressed &&
                LastPadState.getDPadProperty().getUpProperty() == ButtonState::Released);
    }

    bool InputState::getDownProperty() const
    {
        return IsNewKeyPress(Keys::Down) ||
               (CurrentPadState.getDPadProperty().getDownProperty() == ButtonState::Pressed &&
                LastPadState.getDPadProperty().getDownProperty() == ButtonState::Released);
    }

    bool InputState::getToggleCatButtonProperty() const
    {
        return IsNewKeyPress(Keys::Y) ||
               (CurrentPadState.getButtonsProperty().getYProperty() == ButtonState::Pressed &&
                LastPadState.getButtonsProperty().getYProperty() == ButtonState::Released);
    }

    void InputState::Update()
    {
        LastKeyState = CurrentKeyState;
        LastPadState = CurrentPadState;
        CurrentKeyState = Keyboard::GetState();
        CurrentPadState = GamePad::GetState(PlayerIndex::One);
    }

    bool InputState::IsNewKeyPress(Keys key) const
    {
        return (CurrentKeyState.IsKeyDown(key) &&
                LastKeyState.IsKeyUp(key));
    }
}
