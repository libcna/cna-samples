// SPDX-License-Identifier: MS-PL
#pragma once
#include "Gameplay/GameplayObject.hpp"
namespace NetRumble {
class Ship;
/** @brief Base for ship-owned finite-lifetime projectiles. */ class Projectile
    : public GameplayObject {
public:
  /** @brief Destroys a projectile. */ ~Projectile() override = default;
  /** @brief Gets owner. @return Owning ship. */ [[nodiscard]] Ship *
  getOwnerProperty() const {
    return owner_;
  }
  /** @brief Updates lifetime. @param elapsedTime Elapsed seconds. */ void
  Update(float elapsedTime) override;
  /** @brief Draws this projectile. @param elapsedTime Elapsed seconds. @param
   * spriteBatch Batch. */
  virtual void
  Draw(float elapsedTime,
       Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch) = 0;
  /** @brief Applies direct-hit behavior. @param target Target. @return
   * Physics-response decision. */
  bool
  Touch(GameplayObject *target) override;
  /** @brief Applies radial damage and removes the projectile. @param source
   * Cause. @param cleanupOnly Suppress effects. */
  void
  Die(GameplayObject *source, bool cleanupOnly) override;

protected:
  /** @brief Creates from an owner and direction. @param owner Owner. @param
   * direction Direction. */
  Projectile(Ship *owner, Microsoft::Xna::Framework::Vector2 direction);
  Ship *owner_;
  float damageAmount_ = 0, damageRadius_ = 0;
  bool damageOwner_ = true;
  float duration_ = 0;
};
} // namespace NetRumble
