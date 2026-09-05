// SPDX-License-Identifier: MS-PL
#pragma once
#include "Gameplay/Projectiles/Projectile.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include <optional>
namespace Microsoft::Xna::Framework::Content {
class ContentManager;
}
namespace NetRumble {
class ParticleEffectManager;
/** @brief Slow explosive mine. */
class MineProjectile final : public Projectile {
public:
  /** @brief Creates a mine. @param owner Owner. @param direction Direction. */
  MineProjectile(Ship *owner,
                 Microsoft::Xna::Framework::Vector2 direction);

  /** @brief Updates drag and rotation. @param elapsedTime Elapsed seconds. */
  void Update(float elapsedTime) override;

  /** @brief Draws the mine. @param elapsedTime Elapsed seconds. @param spriteBatch Batch. */
  void Draw(float elapsedTime,
            Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch)
      override;

  /** @brief Explodes and emits particles. @param source Cause. @param cleanupOnly Suppress effects. */
  void Die(GameplayObject *source, bool cleanupOnly) override;

  /** @brief Loads texture. @param contentManager Content manager. */
  static void LoadContent(
      Microsoft::Xna::Framework::Content::ContentManager &contentManager);

  /** @brief Releases texture. */
  static void UnloadContent();
  /** @brief Particle manager that receives mine explosion effects. */
  static ParticleEffectManager *ParticleEffectManagerInstance;

private:
  static constexpr float initialSpeed_ = 64.0f;
  static constexpr float dragPerSecond_ = 0.9f;
  static constexpr float rotationRadiansPerSecond_ = 1.0f;
  static std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> texture_;
};
} // namespace NetRumble
