// SPDX-License-Identifier: MS-PL
#include "Screens/SearchResultsScreen.hpp"

#include <any>
#include <cmath>

#include "Gameplay/World.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Gamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerPrivilegeException.hpp"
#include "Microsoft/Xna/Framework/GamerServices/NetworkException.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamerCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Net/AvailableNetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "ScreenManager/MessageBoxScreen.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/LobbyScreen.hpp"
#include "Screens/NetworkBusyScreen.hpp"
#include "System/AsyncCallback.hpp"
#include "System/Console.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::GamerServices;
using namespace Microsoft::Xna::Framework::Graphics;
using namespace Microsoft::Xna::Framework::Net;

SearchResultsScreen::SearchResultsScreen(NetworkSessionType sessionType)
    : sessionType_(sessionType) {
  setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(1.0));
  setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(0.0));
}

void SearchResultsScreen::Update(GameTime &gameTime, bool otherScreenHasFocus,
                                 bool coveredByOtherScreen) {
  bool signedIntoLive = false;
  if (Gamer::getSignedInGamersProperty()->getCountProperty() > 0) {
    for (SignedInGamer *signedInGamer : *Gamer::getSignedInGamersProperty()) {
      if (signedInGamer->getIsSignedInToLiveProperty()) {
        signedIntoLive = true;
        break;
      }
    }
    if (!signedIntoLive &&
        (sessionType_ == NetworkSessionType::PlayerMatch ||
         sessionType_ == NetworkSessionType::Ranked) &&
        !getIsExitingProperty()) {
      ExitScreen();
    }
  } else if (!getIsExitingProperty()) {
    ExitScreen();
  }

  if (coveredByOtherScreen && !getIsExitingProperty()) {
    ExitScreen();
  }
  MenuScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
}

void SearchResultsScreen::OnSelectEntry(int entryIndex) {
  if (!availableSessions_.has_value() || entryIndex < 0 ||
      entryIndex >= availableSessions_->getCountProperty()) {
    return;
  }

  try {
    System::IAsyncResult *asyncResult =
        NetworkSession::BeginJoin(&availableSessions_->getItem(entryIndex),
                                  System::AsyncCallback{}, std::any{});
    auto busyScreen = std::make_shared<NetworkBusyScreen>(
        "Joining the session...", asyncResult);
    busyScreen->OperationCompleted +=
        [this](System::Object *sender, const OperationCompletedEventArgs &e) {
          LoadLobbyScreen(sender, e);
        };
    getScreenManagerProperty().AddScreen(busyScreen);
  } catch (const NetworkException &exception) {
    ShowFailure("Failed joining the session.");
    System::Console::WriteLine("Failed to join session:  " +
                               exception.getMessageProperty());
  } catch (const GamerPrivilegeException &exception) {
    ShowFailure("You do not have permission to join a session.");
    System::Console::WriteLine(
        "Insufficient privilege to join session:  " +
        exception.getMessageProperty());
  }
}

void SearchResultsScreen::OnCancel() {
  if (availableSessions_.has_value()) {
    ExitScreen();
  }
}

void SearchResultsScreen::Draw(const GameTime &gameTime) {
  std::string alternateString;
  if (!availableSessions_.has_value()) {
    alternateString = "Searching...";
  } else if (availableSessions_->getCountProperty() <= 0) {
    alternateString = "No sessions found.";
  }

  if (alternateString.empty()) {
    MenuScreen::Draw(gameTime);
    return;
  }

  ScreenManager &screenManager = getScreenManagerProperty();
  const Viewport viewport =
      screenManager.getGraphicsDevicePublicProperty().getViewportProperty();
  const Vector2 viewportSize(viewport.getWidthProperty(),
                             viewport.getHeightProperty());
  Vector2 position(0.0f, viewportSize.Y * 0.65f);
  const float transitionOffset =
      std::pow(getTransitionPositionProperty(), 2.0f);
  position.Y += getScreenStateProperty() == ScreenState::TransitionOn
                    ? transitionOffset * 256.0f
                    : transitionOffset * 512.0f;

  SpriteBatch &spriteBatch = screenManager.getSpriteBatchProperty();
  SpriteFont &font = screenManager.getFontProperty();
  spriteBatch.Begin();
  const Vector2 origin(0.0f, font.getLineSpacingProperty() / 2.0f);
  const Vector2 size = font.MeasureString(alternateString);
  position.X = viewportSize.X / 2.0f - size.X / 2.0f;
  spriteBatch.DrawString(font, alternateString, position, Color::White, 0.0f,
                         origin, 1.0f, SpriteEffects::None, 0.0f);
  spriteBatch.End();
}

void SearchResultsScreen::SessionsFound(System::Object *,
                                        const OperationCompletedEventArgs &e) {
  try {
    availableSessions_.emplace(
        NetworkSession::EndFind(e.getAsyncResultProperty()));
  } catch (const NetworkException &exception) {
    ShowFailure("Failed searching for the session.");
    System::Console::WriteLine("Failed to search for session:  " +
                               exception.getMessageProperty());
  } catch (const GamerPrivilegeException &exception) {
    ShowFailure("You do not have permission to search for a session. " +
                exception.getMessageProperty());
    System::Console::WriteLine(
        "Insufficient privilege to search for session:  " +
        exception.getMessageProperty());
  }

  auto &menuEntries = getMenuEntriesProperty();
  menuEntries.clear();
  if (!availableSessions_.has_value()) {
    return;
  }

  for (const AvailableNetworkSession &availableSession : *availableSessions_) {
    if (availableSession.getCurrentGamerCountProperty() <
        World::MaximumPlayers) {
      menuEntries.push_back(
          availableSession.getHostGamertagProperty() + " (" +
          std::to_string(availableSession.getCurrentGamerCountProperty()) +
          "/" + std::to_string(World::MaximumPlayers) + ")");
    }
    if (menuEntries.size() >= maximumSessions_) {
      break;
    }
  }
}

void SearchResultsScreen::LoadLobbyScreen(
    System::Object *, const OperationCompletedEventArgs &e) {
  NetworkSession *networkSession = nullptr;
  try {
    networkSession = NetworkSession::EndJoin(e.getAsyncResultProperty());
  } catch (const NetworkException &exception) {
    ShowFailure("Failed joining session.");
    System::Console::WriteLine("Failed joining session:  " +
                               exception.getMessageProperty());
  } catch (const GamerPrivilegeException &exception) {
    ShowFailure("You do not have permission to join a session.");
    System::Console::WriteLine(
        "Insufficient privilege to join session:  " +
        exception.getMessageProperty());
  }

  if (networkSession != nullptr) {
    auto lobbyScreen = std::make_shared<LobbyScreen>(networkSession);
    lobbyScreen->setScreenManagerProperty(getScreenManagerProperty());
    getScreenManagerProperty().AddScreen(lobbyScreen);
  }
}

void SearchResultsScreen::FailedMessageBox(System::Object *,
                                           const System::EventArgs &) {
  ExitScreen();
}

void SearchResultsScreen::ShowFailure(const std::string &message) {
  auto messageBox = std::make_shared<MessageBoxScreen>(message);
  messageBox->Accepted +=
      [this](System::Object *sender, const System::EventArgs &e) {
        FailedMessageBox(sender, e);
      };
  messageBox->Cancelled +=
      [this](System::Object *sender, const System::EventArgs &e) {
        FailedMessageBox(sender, e);
      };
  getScreenManagerProperty().AddScreen(messageBox);
}

const std::string &SearchResultsScreen::GetTypeName() const {
  static const std::string name = "NetRumble.SearchResultsScreen";
  return name;
}
} // namespace NetRumble
