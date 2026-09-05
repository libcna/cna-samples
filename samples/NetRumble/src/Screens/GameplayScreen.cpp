// SPDX-License-Identifier: MS-PL
#include "Screens/GameplayScreen.hpp"

#include <algorithm>
#include <any>
#include <cmath>

#include "AudioManager.hpp"
#include "BloomPostprocess/BloomComponent.hpp"
#include "BloomPostprocess/BloomSettings.hpp"
#include "Gameplay/PlayerData.hpp"
#include "Gameplay/Ship.hpp"
#include "Gameplay/World.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameComponentCollection.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerPresence.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerPresenceMode.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Media/MediaPlayer.hpp"
#include "Microsoft/Xna/Framework/Net/GameEndedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/GamerLeftEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/LocalNetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionEndedEventArgs.hpp"
#include "Rendering/Starfield.hpp"
#include "ScreenManager/InputState.hpp"
#include "ScreenManager/MessageBoxScreen.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::GamerServices;
using namespace Microsoft::Xna::Framework::Graphics;
using namespace Microsoft::Xna::Framework::Media;
using namespace Microsoft::Xna::Framework::Net;

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

GameplayScreen::GameplayScreen(NetworkSession &networkSession, World &world)
    : world_(&world), networkSession_(&networkSession) {
  sessionEndedToken_ = networkSession_->SessionEnded.Add(
      [this](System::Object *sender, const NetworkSessionEndedEventArgs &e) {
        NetworkSessionSessionEnded(sender, e);
      });
  gameEndedToken_ = networkSession_->GameEnded.Add(
      [this](System::Object *sender, const GameEndedEventArgs &e) {
        NetworkSessionGameEnded(sender, e);
      });
  gamerLeftToken_ = networkSession_->GamerLeft.Add(
      [this](System::Object *sender, const GamerLeftEventArgs &e) {
        NetworkSessionGamerLeft(sender, e);
      });

  if (networkSession_->getLocalGamersProperty().getCountProperty() > 0) {
    PlayerData *playerData =
        GetPlayerData(networkSession_->getLocalGamersProperty()[0]);
    if (playerData != nullptr) {
      localShip_ = playerData->getShipProperty();
    }
  }

  setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(1.0));
  setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(1.0));
}

GameplayScreen::~GameplayScreen() { Dispose(false); }

void GameplayScreen::LoadContent() {
  ScreenManager &screenManager = getScreenManagerProperty();
  bloomComponent_ =
      std::make_unique<BloomComponent>(screenManager.getGamePublicProperty());
  bloomComponent_->setSettingsProperty(BloomSettings::PresetSettings[0]);
  screenManager.getGamePublicProperty().getComponentsProperty().Add(
      bloomComponent_.get());
  bloomComponent_->Initialize();
  bloomComponent_->setVisibleProperty(false);

  starfield_ = std::make_unique<Starfield>(
      Vector2::Zero, screenManager.getGraphicsDevicePublicProperty(),
      screenManager.getContentProperty());
  starfield_->LoadContent();
  AudioManager::PlaySoundTrack();
  GameScreen::LoadContent();
}

void GameplayScreen::UnloadContent() {
  if (starfield_ != nullptr) {
    starfield_->UnloadContent();
  }
  GameScreen::UnloadContent();
}

void GameplayScreen::Update(GameTime &gameTime, bool otherScreenHasFocus,
                            bool coveredByOtherScreen) {
  GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
  if (networkSession_ == nullptr || world_ == nullptr) {
    if (!getIsExitingProperty()) {
      ExitScreen();
    }
    GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
    return;
  }

  if (otherScreenHasFocus || coveredByOtherScreen) {
    world_->Update(
        static_cast<float>(
            gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty()),
        true);
  } else if (world_->getGameExitedProperty()) {
    if (!getIsExitingProperty()) {
      ExitScreen();
    }
    networkSession_ = nullptr;
    GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
    return;
  } else {
    world_->Update(
        static_cast<float>(
            gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty()),
        false);
    if (world_->getGameWonProperty() && winnerString_.empty() &&
        world_->getWinnerIndexProperty() >= 0 &&
        world_->getWinnerIndexProperty() <
            networkSession_->getAllGamersProperty().getCountProperty()) {
      winnerString_ =
          networkSession_
              ->getAllGamersProperty()[world_->getWinnerIndexProperty()]
              ->getGamertagProperty();
      winnerString_ += " has won the game!\nPress A to return to the lobby.";
      const Vector2 winnerStringSize =
          world_->getPlayerFontProperty().MeasureString(winnerString_);
      const auto &viewport = getScreenManagerProperty()
                                 .getGraphicsDevicePublicProperty()
                                 .getViewportProperty();
      winnerStringPosition_ = Vector2(
          viewport.getXProperty() + viewport.getWidthProperty() / 2.0f -
              std::floor(winnerStringSize.X / 2.0f),
          viewport.getYProperty() + viewport.getHeightProperty() / 2.0f -
              std::floor(winnerStringSize.Y / 2.0f));
    }
  }
}

