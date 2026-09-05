// SPDX-License-Identifier: MS-PL
#pragma once
#include "Gameplay/Weapons/Weapon.hpp"
namespace NetRumble {
/** @brief Drops timed proximity mines. */
class MineWeapon final : public Weapon {
public:
  /** @brief Creates a mine weapon. @param owner Owning ship. */
  explicit MineWeapon(Ship *owner);

protected:
  /** @brief Creates one mine projectile. @param direction Firing direction. */
  void CreateProjectiles(Microsoft::Xna::Framework::Vector2 direction) override;

private:
  static constexpr float mineSpawnDistance_ = 8.0f;
};
} // namespace NetRumble
