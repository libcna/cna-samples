// SPDX-License-Identifier: MS-PL
#include "Gameplay/Weapons/LaserWeapon.hpp"
#include "Gameplay/Projectiles/LaserProjectile.hpp"
#include "Gameplay/Ship.hpp"
#include "RandomMath.hpp"
namespace NetRumble {
LaserWeapon::LaserWeapon(Ship *o) : Weapon(o) {
  fireDelay_ = .15f;
  fireSoundEffect_ =
      "fire_laser" +
      std::to_string(RandomMath::getRandomProperty().Next(3) + 1);
}
void LaserWeapon::CreateProjectiles(Microsoft::Xna::Framework::Vector2 d) {
  auto p = std::make_shared<LaserProjectile>(owner_, d);
  p->Initialize();
  owner_->getProjectilesProperty().Add(p);
}
} // namespace NetRumble