void GameplayScreen::HandleInput(InputState &input) {
  if (getIsExitingProperty() || world_ == nullptr ||
      world_->getGameExitedProperty()) {
    return;
  }

  if (input.getPauseGameProperty() && !world_->getGameWonProperty()) {
    auto messageBox =
        std::make_shared<MessageBoxScreen>("Exit the game?", false);
    messageBox->Accepted +=
        [this](System::Object *sender, const System::EventArgs &e) {
          ExitMessageBoxAccepted(sender, e);
        };
    getScreenManagerProperty().AddScreen(messageBox);
  }

  if (input.getMenuSelectProperty() && world_->getGameWonProperty()) {
    world_->setGameExitedProperty(true);
    world_ = nullptr;
    if (!getIsExitingProperty()) {
      ExitScreen();
    }
    networkSession_ = nullptr;
  }
}

void GameplayScreen::ExitMessageBoxAccepted(System::Object *,
                                            const System::EventArgs &) {
  if (world_ != nullptr) {
    world_->setGameExitedProperty(true);
    world_ = nullptr;
  }
}

void GameplayScreen::EndSession() {
  if (networkSession_ != nullptr) {
    networkSession_->Dispose();
    networkSession_ = nullptr;
  }
}

void GameplayScreen::ExitScreen() {
  if (bloomComponent_ != nullptr) {
    bloomComponent_->setVisibleProperty(false);
    (void)getScreenManagerProperty()
        .getGamePublicProperty()
        .getComponentsProperty()
        .Remove(bloomComponent_.get());
    bloomComponent_.reset();
  }

  if (!getIsExitingProperty() && networkSession_ != nullptr) {
    if (sessionEndedToken_.has_value()) {
      networkSession_->SessionEnded.Remove(*sessionEndedToken_);
      sessionEndedToken_.reset();
    }
    if (gameEndedToken_.has_value()) {
      networkSession_->GameEnded.Remove(*gameEndedToken_);
      gameEndedToken_.reset();
    }
    if (gamerLeftToken_.has_value()) {
      networkSession_->GamerLeft.Remove(*gamerLeftToken_);
      gamerLeftToken_.reset();
    }
  }
  MediaPlayer::Stop();
  GameScreen::ExitScreen();
}

void GameplayScreen::UpdatePresence() {
  if (getIsExitingProperty() || networkSession_ == nullptr ||
      world_ == nullptr) {
    return;
  }

  const bool isTied = world_->getHighScorersProperty().size() > 1;
  for (int i = 0;
       i < networkSession_->getAllGamersProperty().getCountProperty(); ++i) {
    NetworkGamer *networkGamer = networkSession_->getAllGamersProperty()[i];
    if (!networkGamer->getIsLocalProperty()) {
      continue;
    }

    auto *localGamer = dynamic_cast<LocalNetworkGamer *>(networkGamer);
    SignedInGamer *signedInGamer = localGamer->getSignedInGamerProperty();
    if (!signedInGamer->getIsSignedInToLiveProperty()) {
      continue;
    }

    const bool isHighScorer =
        std::find(world_->getHighScorersProperty().begin(),
                  world_->getHighScorersProperty().end(),
                  i) != world_->getHighScorersProperty().end();
    if (isHighScorer) {
      signedInGamer->getPresenceProperty().setPresenceModeProperty(
          isTied ? GamerPresenceMode::ScoreIsTied : GamerPresenceMode::Winning);
    } else {
      signedInGamer->getPresenceProperty().setPresenceModeProperty(
          GamerPresenceMode::Losing);
    }
  }
}

void GameplayScreen::Draw(const GameTime &gameTime) {
  const float elapsedTime = static_cast<float>(
      gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());
  if (networkSession_ != nullptr) {
    if (localShip_ == nullptr &&
        networkSession_->getLocalGamersProperty().getCountProperty() > 0) {
      PlayerData *playerData =
          GetPlayerData(networkSession_->getLocalGamersProperty()[0]);
      if (playerData != nullptr && playerData->getShipProperty() != nullptr) {
        localShip_ = playerData->getShipProperty();
        starfield_->Reset(localShip_->getPositionProperty());
      }
    }

    if (bloomComponent_ != nullptr) {
      bloomComponent_->BeginDraw();
    }

    if (world_ != nullptr && localShip_ != nullptr && !getIsExitingProperty()) {
      const auto &viewport = getScreenManagerProperty()
                                 .getGraphicsDevicePublicProperty()
                                 .getViewportProperty();
      const Vector2 center(
          localShip_->getPositionProperty().X + viewport.getXProperty() -
              viewport.getWidthProperty() / 2.0f,
          localShip_->getPositionProperty().Y + viewport.getYProperty() -
              viewport.getHeightProperty() / 2.0f);
      starfield_->Draw(center);
      world_->Draw(elapsedTime, center);
      if (bloomComponent_ != nullptr) {
        bloomComponent_->Draw(gameTime);
      }
    }
    DrawHud(static_cast<float>(
        gameTime.getTotalGameTimeProperty().getTotalSecondsProperty()));
  }

  if (getScreenStateProperty() == ScreenState::TransitionOn &&
      getTransitionPositionProperty() > 0.0f) {
    getScreenManagerProperty().FadeBackBufferToBlack(
        255 - getTransitionAlphaProperty());
  }
}

