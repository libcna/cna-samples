// SPDX-License-Identifier: MS-PL
#include "Gameplay/Asteroid.hpp"

#include "AudioManager.hpp"
#include "Gameplay/Projectiles/Projectile.hpp"
#include "Gameplay/Ship.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "RandomMath.hpp"
#include "System/ArgumentOutOfRangeException.hpp"

namespace NetRumble {
using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::Texture2D;

std::array<std::optional<Texture2D>, Asteroid::variations_> Asteroid::textures_;

Asteroid::Asteroid(float radius) {
  if (radius <= 0.0f) {
    throw System::ArgumentOutOfRangeException("radius");
  }

  radius_ = radius;
  mass_ = radius_ * massRadiusRatio_;
  velocity_ = RandomMath::RandomDirection() *
              RandomMath::RandomBetween(initialSpeedMinimum_,
                                        initialSpeedMaximum_);
}

int Asteroid::getVariationProperty() const { return variation_; }

void Asteroid::setVariationProperty(int value) {
  if (value < 0 || value >= variations_) {
    throw System::ArgumentOutOfRangeException("value");
  }
  variation_ = value;
}

void Asteroid::Update(float elapsedTime) {
  const float velocityMassRatio = velocity_.LengthSquared() / mass_;
  rotation_ += velocityMassRatio * velocityMassRatio *
               velocityMassRatioToRotationScalar_ * elapsedTime;
  velocity_ -= velocity_ * (elapsedTime * dragPerSecond_);
  GameplayObject::Update(elapsedTime);
}

void Asteroid::Draw(
    float elapsedTime,
    Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch) {
  GameplayObject::Draw(elapsedTime, spriteBatch, *textures_[variation_],
                       std::nullopt, Color::White);
}

bool Asteroid::Touch(GameplayObject *target) {
  if (auto *player = dynamic_cast<Ship *>(target); player != nullptr) {
    Vector2 playerAsteroidVector = position_ - player->getPositionProperty();
    if (playerAsteroidVector.LengthSquared() > 0.0f) {
      playerAsteroidVector.Normalize();
      const float rammingSpeed =
          Vector2::Dot(playerAsteroidVector, player->getVelocityProperty()) -
          Vector2::Dot(playerAsteroidVector, velocity_);
      const float momentum = mass_ * rammingSpeed;
      player->Damage(this, momentum * momentumToDamageScalar_);
    }
  }

  if (dynamic_cast<Projectile *>(target) == nullptr) {
    AudioManager::PlaySoundEffect("asteroid_touch");
  }
  return true;
}

int Asteroid::getVariationsProperty() { return variations_; }

void Asteroid::LoadContent(ContentManager &contentManager) {
  for (int i = 0; i < variations_; ++i) {
    textures_[i].emplace(contentManager.Load<Texture2D>("Textures/asteroid" +
                                                        std::to_string(i)));
  }
}

void Asteroid::UnloadContent() {
  for (auto &texture : textures_) {
    texture.reset();
  }
}
} // namespace NetRumble
