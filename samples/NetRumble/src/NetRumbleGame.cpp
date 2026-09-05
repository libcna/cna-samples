// SPDX-License-Identifier: MS-PL
#include "NetRumbleGame.hpp"

#include "AudioManager.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameComponentCollection.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesComponent.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/GamerServices/InviteAcceptedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "ScreenManager/MessageBoxScreen.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/BackgroundScreen.hpp"
#include "Screens/GameplayScreen.hpp"
#include "Screens/LobbyScreen.hpp"
#include "Screens/MainMenuScreen.hpp"
#include "System/Console.hpp"
#include "System/IO/DirectoryInfo.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::GamerServices;
using namespace Microsoft::Xna::Framework::Net;

NetRumbleGame::NetRumbleGame() {
  graphics_ = std::make_unique<GraphicsDeviceManager>(this);
  graphics_->setPreferredBackBufferWidthProperty(1280);
  graphics_->setPreferredBackBufferHeightProperty(720);
  getContentProperty().setRootDirectoryProperty("Content");

  gamerServices_ = std::make_unique<GamerServicesComponent>(*this);
  getComponentsProperty().Add(gamerServices_.get());

  screenManager_ = std::make_unique<ScreenManager>(*this);
  getComponentsProperty().Add(screenManager_.get());

  AudioManager::Initialize(*this,
                           System::IO::DirectoryInfo("Content/Audio/wav"));
}

NetRumbleGame::~NetRumbleGame() {
  if (inviteAcceptedToken_.has_value()) {
    NetworkSession::InviteAccepted.Remove(*inviteAcceptedToken_);
  }
}

void NetRumbleGame::Initialize() {
  Game::Initialize();
  screenManager_->AddScreen(std::make_shared<BackgroundScreen>());
  screenManager_->AddScreen(std::make_shared<MainMenuScreen>());
  inviteAcceptedToken_ = NetworkSession::InviteAccepted.Add(
      [this](System::Object *sender, const InviteAcceptedEventArgs &e) {
        NetworkSessionInviteAccepted(sender, e);
      });
}

void NetRumbleGame::NetworkSessionInviteAccepted(
    System::Object *, const InviteAcceptedEventArgs &e) {
  if (Guide::getIsTrialModeProperty()) {
    screenManager_->invited = e.getGamerProperty();
    screenManager_->AddScreen(std::make_shared<MessageBoxScreen>(
        "Need to unlock full version before you can accept this invite."));
    System::Console::WriteLine(
        "Cannot accept invite yet because we're in trial mode");
    return;
  }

  std::shared_ptr<MainMenuScreen> mainMenu;
  const std::vector<std::shared_ptr<GameScreen>> screens =
      screenManager_->GetScreens();

  // C++ has no direct equivalent of the sample's reflection call to EndSession.
  // End borrowed gameplay views before the lobby disposes its owned session.
  for (const auto &screen : screens) {
    if (auto gameplay = std::dynamic_pointer_cast<GameplayScreen>(screen)) {
      gameplay->EndSession();
    }
  }

  for (const auto &screen : screens) {
    if (std::dynamic_pointer_cast<BackgroundScreen>(screen)) {
      continue;
    }
    if (auto candidate = std::dynamic_pointer_cast<MainMenuScreen>(screen)) {
      mainMenu = std::move(candidate);
      continue;
    }
    if (auto lobby = std::dynamic_pointer_cast<LobbyScreen>(screen)) {
      lobby->EndSession();
    }
    screen->ExitScreen();
    screenManager_->RemoveScreen(screen.get());
  }

  if (mainMenu != nullptr) {
    mainMenu->JoinInvitedGame();
  }
}

void NetRumbleGame::Draw(const GameTime &gameTime) {
  getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);
  Game::Draw(gameTime);
}

const std::string &NetRumbleGame::GetTypeName() const {
  static const std::string name = "NetRumble.NetRumbleGame";
  return name;
}
} // namespace NetRumble
