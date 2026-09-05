// SPDX-License-Identifier: MS-PL
#pragma once
#include "Gameplay/Weapons/Weapon.hpp"
namespace NetRumble {
/** @brief Fires explosive rockets. */
class RocketWeapon final : public Weapon {
public:
  /** @brief Creates a rocket weapon. @param owner Owning ship. */
  explicit RocketWeapon(Ship *owner);

protected:
  /** @brief Creates one rocket projectile. @param direction Firing direction. */
  void CreateProjectiles(Microsoft::Xna::Framework::Vector2 direction) override;
};
} // namespace NetRumble
