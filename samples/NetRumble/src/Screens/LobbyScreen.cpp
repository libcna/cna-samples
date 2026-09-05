// SPDX-License-Identifier: MS-PL
#include "Screens/LobbyScreen.hpp"

#include <any>
#include <array>

#include "Gameplay/CollisionManager.hpp"
#include "Gameplay/PlayerData.hpp"
#include "Gameplay/Ship.hpp"
#include "Gameplay/World.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerPresence.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerPresenceMode.hpp"
#include "Microsoft/Xna/Framework/GamerServices/NetworkException.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Net/GameStartedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/GamerJoinedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/LocalNetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionEndedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionState.hpp"
#include "Microsoft/Xna/Framework/Net/SendDataOptions.hpp"
#include "ScreenManager/InputState.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/GameplayScreen.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/Console.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::GamerServices;
using namespace Microsoft::Xna::Framework::Graphics;
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

LobbyScreen::LobbyScreen(NetworkSession *networkSession)
    : networkSession_(networkSession) {
  if (networkSession_ == nullptr) {
    throw System::ArgumentNullException("networkSession");
  }

  getMenuEntriesProperty().push_back("");
  setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(1.0));
  setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(0.0));
}

LobbyScreen::~LobbyScreen() { Dispose(false); }

void LobbyScreen::LoadContent() {
  MenuScreen::LoadContent();
  world_ = std::make_unique<World>(
      getScreenManagerProperty().getGraphicsDevicePublicProperty(),
      getScreenManagerProperty().getContentProperty(), *networkSession_);

  gamerJoinedToken_ = networkSession_->GamerJoined.Add(
      [this](System::Object *sender, const GamerJoinedEventArgs &e) {
        NetworkSessionGamerJoined(sender, e);
      });
  gameStartedToken_ = networkSession_->GameStarted.Add(
      [this](System::Object *sender, const GameStartedEventArgs &e) {
        NetworkSessionGameStarted(sender, e);
      });
  sessionEndedToken_ = networkSession_->SessionEnded.Add(
      [this](System::Object *sender, const NetworkSessionEndedEventArgs &e) {
        NetworkSessionSessionEnded(sender, e);
      });
}

void LobbyScreen::Update(GameTime &gameTime, bool otherScreenHasFocus,
                         bool coveredByOtherScreen) {
  if (networkSession_ != nullptr) {
    try {
      networkSession_->Update();
    } catch (const NetworkException &exception) {
      System::Console::WriteLine("Network failed to update:  " +
                                 exception.getMessageProperty());
      DisposeSession();
    }
  }

  if (world_ != nullptr && !otherScreenHasFocus && !coveredByOtherScreen) {
    if (world_->getGameWonProperty()) {
      world_->Dispose();
      world_.reset();
      for (NetworkGamer *networkGamer :
           networkSession_->getAllGamersProperty()) {
        PlayerData *playerData = GetPlayerData(networkGamer);
        if (playerData != nullptr && playerData->getShipProperty() != nullptr) {
          playerData->getShipProperty()->Die(nullptr, true);
        }
      }
      CollisionManager::getCollectionProperty().ApplyPendingRemovals();
      world_ = std::make_unique<World>(
          getScreenManagerProperty().getGraphicsDevicePublicProperty(),
          getScreenManagerProperty().getContentProperty(), *networkSession_);
    } else if (world_->getGameExitedProperty()) {
      if (!getIsExitingProperty()) {
        ExitScreen();
      }
      world_->Dispose();
      world_.reset();
      DisposeSession();
      MenuScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
      return;
    } else {
      world_->Update(
          static_cast<float>(
              gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty()),
          true);
    }
  }

  if (!otherScreenHasFocus && networkSession_ != nullptr) {
    if (networkSession_->getLocalGamersProperty().getCountProperty() > 0 &&
        networkSession_->getSessionStateProperty() ==
            NetworkSessionState::Lobby) {
      LocalNetworkGamer *localGamer =
          networkSession_->getLocalGamersProperty()[0];
      if (!localGamer->getIsReadyProperty()) {
        getMenuEntriesProperty()[0] = "Press X to Mark as Ready";
      } else if (!networkSession_->getIsEveryoneReadyProperty()) {
        getMenuEntriesProperty()[0] =
            "Waiting for all players to mark as ready...";
      } else if (!networkSession_->getIsHostProperty()) {
        getMenuEntriesProperty()[0] = "Waiting for the host to start game...";
      } else {
        getMenuEntriesProperty()[0] = "Starting the game...";
        networkSession_->StartGame();
      }
    } else if (networkSession_->getSessionStateProperty() ==
               NetworkSessionState::Playing) {
      getMenuEntriesProperty()[0] = "Game starting...";
    }

    if (networkSession_->getSessionStateProperty() ==
            NetworkSessionState::Playing &&
        world_ != nullptr && world_->getInitializedProperty()) {
      auto gameplayScreen =
          std::make_shared<GameplayScreen>(*networkSession_, *world_);
      gameplayScreen->setScreenManagerProperty(getScreenManagerProperty());
      getScreenManagerProperty().AddScreen(gameplayScreen);
    }
  }

  MenuScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
}

