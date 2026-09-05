// SPDX-License-Identifier: MS-PL
#pragma once

#include "CNA/CNAHelper.hpp"

#include <memory>
#include <optional>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "ScreenManager/GameScreen.hpp"
#include "System/IDisposable.hpp"

namespace NetRumble {
class Starfield;

/** @brief Draws the title and moving parallax starfield behind all menus. */
class BackgroundScreen final : public GameScreen, public System::IDisposable {
public:
  /** @brief Constructs the background transition. */
  BackgroundScreen();

  /** @brief Releases the starfield. */
  ~BackgroundScreen() override;

  /** @brief Loads title and starfield content. */
  void LoadContent() override;

  /** @brief Unloads title and starfield content. */
  void UnloadContent() override;

  /** @brief Updates without allowing coverage to hide it. @param gameTime
   * Timing. @param otherScreenHasFocus Focus state. @param coveredByOtherScreen
   * Ignored coverage state. */
  void Update(Microsoft::Xna::Framework::GameTime &gameTime,
              bool otherScreenHasFocus, bool coveredByOtherScreen) override;

  /** @brief Draws the starfield and title. @param gameTime Timing. */
  void Draw(const Microsoft::Xna::Framework::GameTime &gameTime) override;

  /** @brief Disposes the starfield. */
  void Dispose() override;

  /** @brief Returns the managed type name. @return Type name. */
  CNAEXT [[nodiscard]] const std::string &GetTypeName() const override;

private:
  static constexpr float starsParallaxPeriod_ = 30.0f;
  static constexpr float starsParallaxAmplitude_ = 2048.0f;
  void Dispose(bool disposing);

  std::unique_ptr<Starfield> starfield_;
  double movement_{0.0};
  std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> titleTexture_;
  bool disposed_{false};
};
} // namespace NetRumble
