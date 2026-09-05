// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>

#include "Gameplay/PowerUps/PowerUp.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Microsoft::Xna::Framework::Content {
class ContentManager;
}

namespace NetRumble {
/** @brief Power-up that equips the triple-laser weapon. */
class TripleLaserPowerUp final : public PowerUp {
public:
  /** @brief Constructs a triple-laser power-up. */
  TripleLaserPowerUp() = default;

  /** @brief Draws the power-up. @param elapsedTime Elapsed seconds. @param
   * spriteBatch Sprite batch. */
  void
  Draw(float elapsedTime,
       Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch) override;

  /** @brief Equips a touching ship. @param target Other object. @return
   * Collision response flag. */
  bool Touch(GameplayObject *target) override;

  /** @brief Loads the power-up texture. @param contentManager Content manager.
   */
  static void LoadContent(
      Microsoft::Xna::Framework::Content::ContentManager &contentManager);

  /** @brief Releases the power-up texture. */
  static void UnloadContent();

private:
  static std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> texture_;
};
} // namespace NetRumble
