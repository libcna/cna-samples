// SPDX-License-Identifier: MS-PL
#include "Gameplay/PowerUps/RocketPowerUp.hpp"

#include "Gameplay/Ship.hpp"
#include "Gameplay/Weapons/RocketWeapon.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"

namespace NetRumble {
using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::Texture2D;

std::optional<Texture2D> RocketPowerUp::texture_;

void RocketPowerUp::Draw(
    float elapsedTime,
    Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch) {
  PowerUp::Draw(elapsedTime, spriteBatch, *texture_, std::nullopt,
                Color::White);
}

bool RocketPowerUp::Touch(GameplayObject *target) {
  if (auto *ship = dynamic_cast<Ship *>(target); ship != nullptr) {
    ship->setWeaponProperty(std::make_unique<RocketWeapon>(ship));
  }
  return PowerUp::Touch(target);
}

void RocketPowerUp::LoadContent(ContentManager &contentManager) {
  texture_.emplace(contentManager.Load<Texture2D>("Textures/powerupRocket"));
}

void RocketPowerUp::UnloadContent() { texture_.reset(); }
} // namespace NetRumble
