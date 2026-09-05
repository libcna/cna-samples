// SPDX-License-Identifier: MS-PL
#include "Gameplay/World.hpp"

#include <any>
#include <cmath>
#include <limits>

#include "Gameplay/CollisionManager.hpp"
#include "Gameplay/PlayerData.hpp"
#include "Gameplay/PowerUps/DoubleLaserPowerUp.hpp"
#include "Gameplay/PowerUps/PowerUp.hpp"
#include "Gameplay/PowerUps/RocketPowerUp.hpp"
#include "Gameplay/PowerUps/TripleLaserPowerUp.hpp"
#include "Gameplay/Projectiles/LaserProjectile.hpp"
#include "Gameplay/Projectiles/MineProjectile.hpp"
#include "Gameplay/Projectiles/RocketProjectile.hpp"
#include "Gameplay/Ship.hpp"
#include "Gameplay/ShipInput.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Net/LocalNetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionState.hpp"
#include "Microsoft/Xna/Framework/Net/SendDataOptions.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "RandomMath.hpp"
#include "Rendering/Particles/ParticleEffectManager.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Content;
using namespace Microsoft::Xna::Framework::GamerServices;
using namespace Microsoft::Xna::Framework::Graphics;
using namespace Microsoft::Xna::Framework::Input;
using namespace Microsoft::Xna::Framework::Net;

const Point World::barrierCounts_(50, 50);
const Rectangle World::dimensions_(0, 0,
                                   barrierCounts_.X * barrierSize_,
                                   barrierCounts_.Y * barrierSize_);

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

World::World(GraphicsDevice &graphicsDevice, ContentManager &contentManager,
             NetworkSession &networkSession)
    : networkSession_(&networkSession) {
  for (int i = 0;
       i < networkSession_->getAllGamersProperty().getCountProperty(); ++i) {
    if (networkSession_->getAllGamersProperty()[i]->getIsLocalProperty()) {
      updatesSinceStatusPacket_ = i;
      break;
    }
  }

  spriteBatch_ = std::make_unique<SpriteBatch>(graphicsDevice);
  particleEffectManager_ =
      std::make_unique<ParticleEffectManager>(contentManager);
  particleEffectManager_->RegisterParticleEffect(
      ParticleEffectType::LaserExplosion, "Particles/laserExplosion.xml", 40);
  particleEffectManager_->RegisterParticleEffect(
      ParticleEffectType::MineExplosion, "Particles/mineExplosion.xml", 8);
  particleEffectManager_->RegisterParticleEffect(
      ParticleEffectType::RocketExplosion, "Particles/rocketExplosion.xml", 24);
  particleEffectManager_->RegisterParticleEffect(
      ParticleEffectType::RocketTrail, "Particles/rocketTrail.xml", 16);
  particleEffectManager_->RegisterParticleEffect(
      ParticleEffectType::ShipExplosion, "Particles/shipExplosion.xml", 4);
  particleEffectManager_->RegisterParticleEffect(ParticleEffectType::ShipSpawn,
                                                 "Particles/shipSpawn.xml", 4);

  Ship::ParticleEffectManagerInstance = particleEffectManager_.get();
  RocketProjectile::ParticleEffectManagerInstance =
      particleEffectManager_.get();
  MineProjectile::ParticleEffectManagerInstance = particleEffectManager_.get();
  LaserProjectile::ParticleEffectManagerInstance = particleEffectManager_.get();

  playerFont_.emplace(contentManager.Load<SpriteFont>("Fonts/NetRumbleFont"));

  Ship::LoadContent(contentManager);
  Asteroid::LoadContent(contentManager);
  LaserProjectile::LoadContent(contentManager);
  MineProjectile::LoadContent(contentManager);
  RocketProjectile::LoadContent(contentManager);
  DoubleLaserPowerUp::LoadContent(contentManager);
  TripleLaserPowerUp::LoadContent(contentManager);
  RocketPowerUp::LoadContent(contentManager);

  chatAbleTexture_.emplace(contentManager.Load<Texture2D>("Textures/chatAble"));
  chatMuteTexture_.emplace(contentManager.Load<Texture2D>("Textures/chatMute"));
  chatTalkingTexture_.emplace(
      contentManager.Load<Texture2D>("Textures/chatTalking"));
  readyTexture_.emplace(contentManager.Load<Texture2D>("Textures/ready"));
  cornerBarrierTexture_.emplace(
      contentManager.Load<Texture2D>("Textures/barrierEnd"));
  verticalBarrierTexture_.emplace(
      contentManager.Load<Texture2D>("Textures/barrierPurple"));
  horizontalBarrierTexture_.emplace(
      contentManager.Load<Texture2D>("Textures/barrierRed"));

  CollisionManager::getCollectionProperty().clear();
  CollisionManager::setDimensionsProperty(dimensions_);
  auto &barriers = CollisionManager::getBarriersProperty();
  barriers.clear();
  barriers.emplace_back(dimensions_.X, dimensions_.Y, dimensions_.Width,
                        barrierSize_);
  barriers.emplace_back(dimensions_.X, dimensions_.Y + dimensions_.Height,
                        dimensions_.Width, barrierSize_);
  barriers.emplace_back(dimensions_.X, dimensions_.Y, barrierSize_,
                        dimensions_.Height);
  barriers.emplace_back(dimensions_.X + dimensions_.Width, dimensions_.Y,
                        barrierSize_, dimensions_.Height);

  cornerBarriers_.clear();
  cornerBarriers_.emplace_back(dimensions_.X, dimensions_.Y, barrierSize_,
                               barrierSize_);
  cornerBarriers_.emplace_back(dimensions_.X + dimensions_.Width, dimensions_.Y,
                               barrierSize_, barrierSize_);
  cornerBarriers_.emplace_back(dimensions_.X,
                               dimensions_.Y + dimensions_.Height,
                               barrierSize_, barrierSize_);
  cornerBarriers_.emplace_back(dimensions_.X + dimensions_.Width,
                               dimensions_.Y + dimensions_.Height,
                               barrierSize_, barrierSize_);

  verticalBarriers_.clear();
  for (int i = 1; i < barrierCounts_.Y; ++i) {
    verticalBarriers_.emplace_back(
        dimensions_.X, dimensions_.Y + barrierSize_ * i, barrierSize_,
        barrierSize_);
    verticalBarriers_.emplace_back(dimensions_.X + dimensions_.Width,
                                   dimensions_.Y + barrierSize_ * i,
                                   barrierSize_, barrierSize_);
  }

  horizontalBarriers_.clear();
  for (int i = 1; i < barrierCounts_.X; ++i) {
    horizontalBarriers_.emplace_back(
        dimensions_.X + barrierSize_ * i, dimensions_.Y, barrierSize_,
        barrierSize_);
    horizontalBarriers_.emplace_back(
        dimensions_.X + barrierSize_ * i,
        dimensions_.Y + dimensions_.Width, barrierSize_, barrierSize_);
  }
}

