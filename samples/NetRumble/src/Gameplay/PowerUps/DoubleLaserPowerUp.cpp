// SPDX-License-Identifier: MS-PL
#include "Gameplay/PowerUps/DoubleLaserPowerUp.hpp"

#include "Gameplay/Ship.hpp"
#include "Gameplay/Weapons/DoubleLaserWeapon.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"

namespace NetRumble {
using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::Texture2D;

std::optional<Texture2D> DoubleLaserPowerUp::texture_;

void DoubleLaserPowerUp::Draw(
    float elapsedTime,
    Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch) {
  PowerUp::Draw(elapsedTime, spriteBatch, *texture_, std::nullopt,
                Color::White);
}

bool DoubleLaserPowerUp::Touch(GameplayObject *target) {
  if (auto *ship = dynamic_cast<Ship *>(target); ship != nullptr) {
    ship->setWeaponProperty(std::make_unique<DoubleLaserWeapon>(ship));
  }
  return PowerUp::Touch(target);
}

void DoubleLaserPowerUp::LoadContent(ContentManager &contentManager) {
  texture_.emplace(
      contentManager.Load<Texture2D>("Textures/powerupDoubleLaser"));
}

void DoubleLaserPowerUp::UnloadContent() { texture_.reset(); }
} // namespace NetRumble
