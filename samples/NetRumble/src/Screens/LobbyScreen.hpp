// SPDX-License-Identifier: MS-PL
#pragma once

#include "CNA/CNAHelper.hpp"

#include <memory>
#include <optional>
#include <vector>

#include "Microsoft/Xna/Framework/Net/PacketWriter.hpp"
#include "ScreenManager/MenuScreen.hpp"
#include "System/IDisposable.hpp"

namespace Microsoft::Xna::Framework::Net {
class GameStartedEventArgs;
class GamerJoinedEventArgs;
class NetworkSession;
class NetworkSessionEndedEventArgs;
} // namespace Microsoft::Xna::Framework::Net

namespace NetRumble {
class PlayerData;
class World;

/** @brief Displays players and controls readiness before a network game. */
class LobbyScreen final : public MenuScreen, public System::IDisposable {
public:
  /** @brief Constructs a lobby that owns the supplied session. @param
   * networkSession Session returned by NetworkSession create/join. */
  explicit LobbyScreen(
      Microsoft::Xna::Framework::Net::NetworkSession *networkSession);

  /** @brief Releases the world, packet writer, and network session. */
  ~LobbyScreen() override;

  /** @brief Creates the gameplay world and subscribes session events. */
  void LoadContent() override;

  /** @brief Updates the session, lobby readiness, and game transition. @param
   * gameTime Timing. @param otherScreenHasFocus Focus state. @param
   * coveredByOtherScreen Coverage state. */
  void Update(Microsoft::Xna::Framework::GameTime &gameTime,
              bool otherScreenHasFocus, bool coveredByOtherScreen) override;

  /** @brief Handles ready, color, and model input. @param input Input snapshot.
   */
  void HandleInput(InputState &input) override;

  /** @brief Disposes the active network session. */
  void EndSession();

  /** @brief Unsubscribes events and exits the screen. */
  void ExitScreen() override;

  /** @brief Updates lobby gamer presence. */
  void UpdatePresence() override;

  /** @brief Draws all lobby players and instructions. @param gameTime Timing.
   */
  void Draw(const Microsoft::Xna::Framework::GameTime &gameTime) override;

  /** @brief Disposes lobby resources. */
  void Dispose() override;

  /** @brief Returns the managed type name. @return Type name. */
  CNAEXT [[nodiscard]] const std::string &GetTypeName() const override;

protected:
  /** @brief Lobby menu selection is intentionally inert. @param entryIndex
   * Ignored entry. */
  void OnSelectEntry(int entryIndex) override;

  /** @brief Leaves and disposes the lobby. */
  void OnCancel() override;

private:
  void NetworkSessionSessionEnded(
      System::Object *sender,
      const Microsoft::Xna::Framework::Net::NetworkSessionEndedEventArgs &e);
  void NetworkSessionGameStarted(
      System::Object *sender,
      const Microsoft::Xna::Framework::Net::GameStartedEventArgs &e);
  void NetworkSessionGamerJoined(
      System::Object *sender,
      const Microsoft::Xna::Framework::Net::GamerJoinedEventArgs &e);
  void Dispose(bool disposing);
  void DisposeSession();

  std::unique_ptr<World> world_;
  Microsoft::Xna::Framework::Net::NetworkSession *networkSession_;
  Microsoft::Xna::Framework::Net::PacketWriter packetWriter_;
  std::vector<std::unique_ptr<PlayerData>> playerDataOwners_;
  std::optional<std::size_t> sessionEndedToken_;
  std::optional<std::size_t> gameStartedToken_;
  std::optional<std::size_t> gamerJoinedToken_;
  bool disposed_{false};
};
} // namespace NetRumble
