// SPDX-License-Identifier: MS-PL
#pragma once
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include <optional>
namespace Microsoft::Xna::Framework::Graphics {
class SpriteBatch;
class Texture2D;
} // namespace Microsoft::Xna::Framework::Graphics
namespace NetRumble {
/** @brief Base for every simulated collision object. */
class GameplayObject {
public:
  /** @brief Destroys a gameplay object. */ virtual ~GameplayObject() = default;
  /** @brief Gets active state. @return True after initialization and before
   * death. */
  [[nodiscard]] bool
  getActiveProperty() const;
  /** @brief Gets position. @return World position. */ [[nodiscard]] Microsoft::
      Xna::Framework::Vector2
      getPositionProperty() const;
  /** @brief Sets position. @param value World position. */ void
  setPositionProperty(Microsoft::Xna::Framework::Vector2 value);
  /** @brief Gets velocity. @return Velocity. */ [[nodiscard]] Microsoft::Xna::
      Framework::Vector2
      getVelocityProperty() const;
  /** @brief Sets velocity. @param value Velocity. */ void
  setVelocityProperty(Microsoft::Xna::Framework::Vector2 value);
  /** @brief Gets rotation. @return Rotation radians. */ [[nodiscard]] float
  getRotationProperty() const;
  /** @brief Sets rotation. @param value Rotation radians. */ void
  setRotationProperty(float value);
  /** @brief Gets collision radius. @return Radius. */ [[nodiscard]] float
  getRadiusProperty() const;
  /** @brief Sets collision radius. @param value Radius. */ void
  setRadiusProperty(float value);
  /** @brief Gets mass. @return Mass. */ [[nodiscard]] float
  getMassProperty() const;
  /** @brief Gets collision flag. @return Flag. */ [[nodiscard]] bool
  getCollidedThisFrameProperty() const;
  /** @brief Sets collision flag. @param value Flag. */ void
  setCollidedThisFrameProperty(bool value);
  /** @brief Activates and registers the object. */ virtual void Initialize();
  /** @brief Clears frame collision state. @param elapsedTime Elapsed seconds.
   */
  virtual void
  Update(float elapsedTime);
  /** @brief Draws a centered scaled sprite. @param elapsedTime Elapsed seconds.
   * @param spriteBatch Sprite batch. @param sprite Texture. @param
   * sourceRectangle Optional source. @param color Tint. */
  virtual void
  Draw(float elapsedTime,
       Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch,
       Microsoft::Xna::Framework::Graphics::Texture2D &sprite,
       std::optional<Microsoft::Xna::Framework::Rectangle> sourceRectangle,
       Microsoft::Xna::Framework::Color color);
  /** @brief Handles a touch. @param target Other object. @return Whether
   * physics response should occur. */
  virtual bool
  Touch(GameplayObject *target);
  /** @brief Applies damage. @param source Damage source. @param damageAmount
   * Damage. @return Whether damage was applied. */
  virtual bool
  Damage(GameplayObject *source, float damageAmount);
  /** @brief Deactivates the object. @param source Cause. @param cleanupOnly
   * Whether effects should be suppressed. */
  virtual void
  Die(GameplayObject *source, bool cleanupOnly);

protected:
  GameplayObject() = default;
  bool active_ = false;
  Microsoft::Xna::Framework::Vector2 position_ =
      Microsoft::Xna::Framework::Vector2::Zero;
  Microsoft::Xna::Framework::Vector2 velocity_ =
      Microsoft::Xna::Framework::Vector2::Zero;
  float rotation_ = 0;
  float radius_ = 1;
  float mass_ = 1;
  bool collidedThisFrame_ = false;
};
} // namespace NetRumble
