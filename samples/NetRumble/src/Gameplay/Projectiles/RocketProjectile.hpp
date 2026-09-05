// SPDX-License-Identifier: MS-PL
#pragma once
#include "Gameplay/Projectiles/Projectile.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffectInstance.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include <memory>
#include <optional>
namespace Microsoft::Xna::Framework::Content {
class ContentManager;
}
namespace NetRumble {
class ParticleEffect;
class ParticleEffectManager;
/** @brief Explosive rocket with looping sound and trail. */
class RocketProjectile final : public Projectile {
public:
  /** @brief Creates a rocket. @param owner Owner. @param direction Direction. */
  RocketProjectile(Ship *owner,
                   Microsoft::Xna::Framework::Vector2 direction);

  /** @brief Starts sound and trail before registration. */
  void Initialize() override;

  /** @brief Draws the rocket. @param elapsedTime Elapsed seconds. @param spriteBatch Batch. */
  void Draw(float elapsedTime,
            Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch)
      override;

  /** @brief Stops trail/audio and explodes. @param source Cause. @param cleanupOnly Suppress effects. */
  void Die(GameplayObject *source, bool cleanupOnly) override;

  /** @brief Loads texture. @param contentManager Content manager. */
  static void LoadContent(
      Microsoft::Xna::Framework::Content::ContentManager &contentManager);

  /** @brief Releases texture. */
  static void UnloadContent();
  /** @brief Particle manager that receives rocket effects. */
  static ParticleEffectManager *ParticleEffectManagerInstance;

private:
  static constexpr float initialSpeed_ = 650.0f;
  static std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> texture_;
  std::shared_ptr<ParticleEffect> rocketTrailEffect_;
  std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance>
      rocketSound_;
};
} // namespace NetRumble
