// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <optional>

#include "Gameplay/GameplayObject.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Microsoft::Xna::Framework::Content {
class ContentManager;
}

namespace NetRumble {
/** @brief Asteroid that blocks player movement and projectiles. */
class Asteroid final : public GameplayObject {
public:
  /** @brief Constructs an asteroid. @param radius Collision radius. */
  explicit Asteroid(float radius);

  /** @brief Gets the visual variation. @return Variation index. */
  [[nodiscard]] int getVariationProperty() const;

  /** @brief Sets the visual variation. @param value Variation index. */
  void setVariationProperty(int value);

  /** @brief Updates drag and rotation. @param elapsedTime Elapsed seconds. */
  void Update(float elapsedTime) override;

  /** @brief Draws the asteroid. @param elapsedTime Elapsed seconds. @param
   * spriteBatch Sprite batch. */
  void Draw(float elapsedTime,
            Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch);

  /** @brief Handles an asteroid collision. @param target Other object. @return
   * Always true. */
  bool Touch(GameplayObject *target) override;

  /** @brief Gets the number of texture variations. @return Three. */
  [[nodiscard]] static int getVariationsProperty();

  /** @brief Loads asteroid textures. @param contentManager Content manager. */
  static void LoadContent(
      Microsoft::Xna::Framework::Content::ContentManager &contentManager);

  /** @brief Releases asteroid textures. */
  static void UnloadContent();

private:
  static constexpr float massRadiusRatio_ = 0.5f;
  static constexpr float dragPerSecond_ = 0.15f;
  static constexpr float velocityMassRatioToRotationScalar_ = 0.0017f;
  static constexpr float momentumToDamageScalar_ = 0.007f;
  static constexpr int variations_ = 3;
  static constexpr float initialSpeedMinimum_ = 32.0f;
  static constexpr float initialSpeedMaximum_ = 96.0f;
  static std::array<
      std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>,
      variations_>
      textures_;
  int variation_{0};
};
} // namespace NetRumble