World::~World() { Dispose(false); }

bool World::getInitializedProperty() const { return initialized_; }

bool World::getGameWonProperty() const { return gameWon_; }

void World::setGameWonProperty(bool value) { gameWon_ = value; }

int World::getWinnerIndexProperty() const { return winnerIndex_; }

bool World::getGameExitedProperty() const { return gameExited_; }

void World::setGameExitedProperty(bool value) { gameExited_ = value; }

const std::vector<int> &World::getHighScorersProperty() const {
  return highScorers_;
}

const SpriteFont &World::getPlayerFontProperty() const { return *playerFont_; }

void World::GenerateWorld() {
  if (networkSession_ == nullptr ||
      networkSession_->getLocalGamersProperty().getCountProperty() == 0) {
    return;
  }

  packetWriter_.Write(static_cast<SharpRuntime::intcs>(PacketTypes::WorldSetup));
  for (int i = 0; i < MaximumPlayers; ++i) {
    Vector2 position = Vector2::Zero;
    if (i < networkSession_->getAllGamersProperty().getCountProperty()) {
      PlayerData *playerData =
          GetPlayerData(networkSession_->getAllGamersProperty()[i]);
      if (playerData != nullptr && playerData->getShipProperty() != nullptr) {
        Ship *ship = playerData->getShipProperty();
        ship->Initialize();
        position = CollisionManager::FindSpawnPoint(
            ship, ship->getRadiusProperty() * 5.0f);
        ship->setPositionProperty(position);
        ship->setScoreProperty(0);
      }
    }
    packetWriter_.Write(position);
  }

  for (int i = 0; i < static_cast<int>(asteroids_.size()); ++i) {
    float radius = 32.0f;
    switch (RandomMath::getRandomProperty().Next(3)) {
    case 0:
      radius = 32.0f;
      break;
    case 1:
      radius = 60.0f;
      break;
    case 2:
      radius = 96.0f;
      break;
    }

    asteroids_[i] = std::make_unique<Asteroid>(radius);
    packetWriter_.Write(asteroids_[i]->getRadiusProperty());
    asteroids_[i]->setVariationProperty(i % Asteroid::getVariationsProperty());
    packetWriter_.Write(asteroids_[i]->getVariationProperty());
    asteroids_[i]->Initialize();
    asteroids_[i]->setPositionProperty(CollisionManager::FindSpawnPoint(
        asteroids_[i].get(), asteroids_[i]->getRadiusProperty()));
    packetWriter_.Write(asteroids_[i]->getPositionProperty());
    packetWriter_.Write(asteroids_[i]->getVelocityProperty());
  }

  networkSession_->getLocalGamersProperty()[0]->SendData(
      packetWriter_, SendDataOptions::ReliableInOrder);
}

