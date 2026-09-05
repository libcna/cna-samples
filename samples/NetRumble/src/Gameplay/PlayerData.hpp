// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>

#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace Microsoft::Xna::Framework::Net {
class PacketReader;
class PacketWriter;
} // namespace Microsoft::Xna::Framework::Net

namespace NetRumble {
class Ship;

/** @brief Stores the gameplay data associated with one network gamer. */
class PlayerData {
public:
  /** @brief Constructs player data with a new ship. */
  PlayerData();

  /** @brief Gets the ship color index. @return Color index. */
  [[nodiscard]] SharpRuntime::bytecs getShipColorProperty() const;

  /** @brief Sets the ship color index. @param value Color index. */
  void setShipColorProperty(SharpRuntime::bytecs value);

  /** @brief Gets the ship variation index. @return Variation index. */
  [[nodiscard]] SharpRuntime::bytecs getShipVariationProperty() const;

  /** @brief Sets the ship variation index. @param value Variation index. */
  void setShipVariationProperty(SharpRuntime::bytecs value);

  /** @brief Gets this player's ship. @return Ship pointer. */
  [[nodiscard]] Ship *getShipProperty() const;

  /** @brief Replaces this player's ship. @param value New ship. */
  void setShipProperty(std::unique_ptr<Ship> value);

  /** @brief Reads player data from a packet. @param packetReader Source packet.
   */
  void Deserialize(Microsoft::Xna::Framework::Net::PacketReader &packetReader);

  /** @brief Writes player data to a packet. @param packetWriter Destination
   * packet. */
  void
  Serialize(Microsoft::Xna::Framework::Net::PacketWriter &packetWriter) const;

private:
  SharpRuntime::bytecs shipColor_{0};
  SharpRuntime::bytecs shipVariation_{0};
  std::unique_ptr<Ship> ship_;
};
} // namespace NetRumble
