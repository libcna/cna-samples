// SPDX-License-Identifier: MS-PL
#pragma once

#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/Object.hpp"
#include "System/TimeSpan.hpp"

namespace NetRumble {
class InputState;
class ScreenManager;

/** @brief Describes the transition state of a game screen. */
enum class ScreenState {
  /** @brief The screen is becoming visible. */
  TransitionOn,
  /** @brief The screen is fully visible and active. */
  Active,
  /** @brief The screen is becoming hidden. */
  TransitionOff,
  /** @brief The screen is fully hidden. */
  Hidden
};

/** @brief Base class for one screen managed by NetRumble's screen stack. */
class GameScreen : public System::Object {
public:
  /** @brief Destroys the screen. */
  ~GameScreen() override = default;
  /** @brief Gets whether this screen is a popup. @return True for a popup. */
  [[nodiscard]] bool getIsPopupProperty() const;
  /** @brief Gets transition-on duration. @return Transition-on duration. */
  [[nodiscard]] System::TimeSpan getTransitionOnTimeProperty() const;
  /** @brief Gets transition-off duration. @return Transition-off duration. */
  [[nodiscard]] System::TimeSpan getTransitionOffTimeProperty() const;
  /** @brief Gets transition progress. @return Value between zero and one. */
  [[nodiscard]] float getTransitionPositionProperty() const;
  /** @brief Gets transition opacity. @return Alpha from zero through 255. */
  [[nodiscard]] SharpRuntime::bytecs getTransitionAlphaProperty() const;
  /** @brief Gets current state. @return Current screen state. */
  [[nodiscard]] ScreenState getScreenStateProperty() const;
  /** @brief Gets whether the screen is exiting. @return True while exiting. */
  [[nodiscard]] bool getIsExitingProperty() const;
  /** @brief Gets whether the screen can receive input. @return True when
   * active. */
  [[nodiscard]] bool getIsActiveProperty() const;
  /** @brief Gets the owning manager. @return Owning manager. */
  [[nodiscard]] ScreenManager &getScreenManagerProperty() const;
  /** @brief Sets the owning manager. @param value Owning manager. */
  void setScreenManagerProperty(ScreenManager &value);
  /** @brief Loads screen content. */
  virtual void LoadContent();
  /** @brief Unloads screen content. */
  virtual void UnloadContent();
  /** @brief Updates transition state. @param gameTime Timing snapshot. @param
   * otherScreenHasFocus Whether another screen has focus. @param
   * coveredByOtherScreen Whether another screen covers this one. */
  virtual void Update(Microsoft::Xna::Framework::GameTime &gameTime,
                      bool otherScreenHasFocus, bool coveredByOtherScreen);
  /** @brief Handles input while active. @param input Current input state. */
  virtual void HandleInput(InputState &input);
  /** @brief Updates gamer presence for the active screen. */
  virtual void UpdatePresence();
  /** @brief Draws the screen. @param gameTime Timing snapshot. */
  virtual void Draw(const Microsoft::Xna::Framework::GameTime &gameTime) = 0;
  /** @brief Starts or completes removal from the stack. */
  virtual void ExitScreen();
  /** @brief Returns the logical runtime type name. @return Fully qualified type
   * name. */
  CNAEXT [[nodiscard]] const std::string &GetTypeName() const override = 0;

protected:
  /** @brief Sets popup state. @param value New popup state. */
  void setIsPopupProperty(bool value);
  /** @brief Sets transition-on duration. @param value New duration. */
  void setTransitionOnTimeProperty(System::TimeSpan value);
  /** @brief Sets transition-off duration. @param value New duration. */
  void setTransitionOffTimeProperty(System::TimeSpan value);

private:
  bool UpdateTransition(Microsoft::Xna::Framework::GameTime &gameTime,
                        System::TimeSpan time, int direction);
  bool isPopup_ = false;
  System::TimeSpan transitionOnTime_ = System::TimeSpan::Zero;
  System::TimeSpan transitionOffTime_ = System::TimeSpan::Zero;
  float transitionPosition_ = 1.0f;
  ScreenState screenState_ = ScreenState::TransitionOn;
  bool isExiting_ = false;
  bool otherScreenHasFocus_ = false;
  ScreenManager *screenManager_ = nullptr;
};
} // namespace NetRumble