void World::Initialize() {
  gameWon_ = false;
  winnerIndex_ = -1;
  gameExited_ = false;

  for (int i = 0; i < MaximumPlayers; ++i) {
    const Vector2 position = packetReader_.ReadVector2();
    if (i < networkSession_->getAllGamersProperty().getCountProperty()) {
      PlayerData *playerData =
          GetPlayerData(networkSession_->getAllGamersProperty()[i]);
      if (playerData != nullptr && playerData->getShipProperty() != nullptr) {
        Ship *ship = playerData->getShipProperty();
        ship->setPositionProperty(position);
        ship->setScoreProperty(0);
        ship->Initialize();
      }
    }
  }

  for (auto &asteroid : asteroids_) {
    const float radius = packetReader_.ReadSingle();
    if (asteroid == nullptr) {
      asteroid = std::make_unique<Asteroid>(radius);
    }
    asteroid->setVariationProperty(packetReader_.ReadInt32());
    asteroid->setPositionProperty(packetReader_.ReadVector2());
    asteroid->Initialize();
    asteroid->setVelocityProperty(packetReader_.ReadVector2());
  }
  initialized_ = true;
}

void World::Update(float elapsedTime, bool paused) {
  if (gameWon_) {
    particleEffectManager_->Update(elapsedTime);
    CollisionManager::getCollectionProperty().ApplyPendingRemovals();
    if (!CollisionManager::getCollectionProperty().empty()) {
      CollisionManager::getCollectionProperty().clear();
    }
    return;
  }

  ProcessPackets();
  if (!initialized_ || networkSession_ == nullptr ||
      networkSession_->getSessionStateProperty() !=
          NetworkSessionState::Playing) {
    return;
  }

  int highScore = std::numeric_limits<int>::min();
  int highScoreIndex = -1;
  for (int i = 0;
       i < networkSession_->getAllGamersProperty().getCountProperty(); ++i) {
    PlayerData *playerData =
        GetPlayerData(networkSession_->getAllGamersProperty()[i]);
    if (playerData == nullptr || playerData->getShipProperty() == nullptr) {
      continue;
    }

    const int playerScore = playerData->getShipProperty()->getScoreProperty();
    if (playerScore == highScore) {
      highScorers_.push_back(i);
    } else if (playerScore > highScore) {
      highScorers_.clear();
      highScorers_.push_back(i);
      highScore = playerScore;
      highScoreIndex = i;
    }
  }

  if (networkSession_->getIsHostProperty()) {
    auto *localGamer =
        dynamic_cast<LocalNetworkGamer *>(networkSession_->getHostProperty());
    if (localGamer != nullptr && highScore >= winningScore_) {
      packetWriter_.Write(static_cast<SharpRuntime::intcs>(PacketTypes::GameWon));
      packetWriter_.Write(highScoreIndex);
      localGamer->SendData(packetWriter_, SendDataOptions::ReliableInOrder);
    }

    if (localGamer != nullptr) {
      for (int i = 0;
           i < networkSession_->getAllGamersProperty().getCountProperty();
           ++i) {
        PlayerData *playerData =
            GetPlayerData(networkSession_->getAllGamersProperty()[i]);
        if (playerData != nullptr && playerData->getShipProperty() != nullptr &&
            !playerData->getShipProperty()->getActiveProperty() &&
            playerData->getShipProperty()->getRespawnTimerProperty() <= 0.0f) {
          packetWriter_.Write(
              static_cast<SharpRuntime::intcs>(PacketTypes::ShipSpawn));
          packetWriter_.Write(i);
          packetWriter_.Write(CollisionManager::FindSpawnPoint(
              playerData->getShipProperty(),
              playerData->getShipProperty()->getRadiusProperty()));
          localGamer->SendData(packetWriter_, SendDataOptions::ReliableInOrder);
        }
      }

      if (powerUp_ == nullptr) {
        powerUpTimer_ -= elapsedTime;
        if (powerUpTimer_ < 0.0f) {
          packetWriter_.Write(
              static_cast<SharpRuntime::intcs>(PacketTypes::PowerUpSpawn));
          packetWriter_.Write(RandomMath::getRandomProperty().Next(3));
          packetWriter_.Write(CollisionManager::FindSpawnPoint(
              nullptr, PowerUp::PowerUpRadius * 3.0f));
          localGamer->SendData(packetWriter_, SendDataOptions::ReliableInOrder);
        }
      } else {
        powerUpTimer_ = maximumPowerUpTimer_;
      }

      if (updatesSinceWorldDataSend_ >= updatesBetweenWorldDataSend_) {
        packetWriter_.Write(
            static_cast<SharpRuntime::intcs>(PacketTypes::WorldData));
        for (const auto &asteroid : asteroids_) {
          packetWriter_.Write(asteroid->getPositionProperty());
          packetWriter_.Write(asteroid->getVelocityProperty());
        }
        localGamer->SendData(packetWriter_, SendDataOptions::InOrder);
        updatesSinceWorldDataSend_ = 0;
      } else {
        ++updatesSinceWorldDataSend_;
      }
    }
  }

  for (const auto &asteroid : asteroids_) {
    if (asteroid->getActiveProperty()) {
      asteroid->Update(elapsedTime);
    }
  }

  if (powerUp_ != nullptr) {
    if (powerUp_->getActiveProperty()) {
      powerUp_->Update(elapsedTime);
    } else {
      powerUp_.reset();
    }
  }

  if (!paused) {
    ProcessLocalPlayerInput();
  }

  for (NetworkGamer *networkGamer : networkSession_->getAllGamersProperty()) {
    PlayerData *playerData = GetPlayerData(networkGamer);
    if (playerData == nullptr || playerData->getShipProperty() == nullptr) {
      continue;
    }

    Ship *ship = playerData->getShipProperty();
    if (ship->getActiveProperty()) {
      ship->Update(elapsedTime);
      if (networkGamer->getIsLocalProperty() &&
          ship->getLifeProperty() < 0.0f) {
        SendLocalShipDeath();
      }
    } else if (ship->getRespawnTimerProperty() > 0.0f) {
      ship->setRespawnTimerProperty(
          std::max(ship->getRespawnTimerProperty() - elapsedTime, 0.0f));
    }
  }

  if (updatesSinceStatusPacket_ >= updatesBetweenStatusPackets_) {
    updatesSinceStatusPacket_ = 0;
    SendLocalShipData();
  } else {
    ++updatesSinceStatusPacket_;
  }

  CollisionManager::Update(elapsedTime);
  particleEffectManager_->Update(elapsedTime);
}

