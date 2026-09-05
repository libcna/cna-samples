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
/** @brief Fast non-explosive laser bolt. */
class LaserProjectile final : public Projectile {
public:
  /** @brief Creates a laser. @param owner Owner. @param direction Direction. */
  LaserProjectile(Ship *owner,
                  Microsoft::Xna::Framework::Vector2 direction);

  /** @brief Draws the laser. @param elapsedTime Elapsed seconds. @param spriteBatch Batch. */
  void Draw(float elapsedTime,
            Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch)
      override;

  /** @brief Emits impact particles and dies. @param source Cause. @param cleanupOnly Suppress effects. */
  void Die(GameplayObject *source, bool cleanupOnly) override;

  /** @brief Loads texture. @param contentManager Content manager. */
  static void LoadContent(
      Microsoft::Xna::Framework::Content::ContentManager &contentManager);

  /** @brief Releases texture. */
  static void UnloadContent();
  /** @brief Particle manager that receives laser impact effects. */
  static ParticleEffectManager *ParticleEffectManagerInstance;

private:
  static constexpr float initialSpeed_ = 640.0f;
  static std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> texture_;
};
} // namespace NetRumble