void LobbyScreen::HandleInput(InputState &input) {
  if (networkSession_ != nullptr &&
      networkSession_->getLocalGamersProperty().getCountProperty() > 0) {
    LocalNetworkGamer *localGamer =
        networkSession_->getLocalGamersProperty()[0];
    if (input.getMarkReadyProperty()) {
      localGamer->setIsReadyProperty(!localGamer->getIsReadyProperty());
    }

    PlayerData *playerData = GetPlayerData(localGamer);
    if (playerData != nullptr) {
      bool playerDataChanged = false;
      if (input.getShipColorChangeUpProperty()) {
        playerData->setShipColorProperty(Ship::GetNextUniqueColorIndex(
            playerData->getShipColorProperty(), networkSession_));
        playerDataChanged = true;
      } else if (input.getShipColorChangeDownProperty()) {
        playerData->setShipColorProperty(Ship::GetPreviousUniqueColorIndex(
            playerData->getShipColorProperty(), networkSession_));
        playerDataChanged = true;
      }

      if (input.getShipModelChangeUpProperty()) {
        playerData->setShipVariationProperty(static_cast<SharpRuntime::bytecs>(
            (playerData->getShipVariationProperty() + 1) % 4));
        playerDataChanged = true;
      } else if (input.getShipModelChangeDownProperty()) {
        playerData->setShipVariationProperty(
            playerData->getShipVariationProperty() == 0
                ? static_cast<SharpRuntime::bytecs>(3)
                : static_cast<SharpRuntime::bytecs>(
                      playerData->getShipVariationProperty() -
                                      1));
        playerDataChanged = true;
      }

      if (playerDataChanged) {
        packetWriter_.Write(
            static_cast<SharpRuntime::intcs>(World::PacketTypes::PlayerData));
        playerData->Serialize(packetWriter_);
        localGamer->SendData(packetWriter_, SendDataOptions::ReliableInOrder);
      }
    }
  }
  MenuScreen::HandleInput(input);
}

void LobbyScreen::OnSelectEntry(int) {}

void LobbyScreen::EndSession() { DisposeSession(); }

void LobbyScreen::ExitScreen() {
  if (!getIsExitingProperty() && networkSession_ != nullptr) {
    if (gamerJoinedToken_.has_value()) {
      networkSession_->GamerJoined.Remove(*gamerJoinedToken_);
      gamerJoinedToken_.reset();
    }
    if (gameStartedToken_.has_value()) {
      networkSession_->GameStarted.Remove(*gameStartedToken_);
      gameStartedToken_.reset();
    }
    if (sessionEndedToken_.has_value()) {
      networkSession_->SessionEnded.Remove(*sessionEndedToken_);
      sessionEndedToken_.reset();
    }
  }
  MenuScreen::ExitScreen();
}

void LobbyScreen::UpdatePresence() {
  if (getIsExitingProperty() || networkSession_ == nullptr) {
    return;
  }

  for (LocalNetworkGamer *localGamer :
       networkSession_->getLocalGamersProperty()) {
    SignedInGamer *signedInGamer = localGamer->getSignedInGamerProperty();
    if (signedInGamer->getIsSignedInToLiveProperty()) {
      signedInGamer->getPresenceProperty().setPresenceModeProperty(
          networkSession_->getIsHostProperty()
              ? GamerPresenceMode::WaitingForPlayers
              : GamerPresenceMode::WaitingInLobby);
    }
  }
}