void World::ProcessLocalPlayerInput() {
  if (networkSession_ == nullptr ||
      networkSession_->getLocalGamersProperty().getCountProperty() == 0) {
    return;
  }

  LocalNetworkGamer *localGamer = networkSession_->getLocalGamersProperty()[0];
  const PlayerIndex playerIndex =
      localGamer->getSignedInGamerProperty()->getPlayerIndexProperty();
  ShipInput shipInput(GamePad::GetState(playerIndex),
                      Keyboard::GetState(playerIndex));
  shipInput.Serialize(packetWriter_);
  localGamer->SendData(packetWriter_, SendDataOptions::InOrder);
}

void World::SendLocalShipData() {
  if (networkSession_ == nullptr ||
      networkSession_->getLocalGamersProperty().getCountProperty() == 0) {
    return;
  }

  LocalNetworkGamer *localGamer = networkSession_->getLocalGamersProperty()[0];
  PlayerData *playerData = GetPlayerData(localGamer);
  if (playerData == nullptr || playerData->getShipProperty() == nullptr) {
    return;
  }

  Ship *ship = playerData->getShipProperty();
  packetWriter_.Write(static_cast<SharpRuntime::intcs>(PacketTypes::ShipData));
  packetWriter_.Write(ship->getPositionProperty());
  packetWriter_.Write(ship->getVelocityProperty());
  packetWriter_.Write(ship->getRotationProperty());
  packetWriter_.Write(ship->getLifeProperty());
  packetWriter_.Write(ship->getShieldProperty());
  packetWriter_.Write(ship->getScoreProperty());
  localGamer->SendData(packetWriter_, SendDataOptions::InOrder);
}

