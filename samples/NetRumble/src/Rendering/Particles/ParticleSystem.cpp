// SPDX-License-Identifier: MS-PL
#include "Rendering/Particles/ParticleSystem.hpp"
#include "Microsoft/Xna/Framework/Content/ContentLoadException.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "RandomMath.hpp"
#include "System/ArgumentNullException.hpp"
namespace NetRumble {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Content;
using namespace Microsoft::Xna::Framework::Graphics;
ParticleSystem ParticleSystem::Clone() const {
  ParticleSystem c;
  c.name_ = name_;
  c.particleCount_ = particleCount_;
  c.position_ = position_;
  c.color_ = color_;
  c.textureName_ = textureName_;
  c.blendMode_ = blendMode_;
  c.duration_ = duration_;
  c.initialDelay_ = initialDelay_;
  c.particlesPerSecond_ = particlesPerSecond_;
  c.releaseRate_ = releaseRate_;
  c.durationMinimum_ = durationMinimum_;
  c.durationMaximum_ = durationMaximum_;
  c.velocityMinimum_ = velocityMinimum_;
  c.velocityMaximum_ = velocityMaximum_;
  c.accelerationMinimum_ = accelerationMinimum_;
  c.accelerationMaximum_ = accelerationMaximum_;
  c.scaleMinimum_ = scaleMinimum_;
  c.scaleMaximum_ = scaleMaximum_;
  c.opacityMinimum_ = opacityMinimum_;
  c.opacityMaximum_ = opacityMaximum_;
  c.releaseAngleMinimum_ = releaseAngleMinimum_;
  c.releaseAngleMaximum_ = releaseAngleMaximum_;
  c.releaseDistanceMinimum_ = releaseDistanceMinimum_;
  c.releaseDistanceMaximum_ = releaseDistanceMaximum_;
  c.angularVelocity_ = angularVelocity_;
  c.scaleDeltaPerSecond_ = scaleDeltaPerSecond_;
  c.opacityDeltaPerSecond_ = opacityDeltaPerSecond_;
  return c;
}
void ParticleSystem::Initialize(ContentManager &c) {
  releaseRate_ = 1.f / particlesPerSecond_;
  particles_ = std::make_unique<ParticleCache>(particleCount_);
  try {
    texture_.emplace(c.Load<Texture2D>(textureName_));
  } catch (const ContentLoadException &) {
    texture_.emplace(c.Load<Texture2D>("Textures/Particles/defaultParticle"));
  }
  textureOrigin_ = Vector2(texture_->getWidthProperty() / 2.f,
                           texture_->getHeightProperty() / 2.f);
  active_ = true;
}
void ParticleSystem::Reset() {
  particles_->Reset();
  timeRemaining_ = duration_;
  initialDelayRemaining_ = initialDelay_;
  active_ = true;
}
void ParticleSystem::Update(float e) {
  if (!getActiveProperty())
    return;
  if (initialDelayRemaining_ > 0) {
    initialDelayRemaining_ -= e;
    return;
  }
  GenerateParticles(e);
  UpdateParticles(e);
  active_ = particles_->getUsedCountProperty() > 0;
}
void ParticleSystem::GenerateParticles(float e) {
  if (timeRemaining_ <= 0)
    return;
  timeRemaining_ -= e;
  releaseTimer_ += e;
  while (releaseTimer_ >= releaseRate_) {
    auto *p = particles_->GetNextParticle();
    if (!p)
      break;
    InitializeParticle(p);
    releaseTimer_ -= releaseRate_;
  }
}
void ParticleSystem::UpdateParticles(float e) {
  for (auto &x : particles_->getParticlesProperty())
    if (x->TimeRemaining > 0) {
      x->TimeRemaining -= e;
      if (x->TimeRemaining <= 0) {
        particles_->ReleaseParticle(x.get());
        continue;
      }
      x->Update(e, angularVelocity_, scaleDeltaPerSecond_,
                opacityDeltaPerSecond_);
    }
}
void ParticleSystem::InitializeParticle(Particle *p) {
  if (p == nullptr) {
    throw System::ArgumentNullException("particle");
  }
  p->TimeRemaining =
      RandomMath::RandomBetween(durationMinimum_, durationMaximum_);
  Vector2 d =
      RandomMath::RandomDirection(releaseAngleMinimum_, releaseAngleMaximum_);
  p->Position =
      position_ + d * RandomMath::RandomBetween(releaseDistanceMinimum_,
                                                releaseDistanceMaximum_);
  p->Velocity =
      d * RandomMath::RandomBetween(velocityMinimum_, velocityMaximum_);
  p->Acceleration = p->Velocity.LengthSquared() > 0
                        ? d * RandomMath::RandomBetween(accelerationMinimum_,
                                                        accelerationMaximum_)
                        : Vector2::Zero;
  p->Rotation = RandomMath::RandomBetween(0, MathHelper::TwoPi);
  p->Scale = RandomMath::RandomBetween(scaleMinimum_, scaleMaximum_);
  p->Opacity = RandomMath::RandomBetween(opacityMinimum_, opacityMaximum_);
}
void ParticleSystem::Draw(SpriteBatch &b) {
  if (!getActiveProperty())
    return;
  for (auto &x : particles_->getParticlesProperty())
    if (x->TimeRemaining > 0) {
      color_.W = x->Opacity;
      b.Draw(*texture_, x->Position, std::nullopt, Color(color_), x->Rotation,
             textureOrigin_, x->Scale, SpriteEffects::None, 1);
    }
}
void ParticleSystem::Stop(bool immediately) {
  timeRemaining_ = 0;
  if (immediately)
    active_ = false;
}
} // namespace NetRumble
