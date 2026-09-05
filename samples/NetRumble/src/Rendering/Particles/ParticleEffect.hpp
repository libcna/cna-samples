// SPDX-License-Identifier: MS-PL
#pragma once
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "NetRumbleTypes.hpp"
#include "Rendering/Particles/ParticleSystem.hpp"
#include "System/Xml/Serialization/detail/XmlMember.hpp"
#include <memory>
#include <string>
#include <tuple>
#include <vector>
namespace Microsoft::Xna::Framework::Content {
class ContentManager;
}
namespace Microsoft::Xna::Framework::Graphics {
class SpriteBatch;
}
namespace NetRumble {
class GameplayObject;
/** @brief Named group of particle systems loaded from XML. */
class ParticleEffect {
public:
  /** @brief Creates an empty effect for XML deserialization. */
  ParticleEffect() = default;
  /** @brief Clones the configured systems. @return Independent effect. */
  [[nodiscard]] std::shared_ptr<ParticleEffect> Clone() const;
  /** @brief Initializes every particle system. @param content Content manager.
   */
  virtual void
  Initialize(Microsoft::Xna::Framework::Content::ContentManager &content);
  /** @brief Restarts every particle system. */ virtual void Reset();
  /** @brief Advances every active system and follow target. @param elapsedTime
   * Elapsed seconds. */
  virtual void
  Update(float elapsedTime);
  /** @brief Draws systems of one blend mode. @param spriteBatch Active batch.
   * @param blendMode Blend pass. */
  virtual void
  Draw(Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch,
       SpriteBlendMode blendMode);
  /** @brief Stops all systems. @param immediately Whether live particles
   * disappear. */
  void
  Stop(bool immediately);
  /** @brief Gets follow object. @return Object or null. */
  [[nodiscard]] GameplayObject *getGameplayObjectProperty() const {
    return followObject_;
  }
  /** @brief Sets follow object. @param value Object or null. */ void
  setGameplayObjectProperty(GameplayObject *value) {
    followObject_ = value;
  }
  /** @brief Gets active state. @return True while any system is active. */
  [[nodiscard]] bool getActiveProperty() const {
    return active_;
  }
  /** @brief Gets name. @return Name. */ [[nodiscard]] const std::string &
  getNameProperty() const {
    return name_;
  }
  /** @brief Sets name. @param value Name. */ void
  setNameProperty(std::string value) {
    name_ = std::move(value);
  }
  /** @brief Gets position. @return Position. */ [[nodiscard]] Microsoft::Xna::
      Framework::Vector2
      getPositionProperty() const {
    return position_;
  }
  /** @brief Sets position on the effect and systems. @param value Position. */
  void setPositionProperty(Microsoft::Xna::Framework::Vector2 value);
  /** @brief Gets particle systems. @return Mutable systems. */
  [[nodiscard]] std::vector<ParticleSystem> &getParticleSystemsProperty() {
    return particleSystems_;
  }
  /** @brief Loads a complete XML graph from a file stream. @param filepath XML
   * path. @return Loaded effect. */
  [[nodiscard]] static std::shared_ptr<ParticleEffect>
  Load(const std::string &filepath);

private:
  std::string name_;
  std::vector<ParticleSystem> particleSystems_;
  Microsoft::Xna::Framework::Vector2 position_;
  GameplayObject *followObject_ = nullptr;
  bool active_ = false;
  friend constexpr const char *SharpXmlRootName(const ParticleEffect *) {
    return "ParticleEffect";
  }
  friend constexpr auto SharpXmlMembers(const ParticleEffect *) {
    using System::Xml::Serialization::detail::MakeMember;
    return std::make_tuple(
        MakeMember("Name", &ParticleEffect::name_),
        MakeMember("Position", &ParticleEffect::position_),
        MakeMember("ParticleSystems", &ParticleEffect::particleSystems_));
  }
};
} // namespace NetRumble