void World::SendLocalShipDeath() {
  if (networkSession_ == nullptr ||
      networkSession_->getLocalGamersProperty().getCountProperty() == 0) {
    return;
  }

  LocalNetworkGamer *localGamer = networkSession_->getLocalGamersProperty()[0];
  PlayerData *playerData = GetPlayerData(localGamer);
  if (playerData == nullptr || playerData->getShipProperty() == nullptr) {
    return;
  }

  packetWriter_.Write(static_cast<SharpRuntime::intcs>(PacketTypes::ShipDeath));
  int lastDamagedByPlayer = -1;
  auto *lastDamagedByShip = dynamic_cast<Ship *>(
      playerData->getShipProperty()->getLastDamagedByProperty());
  if (lastDamagedByShip != nullptr &&
      lastDamagedByShip != playerData->getShipProperty()) {
    for (int i = 0;
         i < networkSession_->getAllGamersProperty().getCountProperty(); ++i) {
      PlayerData *sourcePlayerData =
          GetPlayerData(networkSession_->getAllGamersProperty()[i]);
      if (sourcePlayerData != nullptr &&
          sourcePlayerData->getShipProperty() == lastDamagedByShip) {
        lastDamagedByPlayer = i;
        break;
      }
    }
  }
  packetWriter_.Write(lastDamagedByPlayer);
  localGamer->SendData(packetWriter_, SendDataOptions::ReliableInOrder);
}

void World::ProcessPackets() {
  if (networkSession_ == nullptr ||
      networkSession_->getLocalGamersProperty().getCountProperty() == 0) {
    return;
  }

  LocalNetworkGamer *localGamer = networkSession_->getLocalGamersProperty()[0];
  while (localGamer->getIsDataAvailableProperty()) {
    NetworkGamer *sender = nullptr;
    localGamer->ReceiveData(packetReader_, sender);
    const auto packetType = static_cast<PacketTypes>(packetReader_.ReadInt32());

    switch (packetType) {
    case PacketTypes::PlayerData:
      UpdatePlayerData(sender);
      break;

    case PacketTypes::WorldSetup:
      if (!initialized_) {
        Initialize();
      }
      break;

    case PacketTypes::ShipData:
      if (sender != nullptr && !sender->getIsLocalProperty()) {
        UpdateShipData(sender);
      }
      break;

    case PacketTypes::WorldData:
      if (!networkSession_->getIsHostProperty() && initialized_) {
        UpdateWorldData();
      }
      break;

    case PacketTypes::ShipInput:
      if (PlayerData *playerData = GetPlayerData(sender);
          playerData != nullptr && playerData->getShipProperty() != nullptr) {
        playerData->getShipProperty()->setShipInputProperty(
            ShipInput(packetReader_));
      }
      break;

    case PacketTypes::ShipSpawn:
      SpawnShip();
      break;

    case PacketTypes::PowerUpSpawn:
      SpawnPowerup();
      break;

    case PacketTypes::ShipDeath:
      KillShip(sender);
      break;

    case PacketTypes::GameWon:
      gameWon_ = true;
      winnerIndex_ = packetReader_.ReadInt32();
      if (networkSession_->getIsHostProperty() &&
          networkSession_->getSessionStateProperty() ==
              NetworkSessionState::Playing) {
        networkSession_->EndGame();
      }
      break;
    }
  }
}