void GameplayScreen::DrawHud(float totalTime) {
  if (networkSession_ == nullptr || world_ == nullptr) {
    return;
  }

  ScreenManager &screenManager = getScreenManagerProperty();
  SpriteBatch &spriteBatch = screenManager.getSpriteBatchProperty();
  const auto &viewport =
      screenManager.getGraphicsDevicePublicProperty().getViewportProperty();
  const int gamerCount =
      networkSession_->getAllGamersProperty().getCountProperty();
  spriteBatch.Begin();

  Vector2 position(viewport.getWidthProperty() * 0.2f,
                   viewport.getHeightProperty() * 0.1f);
  for (int i = 0; i < std::min(4, gamerCount); ++i) {
    world_->DrawPlayerData(totalTime,
                           *networkSession_->getAllGamersProperty()[i],
                           position, spriteBatch, false);
    position.X += viewport.getWidthProperty() * 0.2f;
  }

  position = Vector2(viewport.getWidthProperty() * 0.2f,
                     viewport.getHeightProperty() * 0.9f);
  for (int i = 4; i < std::min(8, gamerCount); ++i) {
    world_->DrawPlayerData(totalTime,
                           *networkSession_->getAllGamersProperty()[i],
                           position, spriteBatch, false);
    position.X += viewport.getWidthProperty() * 0.2f;
  }

  position = Vector2(viewport.getWidthProperty() * 0.13f,
                     viewport.getHeightProperty() * 0.2f);
  for (int i = 8; i < std::min(12, gamerCount); ++i) {
    world_->DrawPlayerData(totalTime,
                           *networkSession_->getAllGamersProperty()[i],
                           position, spriteBatch, false);
    position.Y += viewport.getHeightProperty() * 0.2f;
  }

  position = Vector2(viewport.getWidthProperty() * 0.9f,
                     viewport.getHeightProperty() * 0.2f);
  for (int i = 12; i < std::min(16, gamerCount); ++i) {
    world_->DrawPlayerData(totalTime,
                           *networkSession_->getAllGamersProperty()[i],
                           position, spriteBatch, false);
    position.Y += viewport.getHeightProperty() * 0.2f;
  }

  if (world_->getGameWonProperty() && !winnerString_.empty()) {
    spriteBatch.DrawString(world_->getPlayerFontProperty(), winnerString_,
                           winnerStringPosition_, Color::White, 0.0f,
                           Vector2::Zero, 1.3f, SpriteEffects::None, 0.0f);
  }
  spriteBatch.End();
}

void GameplayScreen::NetworkSessionGameEnded(System::Object *,
                                             const GameEndedEventArgs &) {
  if (world_ != nullptr && !world_->getGameWonProperty() &&
      !world_->getGameExitedProperty()) {
    world_->setGameExitedProperty(true);
  }
  if (!getIsExitingProperty() &&
      (world_ == nullptr || world_->getGameExitedProperty())) {
    world_ = nullptr;
    ExitScreen();
    networkSession_ = nullptr;
  }
}

void GameplayScreen::NetworkSessionSessionEnded(
    System::Object *, const NetworkSessionEndedEventArgs &) {
  if (world_ != nullptr && !world_->getGameExitedProperty()) {
    world_->setGameExitedProperty(true);
    world_ = nullptr;
  }
  if (!getIsExitingProperty()) {
    ExitScreen();
  }
  networkSession_ = nullptr;
}

void GameplayScreen::NetworkSessionGamerLeft(System::Object *,
                                             const GamerLeftEventArgs &e) {
  PlayerData *playerData = GetPlayerData(e.getGamerProperty());
  if (playerData != nullptr && playerData->getShipProperty() != nullptr) {
    playerData->getShipProperty()->Die(nullptr, true);
  }
}

void GameplayScreen::Dispose() { Dispose(true); }

void GameplayScreen::Dispose(bool disposing) {
  if (disposed_ || !disposing) {
    disposed_ = true;
    return;
  }

  if (bloomComponent_ != nullptr) {
    static_cast<Microsoft::Xna::Framework::GameComponent *>(
        bloomComponent_.get())
        ->Dispose();
    bloomComponent_.reset();
  }
  if (starfield_ != nullptr) {
    starfield_->Dispose();
    starfield_.reset();
  }
  disposed_ = true;
}

const std::string &GameplayScreen::GetTypeName() const {
  static const std::string name = "NetRumble.GameplayScreen";
  return name;
}
} // namespace NetRumble
