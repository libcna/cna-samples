// SPDX-License-Identifier: MS-PL
#include "Gameplay/Projectiles/RocketProjectile.hpp"
#include "AudioManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Rendering/Particles/ParticleEffect.hpp"
#include "Rendering/Particles/ParticleEffectManager.hpp"
namespace NetRumble {
std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>
    RocketProjectile::texture_;
ParticleEffectManager *RocketProjectile::ParticleEffectManagerInstance =
    nullptr;
RocketProjectile::RocketProjectile(Ship *o,
                                   Microsoft::Xna::Framework::Vector2 d)
    : Projectile(o, d) {
  velocity_ = initialSpeed_ * d;
  radius_ = 14;
  mass_ = 10;
  duration_ = 4;
  damageAmount_ = 150;
  damageOwner_ = false;
  damageRadius_ = 128;
  rotation_ += Microsoft::Xna::Framework::MathHelper::Pi;
}
void RocketProjectile::Initialize() {
  if (!active_) {
    AudioManager::PlaySoundEffect("rocket", true, rocketSound_);
    if (ParticleEffectManagerInstance)
      rocketTrailEffect_ = ParticleEffectManagerInstance->SpawnEffect(
          ParticleEffectType::RocketTrail, this);
  }
  GameplayObject::Initialize();
}
void RocketProjectile::Draw(
    float e, Microsoft::Xna::Framework::Graphics::SpriteBatch &b) {
  GameplayObject::Draw(e, b, *texture_, std::nullopt,
                       Microsoft::Xna::Framework::Color::White);
}
void RocketProjectile::Die(GameplayObject *s, bool c) {
  if (active_) {
    if (!c) {
      AudioManager::PlaySoundEffect("explosion_medium");
      if (ParticleEffectManagerInstance)
        ParticleEffectManagerInstance->SpawnEffect(
            ParticleEffectType::RocketExplosion, position_);
    }
    if (rocketSound_) {
      rocketSound_->Stop(true);
      rocketSound_->Dispose();
      rocketSound_.reset();
    }
    if (rocketTrailEffect_)
      rocketTrailEffect_->Stop(false);
  }
  Projectile::Die(s, c);
}
void RocketProjectile::LoadContent(
    Microsoft::Xna::Framework::Content::ContentManager &c) {
  texture_.emplace(c.Load<Microsoft::Xna::Framework::Graphics::Texture2D>(
      "Textures/rocket"));
}
void RocketProjectile::UnloadContent() { texture_.reset(); }
} // namespace NetRumble