void World::SpawnShip() {
  const int whichGamer = packetReader_.ReadInt32();
  if (whichGamer >=
      networkSession_->getAllGamersProperty().getCountProperty()) {
    return;
  }

  PlayerData *playerData =
      GetPlayerData(networkSession_->getAllGamersProperty()[whichGamer]);
  if (playerData != nullptr && playerData->getShipProperty() != nullptr) {
    playerData->getShipProperty()->setPositionProperty(
        packetReader_.ReadVector2());
    playerData->getShipProperty()->Initialize();
  }
}

void World::SpawnPowerup() {
  const int whichPowerUp = packetReader_.ReadInt32();
  if (powerUp_ == nullptr) {
    switch (whichPowerUp) {
    case 0:
      powerUp_ = std::make_unique<DoubleLaserPowerUp>();
      break;
    case 1:
      powerUp_ = std::make_unique<TripleLaserPowerUp>();
      break;
    case 2:
      powerUp_ = std::make_unique<RocketPowerUp>();
      break;
    }
  }

  if (powerUp_ != nullptr) {
    powerUp_->setPositionProperty(packetReader_.ReadVector2());
    powerUp_->Initialize();
  }
}

void World::KillShip(NetworkGamer *sender) {
  PlayerData *playerData = GetPlayerData(sender);
  if (playerData == nullptr || playerData->getShipProperty() == nullptr ||
      !playerData->getShipProperty()->getActiveProperty()) {
    return;
  }

  GameplayObject *source = nullptr;
  const int sourcePlayerIndex = packetReader_.ReadInt32();
  if (sourcePlayerIndex >= 0 &&
      sourcePlayerIndex <
          networkSession_->getAllGamersProperty().getCountProperty()) {
    PlayerData *sourcePlayerData = GetPlayerData(
        networkSession_->getAllGamersProperty()[sourcePlayerIndex]);
    source = sourcePlayerData != nullptr ? sourcePlayerData->getShipProperty()
                                         : nullptr;
  }
  playerData->getShipProperty()->Die(source, false);
}

void World::UpdatePlayerData(NetworkGamer *sender) {
  if (networkSession_ == nullptr ||
      networkSession_->getLocalGamersProperty().getCountProperty() == 0 ||
      sender == nullptr) {
    return;
  }

  PlayerData *playerData = GetPlayerData(sender);
  if (playerData == nullptr) {
    return;
  }

  playerData->Deserialize(packetReader_);
  for (LocalNetworkGamer *localNetworkGamer :
       networkSession_->getLocalGamersProperty()) {
    PlayerData *localPlayerData = GetPlayerData(localNetworkGamer);
    if (localPlayerData != nullptr &&
        !Ship::HasUniqueColorIndex(localNetworkGamer, networkSession_)) {
      localPlayerData->setShipColorProperty(Ship::GetNextUniqueColorIndex(
          localPlayerData->getShipColorProperty(), networkSession_));
      packetWriter_.Write(
          static_cast<SharpRuntime::intcs>(PacketTypes::PlayerData));
      localPlayerData->Serialize(packetWriter_);
      networkSession_->getLocalGamersProperty()[0]->SendData(
          packetWriter_, SendDataOptions::ReliableInOrder);
    }
  }
}

void World::UpdateShipData(NetworkGamer *sender) {
  PlayerData *playerData = GetPlayerData(sender);
  if (playerData == nullptr || playerData->getShipProperty() == nullptr) {
    return;
  }

  Ship *ship = playerData->getShipProperty();
  ship->setPositionProperty(packetReader_.ReadVector2());
  ship->setVelocityProperty(packetReader_.ReadVector2());
  ship->setRotationProperty(packetReader_.ReadSingle());
  ship->setLifeProperty(packetReader_.ReadSingle());
  ship->setShieldProperty(packetReader_.ReadSingle());
  ship->setScoreProperty(packetReader_.ReadInt32());
}

void World::UpdateWorldData() {
  for (const auto &asteroid : asteroids_) {
    asteroid->setPositionProperty(packetReader_.ReadVector2());
    asteroid->setVelocityProperty(packetReader_.ReadVector2());
  }
}

