// SPDX-License-Identifier: MS-PL
#include "Screens/MainMenuScreen.hpp"

#include <any>

#include "Gameplay/World.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Gamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerPrivilegeException.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerPresence.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerPresenceMode.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamerCollection.hpp"
#include "Microsoft/Xna/Framework/Net/AvailableNetworkSessionCollection.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/GamerServices/NetworkException.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionJoinError.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionJoinException.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionProperties.hpp"
#include "ScreenManager/MessageBoxScreen.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/LobbyScreen.hpp"
#include "Screens/NetworkBusyScreen.hpp"
#include "Screens/SearchResultsScreen.hpp"
#include "System/AsyncCallback.hpp"
#include "System/Console.hpp"
#include "System/Exception.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::GamerServices;
using namespace Microsoft::Xna::Framework::Net;

namespace {
std::string NetworkSessionJoinErrorName(NetworkSessionJoinError value) {
  switch (value) {
  case NetworkSessionJoinError::SessionNotFound:
    return "SessionNotFound";
  case NetworkSessionJoinError::SessionNotJoinable:
    return "SessionNotJoinable";
  case NetworkSessionJoinError::SessionFull:
    return "SessionFull";
  }
  return std::to_string(static_cast<SharpRuntime::intcs>(value));
}
} // namespace

MainMenuScreen::MainMenuScreen() {
  setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(1.0));
  setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(0.0));
  trialMode_ = Guide::getIsTrialModeProperty();
  updateState_ = false;
}

MainMenuScreen::MainMenuState MainMenuScreen::getStateProperty() const {
  return state_;
}

void MainMenuScreen::setStateProperty(MainMenuState value) {
  if (state_ == value && trialMode_ == Guide::getIsTrialModeProperty() &&
      !updateState_) {
    return;
  }

  updateState_ = false;
  state_ = value;
  trialMode_ = Guide::getIsTrialModeProperty();
  auto &menuEntries = getMenuEntriesProperty();

  switch (state_) {
  case MainMenuState::SignedInLive:
    menuEntries.clear();
    menuEntries.push_back("Quick Match");
    menuEntries.push_back("Create Xbox LIVE Session");
    menuEntries.push_back("Join Xbox LIVE Session");
    menuEntries.push_back("Create System Link Session");
    menuEntries.push_back("Join System Link Session");
    if (Guide::getIsTrialModeProperty()) {
      menuEntries.push_back("Unlock Full Game");
    } else if (getScreenManagerProperty().invited != nullptr) {
      menuEntries.push_back("Join Invited Game");
    }
    menuEntries.push_back("Exit");
    break;

  case MainMenuState::SignedInLocal:
    menuEntries.clear();
    menuEntries.push_back("Create System Link Session");
    menuEntries.push_back("Join System Link Session");
    menuEntries.push_back("Exit");
    break;

  case MainMenuState::SignedOut:
    menuEntries.clear();
    menuEntries.push_back("Sign In");
    menuEntries.push_back("Exit");
    break;

  case MainMenuState::Empty:
    break;
  }
}

void MainMenuScreen::Update(GameTime &gameTime, bool otherScreenHasFocus,
                            bool coveredByOtherScreen) {
  bool signedIntoLive = false;
  if (Gamer::getSignedInGamersProperty()->getCountProperty() > 0) {
    for (SignedInGamer *signedInGamer : *Gamer::getSignedInGamersProperty()) {
      if (signedInGamer->getIsSignedInToLiveProperty()) {
        signedIntoLive = true;
        break;
      }
    }
    setStateProperty(signedIntoLive ? MainMenuState::SignedInLive
                                    : MainMenuState::SignedInLocal);
  } else {
    setStateProperty(MainMenuState::SignedOut);
  }
  MenuScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
}

void MainMenuScreen::OnSelectEntry(int entryIndex) {
  switch (state_) {
  case MainMenuState::SignedInLive:
    switch (entryIndex) {
    case 0:
      QuickMatchSession();
      break;
    case 1:
      CreateSession(NetworkSessionType::PlayerMatch);
      break;
    case 2:
      FindSession(NetworkSessionType::PlayerMatch);
      break;
    case 3:
      CreateSession(NetworkSessionType::SystemLink);
      break;
    case 4:
      FindSession(NetworkSessionType::SystemLink);
      break;
    case 5:
      if (trialMode_) {
        ShowOffer();
      } else if (getScreenManagerProperty().invited != nullptr) {
        JoinInvitedGame();
      } else {
        OnCancel();
      }
      break;
    case 6:
      OnCancel();
      break;
    }
    break;

  case MainMenuState::SignedInLocal:
    switch (entryIndex) {
    case 0:
      CreateSession(NetworkSessionType::SystemLink);
      break;
    case 1:
      FindSession(NetworkSessionType::SystemLink);
      break;
    case 2:
      OnCancel();
      break;
    }
    break;

  case MainMenuState::SignedOut:
    if (entryIndex == 0 && !Guide::getIsVisibleProperty()) {
      Guide::ShowSignIn(1, false);
    } else if (entryIndex == 1) {
      OnCancel();
    }
    break;

  case MainMenuState::Empty:
    break;
  }
}

