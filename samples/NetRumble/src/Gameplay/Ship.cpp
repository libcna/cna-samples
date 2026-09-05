// SPDX-License-Identifier: MS-PL
#include "Gameplay/Ship.hpp"

#include <algorithm>
#include <any>
#include <cmath>
#include "AudioManager.hpp"
#include "Gameplay/PlayerData.hpp"
#include "Gameplay/Projectiles/Projectile.hpp"
#include "Gameplay/Weapons/LaserWeapon.hpp"
#include "Gameplay/Weapons/MineWeapon.hpp"
#include "Gameplay/Weapons/Weapon.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Rendering/Particles/ParticleEffectManager.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/ArgumentOutOfRangeException.hpp"
#include "System/InvalidOperationException.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Content;
using namespace Microsoft::Xna::Framework::Graphics;
using namespace Microsoft::Xna::Framework::Net;

std::array<std::optional<Texture2D>, Ship::variations_> Ship::primaryTextures_;
std::array<std::optional<Texture2D>, Ship::variations_> Ship::overlayTextures_;
std::optional<Texture2D> Ship::shieldTexture_;

std::array<Color, 18> Ship::ShipColors = {
    Color::Lime,          Color::CornflowerBlue,
    Color::Fuchsia,       Color::Red,
    Color::LightSeaGreen, Color::LightGray,
    Color::Gold,          Color::ForestGreen,
    Color::Beige,         Color::LightPink,
    Color::Lavender,      Color::OrangeRed,
    Color::Plum,          Color::Tan,
    Color::YellowGreen,   Color::Azure,
    Color::Aqua,          Color::Salmon,
};

ParticleEffectManager *Ship::ParticleEffectManagerInstance = nullptr;

namespace {
PlayerData *GetPlayerData(NetworkGamer *gamer) {
  if (gamer == nullptr || !gamer->getTagProperty().has_value()) {
    return nullptr;
  }
  try {
    return std::any_cast<PlayerData *>(gamer->getTagProperty());
  } catch (const std::bad_any_cast &) {
    return nullptr;
  }
}
} // namespace

Ship::Ship() {
  radius_ = 24.0f;
  mass_ = 32.0f;
}

Ship::~Ship() = default;

void Ship::setWeaponProperty(std::unique_ptr<Weapon> value) {
  if (value == nullptr) {
    throw System::ArgumentNullException("value");
  }
  weapon_ = std::move(value);
}

void Ship::setVariationProperty(int value) {
  if (value < 0 || value >= getVariationsProperty()) {
    throw System::ArgumentOutOfRangeException("value");
  }
  variation_ = value;
}

void Ship::Initialize() {
  if (!active_) {
    shipInput_ = ShipInput::getEmptyProperty();
    rotation_ = 0.0f;
    velocity_ = Vector2::Zero;
    life_ = lifeMaximum_;
    shield_ = shieldMaximum_;
    shieldRechargeTimer_ = 0.0f;
    safeTimer_ = safeTimerMaximum_;
    weapon_ = std::make_unique<LaserWeapon>(this);
    mineWeapon_ = std::make_unique<MineWeapon>(this);

    AudioManager::PlaySoundEffect("player_spawn");
    if (ParticleEffectManagerInstance != nullptr) {
      ParticleEffectManagerInstance->SpawnEffect(ParticleEffectType::ShipSpawn,
                                                 this);
    }
    projectiles_.clear();
  }
  GameplayObject::Initialize();
}