void World::Draw(float elapsedTime, Vector2 center) {
  const Matrix transform =
      Matrix::CreateTranslation(Vector3(-center.X, -center.Y, 0.0f));
  spriteBatch_->Begin(SpriteSortMode::Deferred, BlendState::NonPremultiplied,
                      nullptr, nullptr, nullptr, nullptr, transform);

  for (const Rectangle &rectangle : cornerBarriers_) {
    spriteBatch_->Draw(*cornerBarrierTexture_, rectangle, Color::White);
  }
  for (const Rectangle &rectangle : verticalBarriers_) {
    spriteBatch_->Draw(*verticalBarrierTexture_, rectangle, Color::White);
  }
  for (const Rectangle &rectangle : horizontalBarriers_) {
    spriteBatch_->Draw(*horizontalBarrierTexture_, rectangle, Color::White);
  }
  for (const auto &asteroid : asteroids_) {
    if (asteroid->getActiveProperty()) {
      asteroid->Draw(elapsedTime, *spriteBatch_);
    }
  }
  if (powerUp_ != nullptr && powerUp_->getActiveProperty()) {
    powerUp_->Draw(elapsedTime, *spriteBatch_);
  }
  for (NetworkGamer *networkGamer : networkSession_->getAllGamersProperty()) {
    PlayerData *playerData = GetPlayerData(networkGamer);
    if (playerData != nullptr && playerData->getShipProperty() != nullptr &&
        playerData->getShipProperty()->getActiveProperty()) {
      playerData->getShipProperty()->Draw(elapsedTime, *spriteBatch_);
    }
  }
  particleEffectManager_->Draw(*spriteBatch_, SpriteBlendMode::AlphaBlend);
  spriteBatch_->End();

  spriteBatch_->Begin(SpriteSortMode::Texture, BlendState::Additive, nullptr,
                      nullptr, nullptr, nullptr, transform);
  particleEffectManager_->Draw(*spriteBatch_, SpriteBlendMode::Additive);
  spriteBatch_->End();
}

void World::DrawPlayerData(float totalTime, NetworkGamer &networkGamer,
                           Vector2 position, SpriteBatch &spriteBatch,
                           bool lobby) {
  PlayerData *playerData = GetPlayerData(&networkGamer);
  if (playerData == nullptr) {
    return;
  }

  float playerStringScale = 1.0f;
  if (networkGamer.getIsLocalProperty()) {
    playerStringScale = 1.0f + 0.08f * (1.0f + std::sin(totalTime * 4.0f));
  }

  const std::string playerString = networkGamer.getGamertagProperty();
  const Color playerColor =
      playerData->getShipProperty() == nullptr
          ? Ship::ShipColors[playerData->getShipColorProperty()]
          : playerData->getShipProperty()->getColorProperty();
  const Vector2 playerStringSize = playerFont_->MeasureString(playerString);
  const Vector2 playerStringPosition = position;
  spriteBatch.DrawString(
      *playerFont_, playerString, playerStringPosition, playerColor, 0.0f,
      Vector2(playerStringSize.X / 2.0f, playerStringSize.Y / 2.0f),
      playerStringScale, SpriteEffects::None, 0.0f);

  Texture2D *chatTexture = nullptr;
  if (networkGamer.getIsMutedByLocalUserProperty()) {
    chatTexture = &*chatMuteTexture_;
  } else if (networkGamer.getIsTalkingProperty()) {
    chatTexture = &*chatTalkingTexture_;
  } else if (networkGamer.getHasVoiceProperty()) {
    chatTexture = &*chatAbleTexture_;
  }

  if (chatTexture != nullptr) {
    const float chatTextureScale =
        0.9f * playerStringSize.Y /
        static_cast<float>(chatTexture->getHeightProperty());
    const Vector2 chatTexturePosition(
        playerStringPosition.X - 1.2f * playerStringSize.X / 2.0f -
            1.1f * chatTextureScale *
                static_cast<float>(chatTexture->getWidthProperty()) / 2.0f,
        playerStringPosition.Y);
    spriteBatch.Draw(
        *chatTexture, chatTexturePosition, std::nullopt, Color::White, 0.0f,
        Vector2(static_cast<float>(chatTexture->getWidthProperty()) / 2.0f,
                static_cast<float>(chatTexture->getHeightProperty()) / 2.0f),
        chatTextureScale, SpriteEffects::None, 0.0f);
  }

  if (lobby) {
    if (playerData->getShipProperty() != nullptr) {
      Ship *ship = playerData->getShipProperty();
      const float oldShipShield = ship->getShieldProperty();
      const float oldShipRadius = ship->getRadiusProperty();
      const Vector2 oldShipPosition = ship->getPositionProperty();
      const float oldShipRotation = ship->getRotationProperty();

      ship->setShieldProperty(0.0f);
      ship->setRadiusProperty(0.6f * playerStringSize.Y);
      ship->setPositionProperty(Vector2(playerStringPosition.X +
                                            1.2f * playerStringSize.X / 2.0f +
                                            1.1f * ship->getRadiusProperty(),
                                        playerStringPosition.Y));
      ship->setRotationProperty(0.0f);
      ship->Draw(0.0f, spriteBatch);

      ship->setRotationProperty(oldShipRotation);
      ship->setPositionProperty(oldShipPosition);
      ship->setShieldProperty(oldShipShield);
      ship->setRadiusProperty(oldShipRadius);
    }

    if (readyTexture_.has_value() && networkGamer.getIsReadyProperty()) {
      const float readyTextureScale =
          0.9f * playerStringSize.Y /
          static_cast<float>(readyTexture_->getHeightProperty());
      const float shipRadius =
          playerData->getShipProperty() == nullptr
              ? 0.0f
              : playerData->getShipProperty()->getRadiusProperty();
      const Vector2 readyTexturePosition(
          playerStringPosition.X + 1.2f * playerStringSize.X / 2.0f +
              2.2f * shipRadius +
              1.1f * readyTextureScale *
                  static_cast<float>(readyTexture_->getWidthProperty()) / 2.0f,
          playerStringPosition.Y);
      spriteBatch.Draw(
          *readyTexture_, readyTexturePosition, std::nullopt, Color::White,
          0.0f,
          Vector2(static_cast<float>(readyTexture_->getWidthProperty()) / 2.0f,
                  static_cast<float>(readyTexture_->getHeightProperty()) /
                      2.0f),
          readyTextureScale, SpriteEffects::None, 0.0f);
    }
  } else if (playerData->getShipProperty() != nullptr) {
    Ship *ship = playerData->getShipProperty();
    std::string scoreString;
    if (ship->getActiveProperty()) {
      scoreString = std::to_string(ship->getScoreProperty());
    } else {
      scoreString =
          "Respawning in: " + std::to_string(static_cast<int>(
                                  std::ceil(ship->getRespawnTimerProperty())));
    }

    const Vector2 scoreStringSize = playerFont_->MeasureString(scoreString);
    const Vector2 scoreStringPosition(position.X,
                                      position.Y + 0.9f * playerStringSize.Y);
    spriteBatch.DrawString(
        *playerFont_, scoreString, scoreStringPosition, playerColor, 0.0f,
        Vector2(scoreStringSize.X / 2.0f, scoreStringSize.Y / 2.0f), 1.0f,
        SpriteEffects::None, 0.0f);
  }
}

