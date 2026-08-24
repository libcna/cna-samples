// SPDX-License-Identifier: MS-PL

#include "GamePadHelper.hpp"

#include "Settings.hpp"
#include "SpacewarGame.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Input;

    GamePadHelper::GamePadHelper(PlayerIndex player)
        : player_(player)
    {
        const Settings& settings = SpacewarGame::getSettingsProperty();
        if (player == PlayerIndex::One)
        {
            keyMapping_.Add(GamePadKeys::Start, settings.Player1Start);
            keyMapping_.Add(GamePadKeys::Back, settings.Player1Back);
            keyMapping_.Add(GamePadKeys::A, settings.Player1A);
            keyMapping_.Add(GamePadKeys::B, settings.Player1B);
            keyMapping_.Add(GamePadKeys::X, settings.Player1X);
            keyMapping_.Add(GamePadKeys::Y, settings.Player1Y);
            keyMapping_.Add(GamePadKeys::Up, settings.Player1Up);
            keyMapping_.Add(GamePadKeys::Down, settings.Player1Down);
            keyMapping_.Add(GamePadKeys::Left, settings.Player1Left);
            keyMapping_.Add(GamePadKeys::Right, settings.Player1Right);
            keyMapping_.Add(GamePadKeys::LeftTrigger, settings.Player1LeftTrigger);
            keyMapping_.Add(GamePadKeys::RightTrigger, settings.Player1RightTrigger);
            keyMapping_.Add(GamePadKeys::ThumbstickLeftXMin, settings.Player1ThumbstickLeftXmin);
            keyMapping_.Add(GamePadKeys::ThumbstickLeftXMax, settings.Player1ThumbstickLeftXmax);
            keyMapping_.Add(GamePadKeys::ThumbstickLeftYMin, settings.Player1ThumbstickLeftYmin);
            keyMapping_.Add(GamePadKeys::ThumbstickLeftYMax, settings.Player1ThumbstickLeftYmax);
        }
        else if (player == PlayerIndex::Two)
        {
            keyMapping_.Add(GamePadKeys::Start, settings.Player2Start);
            keyMapping_.Add(GamePadKeys::Back, settings.Player2Back);
            keyMapping_.Add(GamePadKeys::A, settings.Player2A);
            keyMapping_.Add(GamePadKeys::B, settings.Player2B);
            keyMapping_.Add(GamePadKeys::X, settings.Player2X);
            keyMapping_.Add(GamePadKeys::Y, settings.Player2Y);
            keyMapping_.Add(GamePadKeys::Up, settings.Player2Up);
            keyMapping_.Add(GamePadKeys::Down, settings.Player2Down);
            keyMapping_.Add(GamePadKeys::Left, settings.Player2Left);
            keyMapping_.Add(GamePadKeys::Right, settings.Player2Right);
            keyMapping_.Add(GamePadKeys::LeftTrigger, settings.Player2LeftTrigger);
            keyMapping_.Add(GamePadKeys::RightTrigger, settings.Player2RightTrigger);
            keyMapping_.Add(GamePadKeys::ThumbstickLeftXMin, settings.Player2ThumbstickLeftXmin);
            keyMapping_.Add(GamePadKeys::ThumbstickLeftXMax, settings.Player2ThumbstickLeftXmax);
            keyMapping_.Add(GamePadKeys::ThumbstickLeftYMin, settings.Player2ThumbstickLeftYmin);
            keyMapping_.Add(GamePadKeys::ThumbstickLeftYMax, settings.Player2ThumbstickLeftYmax);
        }
    }

    const GamePadState& GamePadHelper::getStateProperty() const { return state_; }

    float GamePadHelper::getThumbStickLeftXProperty() const
    {
        float result = 0.0f;
        if (game_ != nullptr && game_->getIsActiveProperty())
        {
            if (state_.getIsConnectedProperty()) result = state_.getThumbSticksProperty().getLeftProperty().X;
            if (keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::ThumbstickLeftXMin))) result = -1.0f;
            if (keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::ThumbstickLeftXMax))) result = 1.0f;
        }
        return result;
    }

    float GamePadHelper::getThumbStickLeftYProperty() const
    {
        float result = 0.0f;
        if (game_ != nullptr && game_->getIsActiveProperty())
        {
            if (state_.getIsConnectedProperty()) result = state_.getThumbSticksProperty().getLeftProperty().Y;
            if (keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::ThumbstickLeftYMin))) result = -1.0f;
            if (keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::ThumbstickLeftYMax))) result = 1.0f;
        }
        return result;
    }

    float GamePadHelper::getThumbStickRightXProperty() const
    {
        return game_ != nullptr && game_->getIsActiveProperty() && state_.getIsConnectedProperty()
            ? state_.getThumbSticksProperty().getRightProperty().X : 0.0f;
    }

    float GamePadHelper::getThumbStickRightYProperty() const
    {
        return game_ != nullptr && game_->getIsActiveProperty() && state_.getIsConnectedProperty()
            ? state_.getThumbSticksProperty().getRightProperty().Y : 0.0f;
    }

    bool GamePadHelper::CheckPressed(ButtonState state, bool& released)
    {
        return CheckPressed(state == ButtonState::Pressed, released);
    }

    bool GamePadHelper::CheckPressed(float state, bool& released)
    {
        return CheckPressed(state > 0.0f, released);
    }

    bool GamePadHelper::CheckPressed(bool pressed, bool& released)
    {
        const bool result = released && pressed;
        if (game_ == nullptr || !game_->getIsActiveProperty()) return false;
        if (result) released = false;
        return result;
    }

    bool GamePadHelper::getLeftTriggerPressedProperty()
    {
        return CheckPressed(state_.getTriggersProperty().getLeftProperty(), leftTriggerWasReleased_) ||
               CheckPressed(keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::LeftTrigger)), kbLeftTriggerWasReleased_);
    }
    bool GamePadHelper::getRightTriggerPressedProperty()
    {
        return CheckPressed(state_.getTriggersProperty().getRightProperty(), rightTriggerWasReleased_) ||
               CheckPressed(keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::RightTrigger)), kbRightTriggerWasReleased_);
    }
    bool GamePadHelper::getAPressedProperty()
    {
        return CheckPressed(state_.getButtonsProperty().getAProperty(), aWasReleased_) ||
               CheckPressed(keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::A)), kbAWasReleased_);
    }
    bool GamePadHelper::getBPressedProperty()
    {
        return CheckPressed(state_.getButtonsProperty().getBProperty(), bWasReleased_) ||
               CheckPressed(keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::B)), kbBWasReleased_);
    }
    bool GamePadHelper::getYPressedProperty()
    {
        return CheckPressed(state_.getButtonsProperty().getYProperty(), yWasReleased_) ||
               CheckPressed(keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::Y)), kbYWasReleased_);
    }
    bool GamePadHelper::getXPressedProperty()
    {
        return CheckPressed(state_.getButtonsProperty().getXProperty(), xWasReleased_) ||
               CheckPressed(keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::X)), kbXWasReleased_);
    }
    bool GamePadHelper::getStartPressedProperty()
    {
        return CheckPressed(state_.getButtonsProperty().getStartProperty(), startWasReleased_) ||
               CheckPressed(keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::Start)), kbStartWasReleased_);
    }
    bool GamePadHelper::getBackPressedProperty()
    {
        return CheckPressed(state_.getButtonsProperty().getBackProperty(), backWasReleased_) ||
               CheckPressed(keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::Back)), kbBackWasReleased_);
    }
    bool GamePadHelper::getUpPressedProperty()
    {
        return CheckPressed(state_.getDPadProperty().getUpProperty(), upWasReleased_) ||
               CheckPressed(keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::Up)), kbUpWasReleased_);
    }
    bool GamePadHelper::getDownPressedProperty()
    {
        return CheckPressed(state_.getDPadProperty().getDownProperty(), downWasReleased_) ||
               CheckPressed(keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::Down)), kbDownWasReleased_);
    }
    bool GamePadHelper::getLeftPressedProperty()
    {
        return CheckPressed(state_.getDPadProperty().getLeftProperty(), leftWasReleased_) ||
               CheckPressed(keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::Left)), kbLeftWasReleased_);
    }
    bool GamePadHelper::getRightPressedProperty()
    {
        return CheckPressed(state_.getDPadProperty().getRightProperty(), rightWasReleased_) ||
               CheckPressed(keyState_.IsKeyDown(keyMapping_.Get(GamePadKeys::Right)), kbRightWasReleased_);
    }

    void GamePadHelper::Update(Game& game, const KeyboardState& keyState)
    {
        state_ = GamePad::GetState(player_);
        keyState_ = keyState;
        game_ = &game;

        const Settings& settings = SpacewarGame::getSettingsProperty();
        const bool playerOne = player_ == PlayerIndex::One;
        const auto released = [&](Keys first, Keys second)
        {
            return keyState.IsKeyUp(playerOne ? first : second);
        };
        if (released(settings.Player1A, settings.Player2A)) kbAWasReleased_ = true;
        if (released(settings.Player1B, settings.Player2B)) kbBWasReleased_ = true;
        if (released(settings.Player1Y, settings.Player2Y)) kbYWasReleased_ = true;
        if (released(settings.Player1X, settings.Player2X)) kbXWasReleased_ = true;
        if (released(settings.Player1Start, settings.Player2Start)) kbStartWasReleased_ = true;
        if (released(settings.Player1Back, settings.Player2Back)) kbBackWasReleased_ = true;
        if (released(settings.Player1Up, settings.Player2Up)) kbUpWasReleased_ = true;
        if (released(settings.Player1Down, settings.Player2Down)) kbDownWasReleased_ = true;
        if (released(settings.Player1Left, settings.Player2Left)) kbLeftWasReleased_ = true;
        if (released(settings.Player1Right, settings.Player2Right)) kbRightWasReleased_ = true;
        if (released(settings.Player1LeftTrigger, settings.Player2LeftTrigger)) kbLeftTriggerWasReleased_ = true;
        if (released(settings.Player1RightTrigger, settings.Player2RightTrigger)) kbRightTriggerWasReleased_ = true;

        if (state_.getIsConnectedProperty())
        {
            const auto& buttons = state_.getButtonsProperty();
            const auto& dpad = state_.getDPadProperty();
            if (buttons.getAProperty() == ButtonState::Released) aWasReleased_ = true;
            if (buttons.getBProperty() == ButtonState::Released) bWasReleased_ = true;
            if (buttons.getYProperty() == ButtonState::Released) yWasReleased_ = true;
            if (buttons.getXProperty() == ButtonState::Released) xWasReleased_ = true;
            if (buttons.getStartProperty() == ButtonState::Released) startWasReleased_ = true;
            if (buttons.getBackProperty() == ButtonState::Released) backWasReleased_ = true;
            if (dpad.getUpProperty() == ButtonState::Released) upWasReleased_ = true;
            if (dpad.getDownProperty() == ButtonState::Released) downWasReleased_ = true;
            if (dpad.getLeftProperty() == ButtonState::Released) leftWasReleased_ = true;
            if (dpad.getRightProperty() == ButtonState::Released) rightWasReleased_ = true;
            if (state_.getTriggersProperty().getLeftProperty() == 0.0f) leftTriggerWasReleased_ = true;
            if (state_.getTriggersProperty().getRightProperty() == 0.0f) rightTriggerWasReleased_ = true;
        }
    }
}
