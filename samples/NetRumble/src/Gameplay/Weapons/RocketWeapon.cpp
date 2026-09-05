// SPDX-License-Identifier: MS-PL
#include "Gameplay/Weapons/RocketWeapon.hpp"
#include "Gameplay/Projectiles/RocketProjectile.hpp"
#include "Gameplay/Ship.hpp"
#include "RandomMath.hpp"
namespace NetRumble {
RocketWeapon::RocketWeapon(Ship *o) : Weapon(o) {
  fireDelay_ = .5f;
  fireSoundEffect_ = RandomMath::getRandomProperty().Next(2) == 0
                         ? "fire_rocket1"
                         : "fire_rocket2";
}
void RocketWeapon::CreateProjectiles(Microsoft::Xna::Framework::Vector2 d) {
  auto p = std::make_shared<RocketProjectile>(owner_, d);
  p->Initialize();
  owner_->getProjectilesProperty().Add(p);
}
} // namespace NetRumble
