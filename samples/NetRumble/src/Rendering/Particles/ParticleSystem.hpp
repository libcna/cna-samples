// SPDX-License-Identifier: MS-PL
#pragma once
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "NetRumbleTypes.hpp"
#include "Rendering/Particles/ParticleCache.hpp"
#include "System/Xml/Serialization/detail/XmlMember.hpp"
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
namespace Microsoft::Xna::Framework::Content {
class ContentManager;
}
namespace Microsoft::Xna::Framework::Graphics {
class SpriteBatch;
}
namespace NetRumble {
/** @brief XML-configurable particle emitter. */ class ParticleSystem {
public:
  /** @brief Creates the original default emitter. */ ParticleSystem() =
      default;
  /** @brief Clones serializable configuration without live particles. @return
   * Clone. */
  [[nodiscard]] ParticleSystem
  Clone() const;
  /** @brief Allocates particles and loads the texture. @param content Content
   * manager. */
  virtual void
  Initialize(Microsoft::Xna::Framework::Content::ContentManager &content);
  /** @brief Restarts emission. */ virtual void Reset();
  /** @brief Advances emission and particles. @param elapsedTime Elapsed
   * seconds. */
  virtual void
  Update(float elapsedTime);
  /** @brief Draws all live particles. @param spriteBatch Active batch. */ void
  Draw(Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch);
  /** @brief Stops emission. @param immediately Whether live particles disappear
   * immediately. */
  void
  Stop(bool immediately);
  /** @brief Gets active state. @return True while emitting or particles remain.
   */
  [[nodiscard]] bool
  getActiveProperty() const {
    return active_ || timeRemaining_ > 0;
  }
  /** @brief Gets name. @return Name. */ [[nodiscard]] const std::string &
  getNameProperty() const {
    return name_;
  }
  /** @brief Sets name. @param v Name. */ void setNameProperty(std::string v) {
    name_ = std::move(v);
  }
  /** @brief Gets capacity. @return Count. */ [[nodiscard]] int
  getParticleCountProperty() const {
    return particleCount_;
  }
  /** @brief Sets capacity. @param v Count. */ void
  setParticleCountProperty(int v) {
    particleCount_ = v;
  }
  /** @brief Gets emitter position. @return Position. */
  [[nodiscard]] Microsoft::Xna::Framework::Vector2 getPositionProperty() const {
    return position_;
  }
  /** @brief Sets emitter position. @param v Position. */ void
  setPositionProperty(Microsoft::Xna::Framework::Vector2 v) {
    position_ = v;
  }
  /** @brief Gets tint. @return Tint vector. */ [[nodiscard]] Microsoft::Xna::
      Framework::Vector4
      getColorProperty() const {
    return color_;
  }
  /** @brief Sets tint. @param v Tint. */ void
  setColorProperty(Microsoft::Xna::Framework::Vector4 v) {
    color_ = v;
  }
  /** @brief Gets texture name. @return Content name. */
  [[nodiscard]] const std::string &getTextureNameProperty() const {
    return textureName_;
  }
  /** @brief Sets texture name. @param v Content name. */ void
  setTextureNameProperty(std::string v) {
    textureName_ = std::move(v);
  }
  /** @brief Gets blend mode. @return Blend mode. */
  [[nodiscard]] SpriteBlendMode getBlendModeProperty() const {
    return blendMode_;
  }
  /** @brief Sets blend mode. @param v Blend mode. */ void
  setBlendModeProperty(SpriteBlendMode v) {
    blendMode_ = v;
  }
  /** @brief Gets emitter duration. @return Seconds. */ [[nodiscard]] float
  getDurationProperty() const {
    return duration_;
  }
  /** @brief Sets emitter duration. @param v Seconds. */ void
  setDurationProperty(float v) {
    duration_ = v;
  }
  /** @brief Gets initial delay. @return Seconds. */ [[nodiscard]] float
  getInitialDelayProperty() const {
    return initialDelay_;
  }
  /** @brief Sets initial delay. @param v Seconds. */ void
  setInitialDelayProperty(float v) {
    initialDelay_ = v;
  }
  /** @brief Gets release frequency. @return Particles per second. */
  [[nodiscard]] int getParticlesPerSecondProperty() const {
    return particlesPerSecond_;
  }
  /** @brief Sets release frequency. @param v Particles per second. */ void
  setParticlesPerSecondProperty(int v) {
    particlesPerSecond_ = v;
  }
  /** @brief Gets the minimum particle lifetime. @return Seconds. */
  [[nodiscard]] float getDurationMinimumProperty() const {
    return durationMinimum_;
  }
  /** @brief Sets the minimum particle lifetime. @param v Seconds. */
  void setDurationMinimumProperty(float v) { durationMinimum_ = v; }
  /** @brief Gets the maximum particle lifetime. @return Seconds. */
  [[nodiscard]] float getDurationMaximumProperty() const {
    return durationMaximum_;
  }
  /** @brief Sets the maximum particle lifetime. @param v Seconds. */
  void setDurationMaximumProperty(float v) { durationMaximum_ = v; }
  /** @brief Gets the minimum initial speed. @return Speed. */
  [[nodiscard]] float getVelocityMinimumProperty() const {
    return velocityMinimum_;
  }
  /** @brief Sets the minimum initial speed. @param v Speed. */
  void setVelocityMinimumProperty(float v) { velocityMinimum_ = v; }
  /** @brief Gets the maximum initial speed. @return Speed. */
  [[nodiscard]] float getVelocityMaximumProperty() const {
    return velocityMaximum_;
  }
  /** @brief Sets the maximum initial speed. @param v Speed. */
  void setVelocityMaximumProperty(float v) { velocityMaximum_ = v; }
  /** @brief Gets the minimum acceleration. @return Acceleration. */
  [[nodiscard]] float getAccelerationMinimumProperty() const {
    return accelerationMinimum_;
  }
  /** @brief Sets the minimum acceleration. @param v Acceleration. */
  void setAccelerationMinimumProperty(float v) { accelerationMinimum_ = v; }
  /** @brief Gets the maximum acceleration. @return Acceleration. */
  [[nodiscard]] float getAccelerationMaximumProperty() const {
    return accelerationMaximum_;
  }
  /** @brief Sets the maximum acceleration. @param v Acceleration. */
  void setAccelerationMaximumProperty(float v) { accelerationMaximum_ = v; }
  /** @brief Gets the minimum initial scale. @return Scale. */
  [[nodiscard]] float getScaleMinimumProperty() const { return scaleMinimum_; }
  /** @brief Sets the minimum initial scale. @param v Scale. */
  void setScaleMinimumProperty(float v) { scaleMinimum_ = v; }
  /** @brief Gets the maximum initial scale. @return Scale. */
  [[nodiscard]] float getScaleMaximumProperty() const { return scaleMaximum_; }
  /** @brief Sets the maximum initial scale. @param v Scale. */
  void setScaleMaximumProperty(float v) { scaleMaximum_ = v; }
  /** @brief Gets the minimum initial opacity. @return Opacity. */
  [[nodiscard]] float getOpacityMinimumProperty() const {
    return opacityMinimum_;
  }
  /** @brief Sets the minimum initial opacity. @param v Opacity. */
  void setOpacityMinimumProperty(float v) { opacityMinimum_ = v; }
  /** @brief Gets the maximum initial opacity. @return Opacity. */
  [[nodiscard]] float getOpacityMaximumProperty() const {
    return opacityMaximum_;
  }
  /** @brief Sets the maximum initial opacity. @param v Opacity. */
  void setOpacityMaximumProperty(float v) { opacityMaximum_ = v; }
  /** @brief Gets the minimum release angle. @return Degrees. */
  [[nodiscard]] float getReleaseAngleMinimumProperty() const {
    return releaseAngleMinimum_;
  }
  /** @brief Sets the minimum release angle. @param v Degrees. */
  void setReleaseAngleMinimumProperty(float v) { releaseAngleMinimum_ = v; }
  /** @brief Gets the maximum release angle. @return Degrees. */
  [[nodiscard]] float getReleaseAngleMaximumProperty() const {
    return releaseAngleMaximum_;
  }
  /** @brief Sets the maximum release angle. @param v Degrees. */
  void setReleaseAngleMaximumProperty(float v) { releaseAngleMaximum_ = v; }
  /** @brief Gets the minimum release distance. @return Distance. */
  [[nodiscard]] float getReleaseDistanceMinimumProperty() const {
    return releaseDistanceMinimum_;
  }
  /** @brief Sets the minimum release distance. @param v Distance. */
  void setReleaseDistanceMinimumProperty(float v) {
    releaseDistanceMinimum_ = v;
  }
  /** @brief Gets the maximum release distance. @return Distance. */
  [[nodiscard]] float getReleaseDistanceMaximumProperty() const {
    return releaseDistanceMaximum_;
  }
  /** @brief Sets the maximum release distance. @param v Distance. */
  void setReleaseDistanceMaximumProperty(float v) {
    releaseDistanceMaximum_ = v;
  }
  /** @brief Gets particle angular velocity. @return Radians per second. */
  [[nodiscard]] float getAngularVelocityProperty() const {
    return angularVelocity_;
  }
  /** @brief Sets particle angular velocity. @param v Radians per second. */
  void setAngularVelocityProperty(float v) { angularVelocity_ = v; }
  /** @brief Gets the scale change per second. @return Scale delta. */
  [[nodiscard]] float getScaleDeltaPerSecondProperty() const {
    return scaleDeltaPerSecond_;
  }
  /** @brief Sets the scale change per second. @param v Scale delta. */
  void setScaleDeltaPerSecondProperty(float v) { scaleDeltaPerSecond_ = v; }
  /** @brief Gets the opacity change per second. @return Opacity delta. */
  [[nodiscard]] float getOpacityDeltaPerSecondProperty() const {
    return opacityDeltaPerSecond_;
  }
  /** @brief Sets the opacity change per second. @param v Opacity delta. */
  void setOpacityDeltaPerSecondProperty(float v) {
    opacityDeltaPerSecond_ = v;
  }
private:
  void GenerateParticles(float);
  void UpdateParticles(float);
  void InitializeParticle(Particle *);
  std::string name_ = "DefaultParticleSystem";
  int particleCount_ = 256;
  Microsoft::Xna::Framework::Vector2 position_;
  Microsoft::Xna::Framework::Vector4 color_ =
      Microsoft::Xna::Framework::Color::White.ToVector4();
  std::string textureName_ = "default_particle";
  std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> texture_;
  Microsoft::Xna::Framework::Vector2 textureOrigin_;
  SpriteBlendMode blendMode_ = SpriteBlendMode::AlphaBlend;
  bool active_ = false;
  float duration_ = std::numeric_limits<float>::max(),
        timeRemaining_ = std::numeric_limits<float>::max(), initialDelay_ = 0,
        initialDelayRemaining_ = 0;
  int particlesPerSecond_ = 128;
  float releaseRate_ = .25f, releaseTimer_ = 0, durationMinimum_ = 1,
        durationMaximum_ = 1, velocityMinimum_ = 16, velocityMaximum_ = 32,
        accelerationMinimum_ = 0, accelerationMaximum_ = 0, scaleMinimum_ = 1,
        scaleMaximum_ = 1, opacityMinimum_ = 1, opacityMaximum_ = 1,
        releaseAngleMinimum_ = 0, releaseAngleMaximum_ = 360,
        releaseDistanceMinimum_ = 0, releaseDistanceMaximum_ = 0,
        angularVelocity_ = 0, scaleDeltaPerSecond_ = 0,
        opacityDeltaPerSecond_ = 0;
  std::unique_ptr<ParticleCache> particles_;
  friend constexpr const char *SharpXmlRootName(const ParticleSystem *) {
    return "ParticleSystem";
  }
  friend constexpr auto SharpXmlMembers(const ParticleSystem *) {
    using System::Xml::Serialization::detail::MakeMember;
    return std::make_tuple(
        MakeMember("Name", &ParticleSystem::name_),
        MakeMember("ParticleCount", &ParticleSystem::particleCount_),
        MakeMember("ParticlesPerSecond", &ParticleSystem::particlesPerSecond_),
        MakeMember("Duration", &ParticleSystem::duration_),
        MakeMember("InitialDelay", &ParticleSystem::initialDelay_),
        MakeMember("TextureName", &ParticleSystem::textureName_),
        MakeMember("BlendMode", &ParticleSystem::blendMode_),
        MakeMember("Position", &ParticleSystem::position_),
        MakeMember("Color", &ParticleSystem::color_),
        MakeMember("VelocityMinimum", &ParticleSystem::velocityMinimum_),
        MakeMember("VelocityMaximum", &ParticleSystem::velocityMaximum_),
        MakeMember("AccelerationMinimum",
                   &ParticleSystem::accelerationMinimum_),
        MakeMember("AccelerationMaximum",
                   &ParticleSystem::accelerationMaximum_),
        MakeMember("ScaleMinimum", &ParticleSystem::scaleMinimum_),
        MakeMember("ScaleMaximum", &ParticleSystem::scaleMaximum_),
        MakeMember("OpacityMinimum", &ParticleSystem::opacityMinimum_),
        MakeMember("OpacityMaximum", &ParticleSystem::opacityMaximum_),
        MakeMember("DurationMinimum", &ParticleSystem::durationMinimum_),
        MakeMember("DurationMaximum", &ParticleSystem::durationMaximum_),
        MakeMember("ReleaseAngleMinimum",
                   &ParticleSystem::releaseAngleMinimum_),
        MakeMember("ReleaseAngleMaximum",
                   &ParticleSystem::releaseAngleMaximum_),
        MakeMember("ReleaseDistanceMinimum",
                   &ParticleSystem::releaseDistanceMinimum_),
        MakeMember("ReleaseDistanceMaximum",
                   &ParticleSystem::releaseDistanceMaximum_),
        MakeMember("ScaleDeltaPerSecond",
                   &ParticleSystem::scaleDeltaPerSecond_),
        MakeMember("AngularVelocity", &ParticleSystem::angularVelocity_),
        MakeMember("OpacityDeltaPerSecond",
                   &ParticleSystem::opacityDeltaPerSecond_));
  }
};
} // namespace NetRumble
