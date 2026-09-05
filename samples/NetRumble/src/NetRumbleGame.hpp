// SPDX-License-Identifier: MS-PL
#pragma once

#include "CNA/CNAHelper.hpp"

#include <memory>
#include <optional>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"

namespace Microsoft::Xna::Framework::GamerServices {
class GamerServicesComponent;
class InviteAcceptedEventArgs;
} // namespace Microsoft::Xna::Framework::GamerServices

namespace NetRumble {
class ScreenManager;

/** @brief Hosts the Net Rumble screen stack and framework components. */
class NetRumbleGame final : public Microsoft::Xna::Framework::Game {
public:
  /** @brief Constructs and configures the game. */
  NetRumbleGame();

  /** @brief Releases owned components and event subscriptions. */
  ~NetRumbleGame() override;

  /** @brief Returns the managed type name. @return Type name. */
  CNAEXT [[nodiscard]] const std::string &GetTypeName() const override;

protected:
  /** @brief Adds initial screens and subscribes invitation handling. */
  void Initialize() override;

  /** @brief Clears the frame and draws components. @param gameTime Timing. */
  void Draw(const Microsoft::Xna::Framework::GameTime &gameTime) override;

private:
  void NetworkSessionInviteAccepted(
      System::Object *sender,
      const Microsoft::Xna::Framework::GamerServices::InviteAcceptedEventArgs
          &e);

  std::unique_ptr<Microsoft::Xna::Framework::GraphicsDeviceManager> graphics_;
  std::unique_ptr<
      Microsoft::Xna::Framework::GamerServices::GamerServicesComponent>
      gamerServices_;
  std::unique_ptr<ScreenManager> screenManager_;
  std::optional<std::size_t> inviteAcceptedToken_;
};
} // namespace NetRumble
