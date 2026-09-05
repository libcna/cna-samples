// SPDX-License-Identifier: MS-PL
#include "Gameplay/Projectiles/Projectile.hpp"
#include "Gameplay/CollisionManager.hpp"
#include "Gameplay/PowerUps/PowerUp.hpp"
#include "Gameplay/Ship.hpp"
#include "System/ArgumentNullException.hpp"
#include <cmath>
namespace NetRumble {
using Microsoft::Xna::Framework::Vector2;
Projectile::Projectile(Ship *o, Vector2 d) : owner_(o) {
  if (o == nullptr) {
    throw System::ArgumentNullException("owner");
  }
  velocity_ = d;
  position_ = o->getPositionProperty();
  rotation_ = std::acos(Vector2::Dot(Vector2::UnitY, d));
  if (d.X > 0)
    rotation_ = -rotation_;
}
void Projectile::Update(float e) {
  if (duration_ > 0) {
    duration_ -= e;
    if (duration_ < 0)
      Die(nullptr, false);
  }
  GameplayObject::Update(e);
}
bool Projectile::Touch(GameplayObject *t) {
  if (t) {
    if (dynamic_cast<PowerUp *>(t))
      return false;
    if (!damageOwner_ && t == owner_)
      return false;
    if (auto *p = dynamic_cast<Projectile *>(t); p && p->owner_ == owner_)
      return false;
    t->Damage(this, damageAmount_);
  }
  Die(t, false);
  return GameplayObject::Touch(t);
}
void Projectile::Die(GameplayObject *s, bool cleanup) {
  if (active_ && !cleanup)
    CollisionManager::Explode(this, s, damageAmount_, position_, damageRadius_,
                              damageOwner_);
  GameplayObject::Die(s, cleanup);
}
} // namespace NetRumble
