// SPDX-License-Identifier: MS-PL
#pragma once
#include "Gameplay/Weapons/Weapon.hpp"
namespace NetRumble {
/** @brief Fires the standard laser projectile. */
class LaserWeapon : public Weapon {
public:
  /** @brief Creates a laser weapon. @param owner Owning ship. */
  explicit LaserWeapon(Ship *owner);

protected:
  /** @brief Creates one laser projectile. @param direction Firing direction. */
  void CreateProjectiles(Microsoft::Xna::Framework::Vector2 direction) override;
};
} // namespace NetRumble