void Ship::Update(float elapsedTime) {
  const Vector2 forward(std::sin(rotation_), -std::cos(rotation_));
  const Vector2 right(-forward.Y, forward.X);

  shipInput_.LeftStick.Y *= -1.0f;
  if (shipInput_.LeftStick.LengthSquared() > 0.0f) {
    const Vector2 wantedForward = Vector2::Normalize(shipInput_.LeftStick);
    const float angleDiff = std::acos(Vector2::Dot(wantedForward, forward));
    const float facing =
        Vector2::Dot(wantedForward, right) > 0.0f ? 1.0f : -1.0f;
    if (angleDiff > 0.001f) {
      rotation_ += std::min(
          angleDiff, facing * elapsedTime * rotationRadiansPerSecond_);
    }

    velocity_ += shipInput_.LeftStick * (elapsedTime * fullSpeed_);
    if (velocity_.Length() > velocityMaximum_) {
      velocity_ = Vector2::Normalize(velocity_) * velocityMaximum_;
    }
  }
  shipInput_.LeftStick = Vector2::Zero;

  velocity_ -= velocity_ * (elapsedTime * dragPerSecond_);
  if (velocity_.LengthSquared() <= 0.0f) {
    velocity_ = Vector2::Zero;
  }

  shipInput_.RightStick.Y *= -1.0f;
  if (shipInput_.RightStick.LengthSquared() > fireThresholdSquared_) {
    weapon_->Fire(Vector2::Normalize(shipInput_.RightStick));
  }
  shipInput_.RightStick = Vector2::Zero;

  if (shipInput_.MineFired) {
    mineWeapon_->Fire(-forward);
  }
  shipInput_.MineFired = false;

  if (shieldRechargeTimer_ > 0.0f) {
    shieldRechargeTimer_ = std::max(shieldRechargeTimer_ - elapsedTime, 0.0f);
  }
  if (shieldRechargeTimer_ <= 0.0f && shield_ < shieldMaximum_) {
    shield_ = std::min(
        shieldMaximum_, shield_ + shieldRechargePerSecond_ * elapsedTime);
  }

  radius_ = shield_ > 0.0f ? 24.0f : 20.0f;
  if (weapon_ != nullptr) {
    weapon_->Update(elapsedTime);
  }
  if (mineWeapon_ != nullptr) {
    mineWeapon_->Update(elapsedTime);
  }
  if (safeTimer_ > 0.0f) {
    safeTimer_ = std::max(safeTimer_ - elapsedTime, 0.0f);
  }

  for (const auto &projectile : projectiles_) {
    if (projectile->getActiveProperty()) {
      projectile->Update(elapsedTime);
    } else {
      projectiles_.QueuePendingRemoval(projectile);
    }
  }
  projectiles_.ApplyPendingRemovals();

  GameplayObject::Update(elapsedTime);
}

void Ship::Draw(float elapsedTime, SpriteBatch &spriteBatch) {
  GameplayObject::Draw(elapsedTime, spriteBatch, *primaryTextures_[variation_],
                       std::nullopt, Color::White);
  GameplayObject::Draw(elapsedTime, spriteBatch, *overlayTextures_[variation_],
                       std::nullopt, color_);

  if (shield_ > 0.0f) {
    const float oldRadius = radius_;
    shieldPulseTime_ += elapsedTime;
    radius_ *= shieldScaleBase_ +
               shieldPulseAmplitude_ *
                   std::sin(shieldPulseTime_ / shieldPulseRate_);
    GameplayObject::Draw(
        elapsedTime, spriteBatch, *shieldTexture_, std::nullopt,
        Color(static_cast<SharpRuntime::intcs>(color_.getRProperty()),
              static_cast<SharpRuntime::intcs>(color_.getGProperty()),
              static_cast<SharpRuntime::intcs>(color_.getBProperty()),
              static_cast<SharpRuntime::intcs>(
                  std::floor(shieldAlphaMaximum_ * shield_ /
                             shieldMaximum_))));
    radius_ = oldRadius;
  }

  for (const auto &projectile : projectiles_) {
    projectile->Draw(elapsedTime, spriteBatch);
  }
}

bool Ship::Damage(GameplayObject *source, float damageAmount) {
  if (safeTimer_ > 0.0f || damageAmount <= 0.0f) {
    return false;
  }

  shieldRechargeTimer_ = shieldRechargeTimerMaximum_;
  if (shield_ <= 0.0f) {
    life_ -= damageAmount;
  } else {
    shield_ -= damageAmount;
    if (shield_ < 0.0f) {
      life_ += shield_;
      shield_ = 0.0f;
    }
  }

  if (auto *projectile = dynamic_cast<Projectile *>(source);
      projectile != nullptr) {
    lastDamagedBy_ = projectile->getOwnerProperty();
  } else {
    lastDamagedBy_ = source;
  }
  return true;
}

void Ship::Die(GameplayObject *source, bool cleanupOnly) {
  if (active_) {
    if (!cleanupOnly) {
      Ship *ship = dynamic_cast<Ship *>(source);
      if (ship == nullptr) {
        if (auto *projectile = dynamic_cast<Projectile *>(source);
            projectile != nullptr) {
          ship = projectile->getOwnerProperty();
        }
      }

      if (ship != nullptr) {
        ship->score_ += ship == this ? -1 : 1;
      } else {
        --score_;
      }

      AudioManager::PlaySoundEffect("explosion_shockwave");
      AudioManager::PlaySoundEffect("explosion_large");
      if (ParticleEffectManagerInstance != nullptr) {
        ParticleEffectManagerInstance->SpawnEffect(
            ParticleEffectType::ShipExplosion, position_);
      }
    }

    for (const auto &projectile : projectiles_) {
      projectile->Die(nullptr, true);
    }
    projectiles_.clear();
    respawnTimer_ = respawnTimerOnDeath_;
  }
  GameplayObject::Die(source, cleanupOnly);
}

