// SPDX-License-Identifier: MS-PL
#include "Gameplay/Weapons/DoubleLaserWeapon.hpp"
#include "Gameplay/Projectiles/LaserProjectile.hpp"
#include "Gameplay/Ship.hpp"
namespace NetRumble {
void DoubleLaserWeapon::CreateProjectiles(
    Microsoft::Xna::Framework::Vector2 d) {
  auto cross =
      Microsoft::Xna::Framework::Vector2(-d.Y, d.X) * laserSpread_;
  auto a = std::make_shared<LaserProjectile>(owner_, d);
  a->Initialize();
  owner_->getProjectilesProperty().Add(a);
  a->setPositionProperty(a->getPositionProperty() + cross);
  auto b = std::make_shared<LaserProjectile>(owner_, d);
  b->Initialize();
  owner_->getProjectilesProperty().Add(b);
  b->setPositionProperty(b->getPositionProperty() - cross);
}
} // namespace NetRumble
