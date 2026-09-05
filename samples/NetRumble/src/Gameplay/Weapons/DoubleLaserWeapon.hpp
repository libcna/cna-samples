// SPDX-License-Identifier: MS-PL
#pragma once
#include "Gameplay/Weapons/LaserWeapon.hpp"
namespace NetRumble {
/** @brief Fires two parallel lasers. */
class DoubleLaserWeapon final : public LaserWeapon {
public:
  /** @brief Creates a double laser. @param owner Owning ship. */
  explicit DoubleLaserWeapon(Ship *owner) : LaserWeapon(owner) {}

protected:
  /** @brief Creates two parallel lasers. @param direction Firing direction. */
  void CreateProjectiles(Microsoft::Xna::Framework::Vector2 direction) override;

private:
  static constexpr float laserSpread_ = 8.0f;
};
} // namespace NetRumble
