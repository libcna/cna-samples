// SPDX-License-Identifier: MS-PL

#include "ScreenManager/GameScreen.hpp"

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "ScreenManager/InputState.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace GameStateManagement
{
    using Microsoft::Xna::Framework::Input::Touch::GestureType;
    using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

    bool GameScreen::getIsPopupProperty() const { return isPopup_; }
    System::TimeSpan GameScreen::getTransitionOnTimeProperty() const { return transitionOnTime_; }
    System::TimeSpan GameScreen::getTransitionOffTimeProperty() const { return transitionOffTime_; }
    float GameScreen::getTransitionPositionProperty() const { return transitionPosition_; }
    float GameScreen::getTransitionAlphaProperty() const { return 1.0f - transitionPosition_; }
    ScreenState GameScreen::getScreenStateProperty() const { return screenState_; }
    bool GameScreen::getIsExitingProperty() const { return isExiting_; }
    void GameScreen::setIsExitingProperty(bool value) { isExiting_ = value; }

    bool GameScreen::getIsActiveProperty() const
    {
        return !otherScreenHasFocus_ &&
               (screenState_ == ScreenState::TransitionOn || screenState_ == ScreenState::Active);
    }

    ScreenManager& GameScreen::getScreenManagerProperty() const { return *screenManager_; }
    void GameScreen::setScreenManagerProperty(ScreenManager& value) { screenManager_ = &value; }
    std::optional<Microsoft::Xna::Framework::PlayerIndex>
    GameScreen::getControllingPlayerProperty() const { return controllingPlayer_; }
    void GameScreen::setControllingPlayerProperty(
        std::optional<Microsoft::Xna::Framework::PlayerIndex> value) { controllingPlayer_ = value; }
    GestureType GameScreen::getEnabledGesturesProperty() const { return enabledGestures_; }

    void GameScreen::setEnabledGesturesProperty(GestureType value)
    {
        enabledGestures_ = value;
        if (screenState_ == ScreenState::Active)
            TouchPanel::setEnabledGesturesProperty(value);
    }

    void GameScreen::LoadContent() {}
    void GameScreen::UnloadContent() {}

    void GameScreen::Update(Microsoft::Xna::Framework::GameTime& gameTime,
                            bool otherScreenHasFocus, bool coveredByOtherScreen)
    {
        otherScreenHasFocus_ = otherScreenHasFocus;
        if (isExiting_)
        {
            screenState_ = ScreenState::TransitionOff;
            if (!UpdateTransition(gameTime, transitionOffTime_, 1))
                screenManager_->RemoveScreen(this);
        }
        else if (coveredByOtherScreen)
        {
            screenState_ = UpdateTransition(gameTime, transitionOffTime_, 1)
                ? ScreenState::TransitionOff : ScreenState::Hidden;
        }
        else
        {
            screenState_ = UpdateTransition(gameTime, transitionOnTime_, -1)
                ? ScreenState::TransitionOn : ScreenState::Active;
        }
    }

    bool GameScreen::UpdateTransition(Microsoft::Xna::Framework::GameTime& gameTime,
                                      System::TimeSpan time, int direction)
    {
        const float transitionDelta = time == System::TimeSpan::Zero
            ? 1.0f
            : static_cast<float>(gameTime.getElapsedGameTimeProperty()
                .getTotalMillisecondsProperty() / time.getTotalMillisecondsProperty());
        transitionPosition_ += transitionDelta * static_cast<float>(direction);
        if ((direction < 0 && transitionPosition_ <= 0.0f) ||
            (direction > 0 && transitionPosition_ >= 1.0f))
        {
            transitionPosition_ = Microsoft::Xna::Framework::MathHelper::Clamp(
                transitionPosition_, 0.0f, 1.0f);
            return false;
        }
        return true;
    }

    void GameScreen::HandleInput(InputState&) {}
    void GameScreen::Draw(const Microsoft::Xna::Framework::GameTime&) {}

    void GameScreen::ExitScreen()
    {
        if (transitionOffTime_ == System::TimeSpan::Zero)
            screenManager_->RemoveScreen(this);
        else
            isExiting_ = true;
    }

    void GameScreen::setIsPopupProperty(bool value) { isPopup_ = value; }
    void GameScreen::setTransitionOnTimeProperty(System::TimeSpan value) { transitionOnTime_ = value; }
    void GameScreen::setTransitionOffTimeProperty(System::TimeSpan value) { transitionOffTime_ = value; }
}
