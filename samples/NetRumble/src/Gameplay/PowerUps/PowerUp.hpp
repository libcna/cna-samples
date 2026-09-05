// SPDX-License-Identifier: MS-PL
#pragma once

#include "Gameplay/GameplayObject.hpp"

namespace NetRumble {
/** @brief Base class for collectible weapon power-ups. */
class PowerUp : public GameplayObject {
public:
  /** @brief Collision radius shared by every power-up. */
  static constexpr float PowerUpRadius = 20.0f;

  /** @brief Initializes the power-up and plays its spawn sound. */
  void Initialize() override;

  /** @brief Draws the concrete power-up. @param elapsedTime Elapsed seconds.
   * @param spriteBatch Sprite batch. */
  virtual void
  Draw(float elapsedTime,
       Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch) = 0;

  /**
   * @brief Draws a rotating and pulsing power-up sprite.
   * @param elapsedTime Elapsed seconds.
   * @param spriteBatch Sprite batch.
   * @param sprite Sprite texture.
   * @param sourceRectangle Optional source rectangle.
   * @param color Sprite tint.
   */
  void Draw(float elapsedTime,
            Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch,
            Microsoft::Xna::Framework::Graphics::Texture2D &sprite,
            std::optional<Microsoft::Xna::Framework::Rectangle> sourceRectangle,
            Microsoft::Xna::Framework::Color color) override;

  /** @brief Collects the power-up when touched by a ship. @param target Other
   * object. @return Collision response flag. */
  bool Touch(GameplayObject *target) override;

protected:
  /** @brief Constructs a power-up with the original collision properties. */
  PowerUp();

private:
  static constexpr float rotationSpeed_ = 2.0f;
  static constexpr float pulseAmplitude_ = 0.1f;
  static constexpr float pulseRate_ = 0.1f;
  float pulseTime_{0.0f};
};
} // namespace NetRumble
