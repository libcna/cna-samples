// SPDX-License-Identifier: MS-PL
#include "Gameplay/Weapons/MineWeapon.hpp"
#include "Gameplay/Projectiles/MineProjectile.hpp"
#include "Gameplay/Ship.hpp"
namespace NetRumble {
MineWeapon::MineWeapon(Ship *o) : Weapon(o) { fireDelay_ = 2; }
void MineWeapon::CreateProjectiles(Microsoft::Xna::Framework::Vector2 d) {
  auto p = std::make_shared<MineProjectile>(owner_, d);
  p->Initialize();
  owner_->getProjectilesProperty().Add(p);
  p->setPositionProperty(
      owner_->getPositionProperty() +
      d * (owner_->getRadiusProperty() + p->getRadiusProperty() +
           mineSpawnDistance_));
}
} // namespace NetRumble
