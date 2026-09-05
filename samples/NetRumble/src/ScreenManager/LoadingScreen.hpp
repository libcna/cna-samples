// SPDX-License-Identifier: MS-PL
#pragma once

#include "CNA/CNAHelper.hpp"

#include <functional>

#include "ScreenManager/GameScreen.hpp"
#include "System/EventArgs.hpp"
#include "System/Object.hpp"

namespace NetRumble {
/** @brief Removes the old screen stack before constructing the next screen. */
class LoadingScreen final : public GameScreen {
public:
  using LoadHandler =
      std::function<void(System::Object *, const System::EventArgs &)>;

  /** @brief Begins a loading transition. @param screenManager Owning manager.
   * @param loadNextScreen Callback that creates the next screen. @param
   * loadingIsSlow Whether to draw Loading text. */
  static void Load(ScreenManager &screenManager, LoadHandler loadNextScreen,
                   bool loadingIsSlow);
  /** @brief Updates stack removal and invokes the callback. @param gameTime
   * Timing. @param otherScreenHasFocus Focus state. @param coveredByOtherScreen
   * Cover state. */
  void Update(Microsoft::Xna::Framework::GameTime &gameTime,
              bool otherScreenHasFocus, bool coveredByOtherScreen) override;
  /** @brief Draws loading feedback. @param gameTime Timing snapshot. */
  void Draw(const Microsoft::Xna::Framework::GameTime &gameTime) override;
  /** @brief Returns logical runtime type name. @return Type name. */
  CNAEXT [[nodiscard]] const std::string &GetTypeName() const override;

private:
  LoadingScreen();
  bool loadingIsSlow_ = false;
  bool otherScreensAreGone_ = false;
  LoadHandler loadNextScreen_;
};
} // namespace NetRumble
