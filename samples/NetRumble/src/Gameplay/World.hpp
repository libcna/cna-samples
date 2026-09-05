// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <memory>
#include <optional>
#include <vector>

#include "Gameplay/Asteroid.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Net/PacketReader.hpp"
#include "Microsoft/Xna/Framework/Net/PacketWriter.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/IDisposable.hpp"

namespace Microsoft::Xna::Framework::Content {
class ContentManager;
}

namespace Microsoft::Xna::Framework::Graphics {
class GraphicsDevice;
class SpriteBatch;
} // namespace Microsoft::Xna::Framework::Graphics

namespace Microsoft::Xna::Framework::Net {
class NetworkGamer;
class NetworkSession;
} // namespace Microsoft::Xna::Framework::Net

namespace NetRumble {
class ParticleEffectManager;
class PowerUp;

/** @brief Owns the Net Rumble simulation, rendering, and gameplay packet
 * protocol. */
class World final : public System::IDisposable {
public:
  /** @brief Maximum number of players supported by the sample. */
  static constexpr int MaximumPlayers = 16;

  /** @brief Identifies each gameplay packet payload. */
  enum class PacketTypes {
    /** @brief Announces a player's selected ship data. */
    PlayerData,
    /** @brief Synchronizes a ship's simulation state. */
    ShipData,
    /** @brief Initializes the authoritative world. */
    WorldSetup,
    /** @brief Synchronizes world-object state. */
    WorldData,
    /** @brief Carries a local player's ship input. */
    ShipInput,
    /** @brief Announces a power-up spawn. */
    PowerUpSpawn,
    /** @brief Announces a ship death. */
    ShipDeath,
    /** @brief Announces a ship spawn. */
    ShipSpawn,
    /** @brief Announces the winning player. */
    GameWon,
  };

  /**
   * @brief Constructs the world and loads its content.
   * @param graphicsDevice Graphics device.
   * @param contentManager Content manager.
   * @param networkSession Active network session.
   */
  World(Microsoft::Xna::Framework::Graphics::GraphicsDevice &graphicsDevice,
        Microsoft::Xna::Framework::Content::ContentManager &contentManager,
        Microsoft::Xna::Framework::Net::NetworkSession &networkSession);

  /** @brief Releases world resources. */
  ~World() override;

  /** @brief Gets whether the setup packet has initialized the world. @return
   * Initialization state. */
  [[nodiscard]] bool getInitializedProperty() const;

  /** @brief Gets whether a player has won. @return Win state. */
  [[nodiscard]] bool getGameWonProperty() const;

  /** @brief Sets whether a player has won. @param value Win state. */
  void setGameWonProperty(bool value);

  /** @brief Gets the winning gamer index. @return Gamer index, or -1. */
  [[nodiscard]] int getWinnerIndexProperty() const;

  /** @brief Gets whether the game ended without a winner. @return Exit state.
   */
  [[nodiscard]] bool getGameExitedProperty() const;

  /** @brief Sets whether the game ended without a winner. @param value Exit
   * state. */
  void setGameExitedProperty(bool value);

  /** @brief Gets indexes tied for the current high score. @return Index list.
   */
  [[nodiscard]] const std::vector<int> &getHighScorersProperty() const;

  /** @brief Gets the player font. @return Player font. */
  [[nodiscard]] const Microsoft::Xna::Framework::Graphics::SpriteFont &
  getPlayerFontProperty() const;

  /** @brief Generates and broadcasts the authoritative host world. */
  void GenerateWorld();

  /** @brief Initializes the world from the current setup packet. */
  void Initialize();

  /** @brief Updates networking and simulation. @param elapsedTime Elapsed
   * seconds. @param paused Whether local input is paused. */
  void Update(float elapsedTime, bool paused);

  /** @brief Draws the world centered at the supplied position. @param
   * elapsedTime Elapsed seconds. @param center Camera center. */
  void Draw(float elapsedTime, Microsoft::Xna::Framework::Vector2 center);

  /**
   * @brief Draws a gamer's name, ship, voice, ready, and score state.
   * @param totalTime Total game time.
   * @param networkGamer Gamer to draw.
   * @param position Screen position.
   * @param spriteBatch Active sprite batch.
   * @param lobby Whether lobby decorations should be drawn.
   */
  void
  DrawPlayerData(float totalTime,
                 Microsoft::Xna::Framework::Net::NetworkGamer &networkGamer,
                 Microsoft::Xna::Framework::Vector2 position,
                 Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch,
                 bool lobby);

  /** @brief Disposes packet and graphics resources. */
  void Dispose() override;

private:
  static constexpr int winningScore_ = 5;
  static constexpr int numberOfAsteroids_ = 15;
  static constexpr float maximumPowerUpTimer_ = 10.0f;
  static constexpr int barrierSize_ = 48;
  static constexpr int updatesBetweenWorldDataSend_ = 30;
  static constexpr int updatesBetweenStatusPackets_ = MaximumPlayers;
  static const Microsoft::Xna::Framework::Point barrierCounts_;
  void ProcessLocalPlayerInput();
  void SendLocalShipData();
  void SendLocalShipDeath();
  void ProcessPackets();
  void SpawnShip();
  void SpawnPowerup();
  void KillShip(Microsoft::Xna::Framework::Net::NetworkGamer *sender);
  void UpdatePlayerData(Microsoft::Xna::Framework::Net::NetworkGamer *sender);
  void UpdateShipData(Microsoft::Xna::Framework::Net::NetworkGamer *sender);
  void UpdateWorldData();
  void Dispose(bool disposing);

  static const Microsoft::Xna::Framework::Rectangle dimensions_;

  bool initialized_{false};
  bool gameWon_{false};
  int winnerIndex_{-1};
  bool gameExited_{false};
  std::vector<int> highScorers_;
  std::array<std::unique_ptr<Asteroid>, numberOfAsteroids_> asteroids_;
  std::unique_ptr<PowerUp> powerUp_;
  float powerUpTimer_{maximumPowerUpTimer_ / 2.0f};
  std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch>
      spriteBatch_;
  std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>
      cornerBarrierTexture_;
  std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>
      verticalBarrierTexture_;
  std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>
      horizontalBarrierTexture_;
  std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>
      chatAbleTexture_;
  std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>
      chatMuteTexture_;
  std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>
      chatTalkingTexture_;
  std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> readyTexture_;
  std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> playerFont_;
  std::vector<Microsoft::Xna::Framework::Rectangle> cornerBarriers_;
  std::vector<Microsoft::Xna::Framework::Rectangle> verticalBarriers_;
  std::vector<Microsoft::Xna::Framework::Rectangle> horizontalBarriers_;
  std::unique_ptr<ParticleEffectManager> particleEffectManager_;
  Microsoft::Xna::Framework::Net::NetworkSession *networkSession_;
  Microsoft::Xna::Framework::Net::PacketWriter packetWriter_;
  Microsoft::Xna::Framework::Net::PacketReader packetReader_;
  int updatesSinceWorldDataSend_{0};
  int updatesSinceStatusPacket_{0};
  bool disposed_{false};
};
} // namespace NetRumble