void MainMenuScreen::ShowOffer() {
  Guide::ShowMarketplace(
      (*Gamer::getSignedInGamersProperty())[0]->getPlayerIndexProperty());
}

void MainMenuScreen::JoinInvitedGame() {
  try {
    System::IAsyncResult *asyncResult = NetworkSession::BeginJoinInvited(
        1, System::AsyncCallback{}, std::any{});
    auto busyScreen = std::make_shared<NetworkBusyScreen>(
        "Joining the session...", asyncResult);
    busyScreen->OperationCompleted +=
        [this](System::Object *sender, const OperationCompletedEventArgs &e) {
          InvitedSessionJoined(sender, e);
        };
    getScreenManagerProperty().AddScreen(busyScreen);
  } catch (...) {
  }

  getScreenManagerProperty().invited = nullptr;
  updateState_ = true;
}

void MainMenuScreen::OnCancel() {
  auto messageBox = std::make_shared<MessageBoxScreen>("Exit Net Rumble?");
  messageBox->Accepted +=
      [this](System::Object *sender, const System::EventArgs &e) {
        ExitMessageBoxAccepted(sender, e);
      };
  getScreenManagerProperty().AddScreen(messageBox);
}

void MainMenuScreen::ExitMessageBoxAccepted(System::Object *,
                                            const System::EventArgs &) {
  getScreenManagerProperty().getGamePublicProperty().Exit();
}

void MainMenuScreen::UpdatePresence() {
  for (SignedInGamer *signedInGamer : *Gamer::getSignedInGamersProperty()) {
    if (signedInGamer->getIsSignedInToLiveProperty()) {
      signedInGamer->getPresenceProperty().setPresenceModeProperty(
          GamerPresenceMode::AtMenu);
    }
  }
}

void MainMenuScreen::QuickMatchSession() {
  try {
    System::IAsyncResult *asyncResult = NetworkSession::BeginFind(
        NetworkSessionType::PlayerMatch, 1, NetworkSessionProperties{},
        System::AsyncCallback{}, std::any{});
    auto busyScreen = std::make_shared<NetworkBusyScreen>(
        "Searching for a session...", asyncResult);
    busyScreen->OperationCompleted +=
        [this](System::Object *sender, const OperationCompletedEventArgs &e) {
          QuickMatchSearchCompleted(sender, e);
        };
    getScreenManagerProperty().AddScreen(busyScreen);
  } catch (const NetworkException &exception) {
    ShowFailure("Failed searching for the session.");
    System::Console::WriteLine("Failed to search for session:  " +
                               exception.getMessageProperty());
  } catch (const GamerPrivilegeException &exception) {
    ShowFailure("You do not have permission to search for a session.");
    System::Console::WriteLine(
        "Insufficient privilege to search for session:  " +
        exception.getMessageProperty());
  }
}

void MainMenuScreen::CreateSession(NetworkSessionType sessionType) {
  try {
    System::IAsyncResult *asyncResult =
        NetworkSession::BeginCreate(sessionType, 1, World::MaximumPlayers,
                                    System::AsyncCallback{}, std::any{});
    auto busyScreen = std::make_shared<NetworkBusyScreen>(
        "Creating a session...", asyncResult);
    busyScreen->OperationCompleted +=
        [this](System::Object *sender, const OperationCompletedEventArgs &e) {
          SessionCreated(sender, e);
        };
    getScreenManagerProperty().AddScreen(busyScreen);
  } catch (const NetworkException &exception) {
    ShowFailure("Failed creating the session.");
    System::Console::WriteLine("Failed to create session:  " +
                               exception.getMessageProperty());
  } catch (const GamerPrivilegeException &exception) {
    ShowFailure("You do not have permission to create a session.");
    System::Console::WriteLine(
        "Insufficient privilege to create session:  " +
        exception.getMessageProperty());
  }
}

