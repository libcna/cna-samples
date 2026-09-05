// SPDX-License-Identifier: MS-PL
#pragma once
#include "Gameplay/Weapons/LaserWeapon.hpp"
namespace NetRumble {
/** @brief Fires a three-way laser spread. */
class TripleLaserWeapon final : public LaserWeapon {
public:
  /** @brief Creates a triple laser. @param owner Owning ship. */
  explicit TripleLaserWeapon(Ship *owner);

protected:
  /** @brief Creates a three-way laser spread. @param direction Firing direction. */
  void CreateProjectiles(Microsoft::Xna::Framework::Vector2 direction) override;

private:
  static const float laserSpreadRadians_;
};
} // namespace NetRumble
