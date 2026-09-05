// SPDX-License-Identifier: MS-PL
#include "Gameplay/PowerUps/PowerUp.hpp"

#include <cmath>
#include <limits>

#include "AudioManager.hpp"
#include "Gameplay/Ship.hpp"

namespace NetRumble {
PowerUp::PowerUp() {
  radius_ = PowerUpRadius;
  mass_ =
      static_cast<float>(std::numeric_limits<SharpRuntime::intcs>::max());
}

void PowerUp::Initialize() {
  if (!active_) {
    AudioManager::PlaySoundEffect("powerup_spawn");
  }
  GameplayObject::Initialize();
}

void PowerUp::Draw(
    float elapsedTime,
    Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch,
    Microsoft::Xna::Framework::Graphics::Texture2D &sprite,
    std::optional<Microsoft::Xna::Framework::Rectangle> sourceRectangle,
    Microsoft::Xna::Framework::Color color) {
  rotation_ += rotationSpeed_ * elapsedTime;

  const float oldRadius = radius_;
  pulseTime_ += elapsedTime;
  radius_ *= 1.0f + pulseAmplitude_ * std::sin(pulseTime_ / pulseRate_);
  GameplayObject::Draw(elapsedTime, spriteBatch, sprite, sourceRectangle,
                       color);
  radius_ = oldRadius;
}

bool PowerUp::Touch(GameplayObject *target) {
  if (dynamic_cast<Ship *>(target) != nullptr) {
    AudioManager::PlaySoundEffect("powerup_touch");
    Die(target, false);
    return false;
  }
  return GameplayObject::Touch(target);
}
} // namespace NetRumble
