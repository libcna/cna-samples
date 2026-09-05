// SPDX-License-Identifier: MS-PL
#include "Rendering/Particles/ParticleEffect.hpp"
#include "Gameplay/GameplayObject.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "System/IO/FileStream.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"
#include "XmlSerializationAdapters.hpp"
namespace NetRumble {
std::shared_ptr<ParticleEffect> ParticleEffect::Clone() const {
  auto c = std::make_shared<ParticleEffect>();
  c->name_ = name_;
  c->position_ = position_;
  for (const auto &s : particleSystems_)
    c->particleSystems_.push_back(s.Clone());
  return c;
}
void ParticleEffect::Initialize(
    Microsoft::Xna::Framework::Content::ContentManager &c) {
  for (auto &s : particleSystems_)
    s.Initialize(c);
  active_ = true;
}
void ParticleEffect::Reset() {
  for (auto &s : particleSystems_)
    s.Reset();
  active_ = true;
}
void ParticleEffect::Update(float e) {
  if (followObject_) {
    if (followObject_->getActiveProperty())
      setPositionProperty(followObject_->getPositionProperty());
    else {
      followObject_ = nullptr;
      Stop(false);
    }
  }
  active_ = false;
  for (auto &s : particleSystems_)
    if (s.getActiveProperty()) {
      s.Update(e);
      active_ = true;
    }
}
void ParticleEffect::Draw(Microsoft::Xna::Framework::Graphics::SpriteBatch &b,
                          SpriteBlendMode mode) {
  if (!active_)
    return;
  for (auto &s : particleSystems_)
    if (s.getBlendModeProperty() == mode)
      s.Draw(b);
}
void ParticleEffect::Stop(bool now) {
  for (auto &s : particleSystems_)
    s.Stop(now);
  if (now)
    active_ = false;
}
void ParticleEffect::setPositionProperty(Microsoft::Xna::Framework::Vector2 p) {
  position_ = p;
  for (auto &s : particleSystems_)
    s.setPositionProperty(p);
}
std::shared_ptr<ParticleEffect> ParticleEffect::Load(const std::string &path) {
  System::IO::FileStream stream(path);
  auto value =
      System::Xml::Serialization::XmlSerializer<ParticleEffect>{}.Deserialize(
          stream);
  return std::make_shared<ParticleEffect>(std::move(value));
}
} // namespace NetRumble