void LobbyScreen::Draw(const GameTime &gameTime) {
  ScreenManager &screenManager = getScreenManagerProperty();
  const auto &viewport =
      screenManager.getGraphicsDevicePublicProperty().getViewportProperty();
  std::array<Vector2, 4> columnPositions = {
      Vector2(viewport.getWidthProperty() * 0.2f,
              viewport.getHeightProperty() * 0.70f),
      Vector2(viewport.getWidthProperty() * 0.4f,
              viewport.getHeightProperty() * 0.70f),
      Vector2(viewport.getWidthProperty() * 0.6f,
              viewport.getHeightProperty() * 0.70f),
      Vector2(viewport.getWidthProperty() * 0.8f,
              viewport.getHeightProperty() * 0.70f),
  };

  SpriteBatch &spriteBatch = screenManager.getSpriteBatchProperty();
  spriteBatch.Begin();
  if (networkSession_ != nullptr && world_ != nullptr) {
    for (int i = 0;
         i < networkSession_->getAllGamersProperty().getCountProperty(); ++i) {
      world_->DrawPlayerData(
          static_cast<float>(
              gameTime.getTotalGameTimeProperty().getTotalSecondsProperty()),
          *networkSession_->getAllGamersProperty()[i], columnPositions[i % 4],
          spriteBatch, true);
      columnPositions[i % 4].Y += viewport.getHeightProperty() * 0.03f;
    }
  }

  const Rectangle titleSafeArea = screenManager.getTitleSafeAreaProperty();
  spriteBatch.DrawString(
      screenManager.getFontProperty(),
      "Press X to mark/unmark ready, LB/RB to toggle color, LT/RT to toggle "
      "ship",
      Vector2(static_cast<float>(titleSafeArea.X),
              static_cast<float>(
                  titleSafeArea.Y + titleSafeArea.Height -
                  screenManager.getFontProperty().getLineSpacingProperty())),
      Color::White);
  spriteBatch.End();
  MenuScreen::Draw(gameTime);
}

void LobbyScreen::OnCancel() {
  if (!getIsExitingProperty()) {
    ExitScreen();
  }
  if (world_ != nullptr) {
    world_->Dispose();
    world_.reset();
  }
  DisposeSession();
}

void LobbyScreen::NetworkSessionSessionEnded(
    System::Object *, const NetworkSessionEndedEventArgs &) {
  OnCancel();
}

void LobbyScreen::NetworkSessionGameStarted(System::Object *,
                                            const GameStartedEventArgs &) {
  if (networkSession_ != nullptr && networkSession_->getIsHostProperty() &&
      world_ != nullptr) {
    world_->GenerateWorld();
  }
}

void LobbyScreen::NetworkSessionGamerJoined(System::Object *,
                                            const GamerJoinedEventArgs &e) {
  NetworkGamer *joinedGamer = e.getGamerProperty();
  for (int i = 0;
       i < networkSession_->getAllGamersProperty().getCountProperty(); ++i) {
    if (networkSession_->getAllGamersProperty()[i] == joinedGamer) {
      auto playerData = std::make_unique<PlayerData>();
      PlayerData *playerDataPointer = playerData.get();
      playerDataOwners_.push_back(std::move(playerData));
      joinedGamer->setTagProperty(std::any(playerDataPointer));
      playerDataPointer->setShipVariationProperty(
          static_cast<SharpRuntime::bytecs>(i % 4));
      playerDataPointer->setShipColorProperty(
          static_cast<SharpRuntime::bytecs>(i));
    }
  }

  if (networkSession_->getLocalGamersProperty().getCountProperty() > 0 &&
      !joinedGamer->getIsLocalProperty()) {
    PlayerData *playerData =
        GetPlayerData(networkSession_->getLocalGamersProperty()[0]);
    if (playerData != nullptr) {
      packetWriter_.Write(
          static_cast<SharpRuntime::intcs>(World::PacketTypes::PlayerData));
      playerData->Serialize(packetWriter_);
      networkSession_->getLocalGamersProperty()[0]->SendData(
          packetWriter_, SendDataOptions::ReliableInOrder, joinedGamer);
    }
  }
}

void LobbyScreen::Dispose() { Dispose(true); }

void LobbyScreen::Dispose(bool disposing) {
  if (disposed_ || !disposing) {
    disposed_ = true;
    return;
  }

  if (world_ != nullptr) {
    world_->Dispose();
    world_.reset();
  }
  packetWriter_.Close();
  DisposeSession();
  disposed_ = true;
}

void LobbyScreen::DisposeSession() {
  if (networkSession_ != nullptr) {
    networkSession_->Dispose();
    delete networkSession_;
    networkSession_ = nullptr;
  }
}

const std::string &LobbyScreen::GetTypeName() const {
  static const std::string name = "NetRumble.LobbyScreen";
  return name;
}
} // namespace NetRumble
