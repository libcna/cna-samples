// SPDX-License-Identifier: MS-PL
#include "Gameplay/ShipInput.hpp"
#include "Gameplay/World.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Net/PacketReader.hpp"
#include "Microsoft/Xna/Framework/Net/PacketWriter.hpp"
namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Input;
using namespace Microsoft::Xna::Framework::Net;
ShipInput::ShipInput(Vector2 l, Vector2 r, bool m)
    : LeftStick(l), RightStick(r), MineFired(m) {}
ShipInput::ShipInput(PacketReader &r)
    : LeftStick(r.ReadVector2()), RightStick(r.ReadVector2()),
      MineFired(r.ReadBoolean()) {}
ShipInput::ShipInput(const GamePadState &g, const KeyboardState &k) {
  bool hit = false;
  if (k.IsKeyDown(Keys::W)) {
    LeftStick += Vector2::UnitY;
    hit = true;
  }
  if (k.IsKeyDown(Keys::A)) {
    LeftStick -= Vector2::UnitX;
    hit = true;
  }
  if (k.IsKeyDown(Keys::S)) {
    LeftStick -= Vector2::UnitY;
    hit = true;
  }
  if (k.IsKeyDown(Keys::D)) {
    LeftStick += Vector2::UnitX;
    hit = true;
  }
  if (hit && LeftStick.LengthSquared() > 0)
    LeftStick.Normalize();
  else if (!hit)
    LeftStick = g.getThumbSticksProperty().getLeftProperty();
  hit = false;
  if (k.IsKeyDown(Keys::Up)) {
    RightStick += Vector2::UnitY;
    hit = true;
  }
  if (k.IsKeyDown(Keys::Left)) {
    RightStick -= Vector2::UnitX;
    hit = true;
  }
  if (k.IsKeyDown(Keys::Down)) {
    RightStick -= Vector2::UnitY;
    hit = true;
  }
  if (k.IsKeyDown(Keys::Right)) {
    RightStick += Vector2::UnitX;
    hit = true;
  }
  if (hit && RightStick.LengthSquared() > 0)
    RightStick.Normalize();
  else if (!hit)
    RightStick = g.getThumbSticksProperty().getRightProperty();
  MineFired = k.IsKeyDown(Keys::Space) ||
              g.getTriggersProperty().getRightProperty() >= .9f;
}
ShipInput ShipInput::getEmptyProperty() { return {}; }
void ShipInput::Serialize(PacketWriter &w) const {
  w.Write(static_cast<int>(World::PacketTypes::ShipInput));
  w.Write(LeftStick);
  w.Write(RightStick);
  w.Write(MineFired);
}
} // namespace NetRumble
