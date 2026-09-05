// SPDX-License-Identifier: MS-PL
#pragma once

#include <string>
#include <vector>

#include "ScreenManager/GameScreen.hpp"

namespace NetRumble {
/** @brief Base for vertically arranged NetRumble menus. */
class MenuScreen : public GameScreen {
public:
  /** @brief Handles menu navigation and activation. @param input Current input.
   */
  void HandleInput(InputState &input) override;
  /** @brief Draws the menu. @param gameTime Timing snapshot. */
  void Draw(const Microsoft::Xna::Framework::GameTime &gameTime) override;

protected:
  /** @brief Initializes transition timings. */
  MenuScreen();
  /** @brief Gets mutable menu text. @return Menu entries. */
  [[nodiscard]] std::vector<std::string> &getMenuEntriesProperty();
  /** @brief Handles selecting one entry. @param entryIndex Selected index. */
  virtual void OnSelectEntry(int entryIndex) = 0;
  /** @brief Handles cancellation. */
  virtual void OnCancel() = 0;

private:
  std::vector<std::string> menuEntries_;
  int selectedEntry_ = 0;
};
} // namespace NetRumble
