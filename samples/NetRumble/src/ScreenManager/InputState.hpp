// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace NetRumble {
/** @brief Tracks current and previous keyboard and first-gamepad input. */
class InputState {
public:
  /** @brief Keyboard state sampled during the current update. */
  Microsoft::Xna::Framework::Input::KeyboardState CurrentKeyboardState;
  /** @brief First-player gamepad state sampled during the current update. */
  Microsoft::Xna::Framework::Input::GamePadState CurrentGamePadState;
  /** @brief Keyboard state sampled during the previous update. */
  Microsoft::Xna::Framework::Input::KeyboardState LastKeyboardState;
  /** @brief First-player gamepad state sampled during the previous update. */
  Microsoft::Xna::Framework::Input::GamePadState LastGamePadState;

  /** @brief Gets a new menu-up action. @return True on a new up action. */
  [[nodiscard]] bool getMenuUpProperty() const;
  /** @brief Gets a new menu-down action. @return True on a new down action. */
  [[nodiscard]] bool getMenuDownProperty() const;
  /** @brief Gets a new menu-selection action. @return True on selection. */
  [[nodiscard]] bool getMenuSelectProperty() const;
  /** @brief Gets a new menu-cancel action. @return True on cancellation. */
  [[nodiscard]] bool getMenuCancelProperty() const;
  /** @brief Gets a new pause action. @return True on pause. */
  [[nodiscard]] bool getPauseGameProperty() const;
  /** @brief Gets a color-increment action. @return True on action. */
  [[nodiscard]] bool getShipColorChangeUpProperty() const;
  /** @brief Gets a color-decrement action. @return True on action. */
  [[nodiscard]] bool getShipColorChangeDownProperty() const;
  /** @brief Gets a ship-model increment action. @return True on action. */
  [[nodiscard]] bool getShipModelChangeUpProperty() const;
  /** @brief Gets a ship-model decrement action. @return True on action. */
  [[nodiscard]] bool getShipModelChangeDownProperty() const;
  /** @brief Gets a lobby-ready action. @return True on action. */
  [[nodiscard]] bool getMarkReadyProperty() const;
  /** @brief Samples the keyboard and first gamepad. */
  void Update();
  /** @brief Tests a newly pressed key. @param key Key to test. @return True
   * only on its up-to-down edge. */
  [[nodiscard]] bool
  IsNewKeyPress(Microsoft::Xna::Framework::Input::Keys key) const;
};
} // namespace NetRumble
