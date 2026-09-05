// SPDX-License-Identifier: MS-PL
#include "Gameplay/GameplayObject.hpp"
#include "Gameplay/CollisionManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include <cmath>
#include "System/ArgumentException.hpp"
namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;
bool GameplayObject::getActiveProperty() const { return active_; }
Vector2 GameplayObject::getPositionProperty() const { return position_; }
void GameplayObject::setPositionProperty(Vector2 v) { position_ = v; }
Vector2 GameplayObject::getVelocityProperty() const { return velocity_; }
void GameplayObject::setVelocityProperty(Vector2 v) {
  if (std::isnan(v.X) || std::isnan(v.Y))
    throw System::ArgumentException("Velocity was NaN");
  velocity_ = v;
}
float GameplayObject::getRotationProperty() const { return rotation_; }
void GameplayObject::setRotationProperty(float v) { rotation_ = v; }
float GameplayObject::getRadiusProperty() const { return radius_; }
void GameplayObject::setRadiusProperty(float v) { radius_ = v; }
float GameplayObject::getMassProperty() const { return mass_; }
bool GameplayObject::getCollidedThisFrameProperty() const {
  return collidedThisFrame_;
}
void GameplayObject::setCollidedThisFrameProperty(bool v) {
  collidedThisFrame_ = v;
}
void GameplayObject::Initialize() {
  if (!active_) {
    active_ = true;
    CollisionManager::getCollectionProperty().Add(this);
  }
}
void GameplayObject::Update(float) { collidedThisFrame_ = false; }
void GameplayObject::Draw(float, SpriteBatch &b, Texture2D &t,
                          std::optional<Rectangle> r, Color c) {
  b.Draw(t, position_, r, c, rotation_,
         Vector2(t.getWidthProperty() / 2.f, t.getHeightProperty() / 2.f),
         2.f * radius_ /
             MathHelper::Min(float(t.getWidthProperty()),
                             float(t.getHeightProperty())),
         SpriteEffects::None, 0);
}
bool GameplayObject::Touch(GameplayObject *) { return true; }
bool GameplayObject::Damage(GameplayObject *, float) { return false; }
void GameplayObject::Die(GameplayObject *, bool) {
  if (active_) {
    active_ = false;
    CollisionManager::getCollectionProperty().QueuePendingRemoval(this);
  }
}
} // namespace NetRumble
