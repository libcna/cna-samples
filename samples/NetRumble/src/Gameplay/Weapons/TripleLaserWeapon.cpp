// SPDX-License-Identifier: MS-PL
#include "Gameplay/Weapons/TripleLaserWeapon.hpp"
#include "Gameplay/Projectiles/LaserProjectile.hpp"
#include "Gameplay/Ship.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include <cmath>
namespace NetRumble {
const float TripleLaserWeapon::laserSpreadRadians_ =
    Microsoft::Xna::Framework::MathHelper::ToRadians(2.5f);

TripleLaserWeapon::TripleLaserWeapon(Ship *o) : LaserWeapon(o) {
  fireDelay_ = .3f;
}
void TripleLaserWeapon::CreateProjectiles(
    Microsoft::Xna::Framework::Vector2 d) {
  using Microsoft::Xna::Framework::Vector2;
  float r = std::acos(Vector2::Dot(Vector2(0, -1), d));
  r *= Vector2::Dot(Vector2(0, -1), Vector2(d.Y, -d.X)) > 0 ? 1.f : -1.f;
  Vector2 directions[]{
      d,
      Vector2(std::sin(r - laserSpreadRadians_),
              -std::cos(r - laserSpreadRadians_)),
      Vector2(std::sin(r + laserSpreadRadians_),
              -std::cos(r + laserSpreadRadians_))};
  for (auto x : directions) {
    auto p = std::make_shared<LaserProjectile>(owner_, x);
    p->Initialize();
    owner_->getProjectilesProperty().Add(p);
  }
}
} // namespace NetRumble