void World::Dispose() { Dispose(true); }

void World::Dispose(bool disposing) {
  if (disposed_ || !disposing) {
    disposed_ = true;
    return;
  }

  packetReader_.Close();
  packetWriter_.Close();
  if (spriteBatch_ != nullptr) {
    spriteBatch_->Dispose();
    spriteBatch_.reset();
  }

  cornerBarrierTexture_.reset();
  verticalBarrierTexture_.reset();
  horizontalBarrierTexture_.reset();

  Ship::UnloadContent();
  Asteroid::UnloadContent();
  LaserProjectile::UnloadContent();
  MineProjectile::UnloadContent();
  RocketProjectile::UnloadContent();
  DoubleLaserPowerUp::UnloadContent();
  TripleLaserPowerUp::UnloadContent();

  Ship::ParticleEffectManagerInstance = nullptr;
  RocketProjectile::ParticleEffectManagerInstance = nullptr;
  MineProjectile::ParticleEffectManagerInstance = nullptr;
  LaserProjectile::ParticleEffectManagerInstance = nullptr;

  particleEffectManager_->UnregisterParticleEffect(
      ParticleEffectType::MineExplosion);
  particleEffectManager_->UnregisterParticleEffect(
      ParticleEffectType::RocketExplosion);
  particleEffectManager_->UnregisterParticleEffect(
      ParticleEffectType::RocketTrail);
  particleEffectManager_->UnregisterParticleEffect(
      ParticleEffectType::ShipExplosion);
  particleEffectManager_->UnregisterParticleEffect(
      ParticleEffectType::ShipSpawn);
  particleEffectManager_.reset();
  disposed_ = true;
}
} // namespace NetRumble
