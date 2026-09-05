// SPDX-License-Identifier: MS-PL
#pragma once

#include "CNA/CNAHelper.hpp"

#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "OperationCompletedEventArgs.hpp"
#include "ScreenManager/GameScreen.hpp"
#include "System/EventHandler.hpp"

namespace System {
class IAsyncResult;
}

namespace NetRumble {
/** @brief Popup displayed while an asynchronous network operation is running.
 */
class NetworkBusyScreen final : public GameScreen {
public:
  /** @brief Raised when the tracked operation completes. */
  System::EventHandler<OperationCompletedEventArgs> OperationCompleted;

  /** @brief Constructs the busy popup. @param message Display text. @param
   * asyncResult Tracked operation. */
  NetworkBusyScreen(std::string message, System::IAsyncResult *asyncResult);

  /** @brief Loads the busy indicator. */
  void LoadContent() override;

  /** @brief Checks operation completion. @param gameTime Timing. @param
   * otherScreenHasFocus Focus state. @param coveredByOtherScreen Coverage
   * state. */
  void Update(Microsoft::Xna::Framework::GameTime &gameTime,
              bool otherScreenHasFocus, bool coveredByOtherScreen) override;

  /** @brief Draws the busy popup. @param gameTime Timing. */
  void Draw(const Microsoft::Xna::Framework::GameTime &gameTime) override;

  /** @brief Returns the managed type name. @return Type name. */
  CNAEXT [[nodiscard]] const std::string &GetTypeName() const override;

private:
  static constexpr float busyTextureScale_ = 0.8f;
  std::string message_;
  System::IAsyncResult *asyncResult_;
  std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> busyTexture_;
};
} // namespace NetRumble
