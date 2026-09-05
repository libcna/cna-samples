// SPDX-License-Identifier: MS-PL
#pragma once

#include "CNA/CNAHelper.hpp"

#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "ScreenManager/GameScreen.hpp"
#include "System/EventArgs.hpp"
#include "System/EventHandler.hpp"

namespace NetRumble {
/** @brief Popup confirmation screen used by menus and gameplay. */
class MessageBoxScreen final : public GameScreen {
public:
  /** @brief Creates an ordinary message box. @param message Text to display. */
  explicit MessageBoxScreen(std::string message);
  /** @brief Creates a message box with pause-menu selection semantics. @param
   * message Text. @param pauseMenu Whether A-only acceptance is required. */
  MessageBoxScreen(std::string message, bool pauseMenu);
  /** @brief Loads the small usage font. */
  void LoadContent() override;
  /** @brief Handles accept and cancel. @param input Current input. */
  void HandleInput(InputState &input) override;
  /** @brief Draws the popup. @param gameTime Timing snapshot. */
  void Draw(const Microsoft::Xna::Framework::GameTime &gameTime) override;
  /** @brief Returns logical runtime type name. @return Type name. */
  CNAEXT [[nodiscard]] const std::string &GetTypeName() const override;

  /** @brief Raised when the user accepts the message box. */
  System::EventHandler<System::EventArgs> Accepted;
  /** @brief Raised when the user cancels the message box. */
  System::EventHandler<System::EventArgs> Cancelled;

private:
  bool pauseMenu_ = false;
  std::string message_;
  std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> smallFont_;
};
} // namespace NetRumble
