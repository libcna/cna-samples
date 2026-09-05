// SPDX-License-Identifier: MS-PL
#include "Gameplay/Projectiles/MineProjectile.hpp"
#include "AudioManager.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Rendering/Particles/ParticleEffectManager.hpp"
namespace NetRumble {
std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>
    MineProjectile::texture_;
ParticleEffectManager *MineProjectile::ParticleEffectManagerInstance = nullptr;
MineProjectile::MineProjectile(Ship *o, Microsoft::Xna::Framework::Vector2 d)
    : Projectile(o, d) {
  velocity_ = initialSpeed_ * d;
  radius_ = 10;
  mass_ = 5;
  duration_ = 15;
  damageAmount_ = 200;
  damageOwner_ = false;
  damageRadius_ = 80;
}
void MineProjectile::Update(float e) {
  Projectile::Update(e);
  velocity_ -= velocity_ * (e * dragPerSecond_);
  rotation_ += rotationRadiansPerSecond_ * e;
}
void MineProjectile::Draw(float e,
                          Microsoft::Xna::Framework::Graphics::SpriteBatch &b) {
  GameplayObject::Draw(e, b, *texture_, std::nullopt,
                       Microsoft::Xna::Framework::Color::White);
}
void MineProjectile::Die(GameplayObject *s, bool c) {
  if (active_ && !c) {
    AudioManager::PlaySoundEffect("explosion_large");
    if (ParticleEffectManagerInstance)
      ParticleEffectManagerInstance->SpawnEffect(
          ParticleEffectType::MineExplosion, position_);
  }
  Projectile::Die(s, c);
}
void MineProjectile::LoadContent(
    Microsoft::Xna::Framework::Content::ContentManager &c) {
  texture_.emplace(
      c.Load<Microsoft::Xna::Framework::Graphics::Texture2D>("Textures/mine"));
}
void MineProjectile::UnloadContent() { texture_.reset(); }
} // namespace NetRumble
