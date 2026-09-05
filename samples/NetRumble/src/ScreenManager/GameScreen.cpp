// SPDX-License-Identifier: MS-PL
#include "ScreenManager/GameScreen.hpp"

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace NetRumble {
bool GameScreen::getIsPopupProperty() const { return isPopup_; }
System::TimeSpan GameScreen::getTransitionOnTimeProperty() const {
  return transitionOnTime_;
}
System::TimeSpan GameScreen::getTransitionOffTimeProperty() const {
  return transitionOffTime_;
}
float GameScreen::getTransitionPositionProperty() const {
  return transitionPosition_;
}
SharpRuntime::bytecs GameScreen::getTransitionAlphaProperty() const {
  return static_cast<SharpRuntime::bytecs>(255.0f -
                                           transitionPosition_ * 255.0f);
}
ScreenState GameScreen::getScreenStateProperty() const { return screenState_; }
bool GameScreen::getIsExitingProperty() const { return isExiting_; }
bool GameScreen::getIsActiveProperty() const {
  return !otherScreenHasFocus_ && (screenState_ == ScreenState::TransitionOn ||
                                   screenState_ == ScreenState::Active);
}
ScreenManager &GameScreen::getScreenManagerProperty() const {
  return *screenManager_;
}
void GameScreen::setScreenManagerProperty(ScreenManager &value) {
  screenManager_ = &value;
}
void GameScreen::LoadContent() {}
void GameScreen::UnloadContent() {}
void GameScreen::HandleInput(InputState &) {}
void GameScreen::UpdatePresence() {}

void GameScreen::Update(Microsoft::Xna::Framework::GameTime &gameTime,
                        bool otherScreenHasFocus, bool coveredByOtherScreen) {
  otherScreenHasFocus_ = otherScreenHasFocus;
  if (isExiting_) {
    screenState_ = ScreenState::TransitionOff;
    if (!UpdateTransition(gameTime, transitionOffTime_, 1)) {
      screenManager_->RemoveScreen(this);
      isExiting_ = false;
    }
  } else if (coveredByOtherScreen) {
    screenState_ = UpdateTransition(gameTime, transitionOffTime_, 1)
                       ? ScreenState::TransitionOff
                       : ScreenState::Hidden;
  } else {
    screenState_ = UpdateTransition(gameTime, transitionOnTime_, -1)
                       ? ScreenState::TransitionOn
                       : ScreenState::Active;
  }
}

bool GameScreen::UpdateTransition(Microsoft::Xna::Framework::GameTime &gameTime,
                                  System::TimeSpan time, int direction) {
  const float delta =
      time == System::TimeSpan::Zero
          ? 1.0f
          : static_cast<float>(gameTime.getElapsedGameTimeProperty()
                                   .getTotalMillisecondsProperty() /
                               time.getTotalMillisecondsProperty());
  transitionPosition_ += delta * static_cast<float>(direction);
  if (transitionPosition_ <= 0.0f || transitionPosition_ >= 1.0f) {
    transitionPosition_ = Microsoft::Xna::Framework::MathHelper::Clamp(
        transitionPosition_, 0.0f, 1.0f);
    return false;
  }
  return true;
}

void GameScreen::ExitScreen() {
  if (transitionOffTime_ == System::TimeSpan::Zero)
    screenManager_->RemoveScreen(this);
  else
    isExiting_ = true;
}

void GameScreen::setIsPopupProperty(bool value) { isPopup_ = value; }
void GameScreen::setTransitionOnTimeProperty(System::TimeSpan value) {
  transitionOnTime_ = value;
}
void GameScreen::setTransitionOffTimeProperty(System::TimeSpan value) {
  transitionOffTime_ = value;
}
} // namespace NetRumble
