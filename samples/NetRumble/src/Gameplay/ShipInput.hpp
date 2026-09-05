// SPDX-License-Identifier: MS-PL
#pragma once
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
namespace Microsoft::Xna::Framework::Net {
class PacketReader;
class PacketWriter;
} // namespace Microsoft::Xna::Framework::Net
namespace NetRumble {
/** @brief Movement, aim and mine input serialized between peers. */
struct ShipInput {
  /** @brief Movement input from the left stick or keyboard. */
  Microsoft::Xna::Framework::Vector2 LeftStick;
  /** @brief Aiming input from the right stick or keyboard. */
  Microsoft::Xna::Framework::Vector2 RightStick;
  /** @brief Whether the mine action was pressed. */
  bool MineFired = false;
  /** @brief Creates empty input. */ ShipInput() = default;
  /** @brief Creates explicit input. @param leftStick Movement. @param
   * rightStick Aim. @param mineFired Mine flag. */
  ShipInput(Microsoft::Xna::Framework::Vector2 leftStick,
            Microsoft::Xna::Framework::Vector2 rightStick, bool mineFired);
  /** @brief Reads input from a packet. @param packetReader Packet reader. */
  explicit ShipInput(
      Microsoft::Xna::Framework::Net::PacketReader &packetReader);
  /** @brief Maps keyboard and gamepad state. @param gamePadState Gamepad state.
   * @param keyboardState Keyboard state. */
  ShipInput(
      const Microsoft::Xna::Framework::Input::GamePadState &gamePadState,
      const Microsoft::Xna::Framework::Input::KeyboardState &keyboardState);
  /** @brief Returns zero input. @return Empty value. */
  [[nodiscard]] static ShipInput getEmptyProperty();
  /** @brief Writes the packet type and fields. @param packetWriter Packet
   * writer. */
  void
  Serialize(Microsoft::Xna::Framework::Net::PacketWriter &packetWriter) const;
};
} // namespace NetRumble
