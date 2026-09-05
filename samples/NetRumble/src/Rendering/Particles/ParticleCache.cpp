// SPDX-License-Identifier: MS-PL
#include "Rendering/Particles/ParticleCache.hpp"
#include "System/ArgumentOutOfRangeException.hpp"
namespace NetRumble {
ParticleCache::ParticleCache(int count) {
  if (count <= 0)
    throw System::ArgumentOutOfRangeException("count");
  for (int i = 0; i < count; ++i) {
    particles_.push_back(std::make_unique<Particle>());
    freeParticles_.push(particles_.back().get());
  }
}
int ParticleCache::getTotalCountProperty() const {
  return int(particles_.size());
}
int ParticleCache::getFreeCountProperty() const {
  return int(freeParticles_.size());
}
int ParticleCache::getUsedCountProperty() const {
  return getTotalCountProperty() - getFreeCountProperty();
}
void ParticleCache::Reset() {
  freeParticles_ = {};
  for (auto &p : particles_) {
    p->TimeRemaining = 0;
    freeParticles_.push(p.get());
  }
}
Particle *ParticleCache::GetNextParticle() {
  if (freeParticles_.empty())
    return nullptr;
  auto *p = freeParticles_.front();
  freeParticles_.pop();
  return p;
}
void ParticleCache::ReleaseParticle(Particle *p) {
  if (p)
    freeParticles_.push(p);
}
const std::vector<std::unique_ptr<Particle>> &
ParticleCache::getParticlesProperty() const {
  return particles_;
}
} // namespace NetRumble
