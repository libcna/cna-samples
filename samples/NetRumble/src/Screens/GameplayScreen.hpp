// SPDX-License-Identifier: MS-PL
#pragma once

#include "CNA/CNAHelper.hpp"

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "ScreenManager/GameScreen.hpp"
#include "System/EventArgs.hpp"
#include "System/IDisposable.hpp"

namespace Microsoft::Xna::Framework::Net {
class GameEndedEventArgs;
class GamerLeftEventArgs;
class NetworkSession;
class NetworkSessionEndedEventArgs;
} // namespace Microsoft::Xna::Framework::Net

namespace NetRumble {
class BloomComponent;
class Ship;
class Starfield;
class World;

/** @brief Runs and renders an active network game. */
class GameplayScreen final : public GameScreen, public System::IDisposable {
public:
  /** @brief Constructs gameplay over lobby-owned objects. @param networkSession
   * Active session. @param world Active world. */
  GameplayScreen(Microsoft::Xna::Framework::Net::NetworkSession &networkSession,
                 World &world);

  /** @brief Releases graphics components. */
  ~GameplayScreen() override;

  /** @brief Loads bloom, starfield, and music. */
  void LoadContent() override;

  /** @brief Unloads starfield content. */
  void UnloadContent() override;

  /** @brief Updates gameplay and winner state. @param gameTime Timing. @param
   * otherScreenHasFocus Focus state. @param coveredByOtherScreen Coverage
   * state. */
  void Update(Microsoft::Xna::Framework::GameTime &gameTime,
              bool otherScreenHasFocus, bool coveredByOtherScreen) override;

  /** @brief Handles pause and winner confirmation input. @param input Input
   * snapshot. */
  void HandleInput(InputState &input) override;

  /** @brief Disposes the network session when forcibly replaced by an
   * invitation. */
  void EndSession();

  /** @brief Removes bloom, unsubscribes networking, and exits. */
  void ExitScreen() override;

  /** @brief Updates local gamer score presence. */
  void UpdatePresence() override;

  /** @brief Draws the world, postprocess, HUD, and transition. @param gameTime
   * Timing. */
  void Draw(const Microsoft::Xna::Framework::GameTime &gameTime) override;

  /** @brief Disposes owned graphics helpers. */
  void Dispose() override;

  /** @brief Returns the managed type name. @return Type name. */
  CNAEXT [[nodiscard]] const std::string &GetTypeName() const override;

private:
  void ExitMessageBoxAccepted(System::Object *sender,
                              const System::EventArgs &e);
  void DrawHud(float totalTime);
  void NetworkSessionGameEnded(
      System::Object *sender,
      const Microsoft::Xna::Framework::Net::GameEndedEventArgs &e);
  void NetworkSessionSessionEnded(
      System::Object *sender,
      const Microsoft::Xna::Framework::Net::NetworkSessionEndedEventArgs &e);
  void NetworkSessionGamerLeft(
      System::Object *sender,
      const Microsoft::Xna::Framework::Net::GamerLeftEventArgs &e);
  void Dispose(bool disposing);

  World *world_;
  Ship *localShip_{nullptr};
  std::string winnerString_;
  Microsoft::Xna::Framework::Vector2 winnerStringPosition_{};
  std::unique_ptr<BloomComponent> bloomComponent_;
  std::unique_ptr<Starfield> starfield_;
  Microsoft::Xna::Framework::Net::NetworkSession *networkSession_;
  std::optional<std::size_t> sessionEndedToken_;
  std::optional<std::size_t> gameEndedToken_;
  std::optional<std::size_t> gamerLeftToken_;
  bool disposed_{false};
};
} // namespace NetRumble
