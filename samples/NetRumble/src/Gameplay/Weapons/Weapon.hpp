// SPDX-License-Identifier: MS-PL
#pragma once
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include <string>
namespace NetRumble {
class Ship;

/** @brief Base for cooldown-controlled ship weapons. */
class Weapon {
public:
  /** @brief Destroys the weapon. */
  virtual ~Weapon() = default;

  /** @brief Advances cooldown. @param elapsedTime Elapsed seconds. */
  virtual void Update(float elapsedTime);

  /** @brief Fires when cooldown permits. @param direction Firing direction. */
  virtual void Fire(Microsoft::Xna::Framework::Vector2 direction);

protected:
  /** @brief Creates a weapon for an owner. @param owner Owning ship. */
  explicit Weapon(Ship *owner);

  /** @brief Creates projectiles for one firing. @param direction Firing direction. */
  virtual void CreateProjectiles(
      Microsoft::Xna::Framework::Vector2 direction) = 0;

  Ship *owner_;
  float timeToNextFire_ = 0, fireDelay_ = 0;
  std::string fireSoundEffect_;
};
} // namespace NetRumble