void MainMenuScreen::FindSession(NetworkSessionType sessionType) {
  auto searchResultsScreen = std::make_shared<SearchResultsScreen>(sessionType);
  searchResultsScreen->setScreenManagerProperty(getScreenManagerProperty());
  getScreenManagerProperty().AddScreen(searchResultsScreen);

  try {
    System::IAsyncResult *asyncResult =
        NetworkSession::BeginFind(sessionType, 1, NetworkSessionProperties{},
                                  System::AsyncCallback{}, std::any{});
    auto busyScreen = std::make_shared<NetworkBusyScreen>(
        "Searching for a session...", asyncResult);
    busyScreen->OperationCompleted +=
        [searchResultsScreen](System::Object *sender,
                              const OperationCompletedEventArgs &e) {
          searchResultsScreen->SessionsFound(sender, e);
        };
    getScreenManagerProperty().AddScreen(busyScreen);
  } catch (const NetworkException &exception) {
    ShowFailure("Failed searching for the session.");
    System::Console::WriteLine("Failed to search for session:  " +
                               exception.getMessageProperty());
  } catch (const GamerPrivilegeException &exception) {
    ShowFailure("You do not have permission to search for a session.");
    System::Console::WriteLine(
        "Insufficient privilege to search for session:  " +
        exception.getMessageProperty());
  }
}

void MainMenuScreen::QuickMatchSearchCompleted(
    System::Object *, const OperationCompletedEventArgs &e) {
  try {
    AvailableNetworkSessionCollection availableSessions =
        NetworkSession::EndFind(e.getAsyncResultProperty());
    if (availableSessions.getCountProperty() > 0) {
      try {
        System::IAsyncResult *asyncResult = NetworkSession::BeginJoin(
            &availableSessions.getItem(0), System::AsyncCallback{}, std::any{});
        auto busyScreen = std::make_shared<NetworkBusyScreen>(
            "Joining the session...", asyncResult);
        busyScreen->OperationCompleted +=
            [this](System::Object *sender,
                   const OperationCompletedEventArgs &args) {
              QuickMatchSessionJoined(sender, args);
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
    } else {
      ShowFailure("No matches were found.");
    }
  } catch (const GamerPrivilegeException &exception) {
    ShowFailure(exception.getMessageProperty());
  }
}

void MainMenuScreen::SessionCreated(System::Object *,
                                    const OperationCompletedEventArgs &e) {
  NetworkSession *networkSession = nullptr;
  try {
    networkSession = NetworkSession::EndCreate(e.getAsyncResultProperty());
  } catch (const NetworkException &exception) {
    ShowFailure("Failed creating the session.");
    System::Console::WriteLine("Failed to create session:  " +
                               exception.getMessageProperty());
  } catch (const GamerPrivilegeException &exception) {
    ShowFailure("You do not have permission to create a session. " +
                exception.getMessageProperty());
    System::Console::WriteLine(
        "Insufficient privilege to create session:  " +
        exception.getMessageProperty());
  }

  if (networkSession != nullptr) {
    networkSession->setAllowHostMigrationProperty(true);
    networkSession->setAllowJoinInProgressProperty(false);
    LoadLobbyScreen(networkSession);
  }
}

void MainMenuScreen::QuickMatchSessionJoined(
    System::Object *, const OperationCompletedEventArgs &e) {
  NetworkSession *networkSession = nullptr;
  try {
    networkSession = NetworkSession::EndJoin(e.getAsyncResultProperty());
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
  if (networkSession != nullptr) {
    LoadLobbyScreen(networkSession);
  }
}

void MainMenuScreen::LoadLobbyScreen(NetworkSession *networkSession) {
  if (networkSession != nullptr) {
    auto lobbyScreen = std::make_shared<LobbyScreen>(networkSession);
    lobbyScreen->setScreenManagerProperty(getScreenManagerProperty());
    getScreenManagerProperty().AddScreen(lobbyScreen);
  }
}

void MainMenuScreen::InvitedSessionJoined(
    System::Object *, const OperationCompletedEventArgs &e) {
  NetworkSession *networkSession = nullptr;
  try {
    networkSession = NetworkSession::EndJoinInvited(e.getAsyncResultProperty());
  } catch (const NetworkSessionJoinException &exception) {
    ShowFailure("Failed joining the session (" +
                NetworkSessionJoinErrorName(
                    exception.getJoinErrorProperty()) +
                ").");
    System::Console::WriteLine("Failed to join session:  " +
                               exception.getMessageProperty());
  } catch (const System::Exception &exception) {
    ShowFailure("Failed joining the session (" +
                exception.getMessageProperty() + ").");
    System::Console::WriteLine("Failed to join session:  " +
                               exception.getMessageProperty());
  }
  if (networkSession != nullptr) {
    LoadLobbyScreen(networkSession);
  }
}

void MainMenuScreen::FailedMessageBox(System::Object *,
                                      const System::EventArgs &) {}

void MainMenuScreen::ShowFailure(const std::string &message) {
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

const std::string &MainMenuScreen::GetTypeName() const {
  static const std::string name = "NetRumble.MainMenuScreen";
  return name;
}
} // namespace NetRumble
