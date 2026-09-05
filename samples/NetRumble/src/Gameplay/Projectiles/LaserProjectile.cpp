// SPDX-License-Identifier: MS-PL
#include "Gameplay/Projectiles/LaserProjectile.hpp"
#include "Gameplay/Ship.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Rendering/Particles/ParticleEffectManager.hpp"
namespace NetRumble {
std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>
    LaserProjectile::texture_;
ParticleEffectManager *LaserProjectile::ParticleEffectManagerInstance = nullptr;
LaserProjectile::LaserProjectile(Ship *o, Microsoft::Xna::Framework::Vector2 d)
    : Projectile(o, d) {
  velocity_ = initialSpeed_ * d;
  radius_ = 4;
  mass_ = .5f;
  duration_ = 5;
  damageAmount_ = 20;
  damageOwner_ = false;
}
void LaserProjectile::Draw(
    float e, Microsoft::Xna::Framework::Graphics::SpriteBatch &b) {
  GameplayObject::Draw(e, b, *texture_, std::nullopt,
                       owner_ ? owner_->getColorProperty()
                              : Microsoft::Xna::Framework::Color::White);
}
void LaserProjectile::Die(GameplayObject *s, bool c) {
  if (active_ && !c && ParticleEffectManagerInstance)
    ParticleEffectManagerInstance->SpawnEffect(
        ParticleEffectType::LaserExplosion, position_);
  Projectile::Die(s, c);
}
void LaserProjectile::LoadContent(
    Microsoft::Xna::Framework::Content::ContentManager &c) {
  texture_.emplace(
      c.Load<Microsoft::Xna::Framework::Graphics::Texture2D>("Textures/laser"));
}
void LaserProjectile::UnloadContent() { texture_.reset(); }
} // namespace NetRumble