void Ship::LoadContent(ContentManager &contentManager) {
  for (int i = 0; i < getVariationsProperty(); ++i) {
    primaryTextures_[i].emplace(
        contentManager.Load<Texture2D>("Textures/ship" + std::to_string(i)));
    overlayTextures_[i].emplace(contentManager.Load<Texture2D>(
        "Textures/ship" + std::to_string(i) + "Overlay"));
  }
  shieldTexture_.emplace(
      contentManager.Load<Texture2D>("Textures/shipShields"));
}

void Ship::UnloadContent() {
  for (int i = 0; i < getVariationsProperty(); ++i) {
    primaryTextures_[i].reset();
    overlayTextures_[i].reset();
  }
  shieldTexture_.reset();
}

bool Ship::HasUniqueColorIndex(NetworkGamer *networkGamer,
                               NetworkSession *networkSession) {
  if (networkGamer == nullptr) {
    throw System::ArgumentNullException("networkGamer");
  }
  if (networkSession == nullptr) {
    throw System::ArgumentNullException("networkSession");
  }

  PlayerData *playerData = GetPlayerData(networkGamer);
  if (playerData == nullptr) {
    throw System::ArgumentNullException("networkGamer.Tag as PlayerData");
  }

  for (NetworkGamer *gamer : networkSession->getAllGamersProperty()) {
    if (gamer == networkGamer) {
      continue;
    }
    PlayerData *gamerData = GetPlayerData(gamer);
    if (gamerData != nullptr && gamerData->getShipColorProperty() ==
                                    playerData->getShipColorProperty()) {
      return false;
    }
  }
  return true;
}

SharpRuntime::bytecs
Ship::GetNextUniqueColorIndex(SharpRuntime::bytecs currentColorIndex,
                              NetworkSession *networkSession) {
  if (currentColorIndex >= ShipColors.size()) {
    throw System::ArgumentOutOfRangeException("currentColorIndex");
  }
  if (networkSession == nullptr) {
    throw System::ArgumentNullException("networkSession");
  }

  const int gamerCount =
      networkSession->getAllGamersProperty().getCountProperty();
  if (gamerCount > static_cast<int>(ShipColors.size())) {
    throw System::InvalidOperationException(
        "There are more gamers than there are colors.");
  }
  if (gamerCount == static_cast<int>(ShipColors.size())) {
    return currentColorIndex;
  }

  bool colorFound;
  SharpRuntime::bytecs newColorIndex = currentColorIndex;
  do {
    ++newColorIndex;
    if (newColorIndex >= ShipColors.size()) {
      newColorIndex = 0;
    }

    colorFound = false;
    for (NetworkGamer *networkGamer : networkSession->getAllGamersProperty()) {
      PlayerData *playerData = GetPlayerData(networkGamer);
      if (playerData != nullptr &&
          playerData->getShipColorProperty() == newColorIndex) {
        colorFound = true;
        break;
      }
    }
  } while (colorFound && newColorIndex != currentColorIndex);
  return newColorIndex;
}

SharpRuntime::bytecs
Ship::GetPreviousUniqueColorIndex(SharpRuntime::bytecs currentColorIndex,
                                  NetworkSession *networkSession) {
  if (currentColorIndex >= ShipColors.size()) {
    throw System::ArgumentOutOfRangeException("currentColorIndex");
  }
  if (networkSession == nullptr) {
    throw System::ArgumentNullException("networkSession");
  }

  const int gamerCount =
      networkSession->getAllGamersProperty().getCountProperty();
  if (gamerCount > static_cast<int>(ShipColors.size())) {
    throw System::InvalidOperationException(
        "There are more gamers than there are colors.");
  }
  if (gamerCount == static_cast<int>(ShipColors.size())) {
    return currentColorIndex;
  }

  bool colorFound;
  SharpRuntime::bytecs newColorIndex = currentColorIndex;
  do {
    newColorIndex = newColorIndex == 0
                        ? static_cast<SharpRuntime::bytecs>(ShipColors.size() - 1)
                        : static_cast<SharpRuntime::bytecs>(newColorIndex - 1);

    colorFound = false;
    for (NetworkGamer *networkGamer : networkSession->getAllGamersProperty()) {
      PlayerData *playerData = GetPlayerData(networkGamer);
      if (playerData != nullptr &&
          playerData->getShipColorProperty() == newColorIndex) {
        colorFound = true;
        break;
      }
    }
  } while (colorFound && newColorIndex != currentColorIndex);
  return newColorIndex;
}
} // namespace NetRumble
