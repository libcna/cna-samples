// SPDX-License-Identifier: MS-PL
#include "Gameplay/PlayerData.hpp"

#include "Gameplay/Ship.hpp"
#include "Microsoft/Xna/Framework/Net/PacketReader.hpp"
#include "Microsoft/Xna/Framework/Net/PacketWriter.hpp"
#include "System/ArgumentOutOfRangeException.hpp"

namespace NetRumble {
PlayerData::PlayerData() { setShipProperty(std::make_unique<Ship>()); }

SharpRuntime::bytecs PlayerData::getShipColorProperty() const {
  return shipColor_;
}

void PlayerData::setShipColorProperty(SharpRuntime::bytecs value) {
  if (value >= Ship::ShipColors.size()) {
    throw System::ArgumentOutOfRangeException("value");
  }

  shipColor_ = value;
  if (ship_ != nullptr) {
    ship_->setColorProperty(Ship::ShipColors[shipColor_]);
  }
}

SharpRuntime::bytecs PlayerData::getShipVariationProperty() const {
  return shipVariation_;
}

void PlayerData::setShipVariationProperty(SharpRuntime::bytecs value) {
  if (value >= Ship::getVariationsProperty()) {
    throw System::ArgumentOutOfRangeException("value");
  }

  shipVariation_ = value;
  if (ship_ != nullptr) {
    ship_->setVariationProperty(shipVariation_);
  }
}

Ship *PlayerData::getShipProperty() const { return ship_.get(); }

void PlayerData::setShipProperty(std::unique_ptr<Ship> value) {
  ship_ = std::move(value);
  if (ship_ != nullptr) {
    ship_->setVariationProperty(shipVariation_);
    ship_->setColorProperty(Ship::ShipColors[shipColor_]);
  }
}

void PlayerData::Deserialize(
    Microsoft::Xna::Framework::Net::PacketReader &packetReader) {
  setShipColorProperty(packetReader.ReadByte());
  setShipVariationProperty(packetReader.ReadByte());
}

void PlayerData::Serialize(
    Microsoft::Xna::Framework::Net::PacketWriter &packetWriter) const {
  packetWriter.Write(shipColor_);
  packetWriter.Write(shipVariation_);
}
} // namespace NetRumble
