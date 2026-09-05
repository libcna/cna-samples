// SPDX-License-Identifier: MS-PL
#pragma once
#include "Rendering/Particles/Particle.hpp"
#include <memory>
#include <queue>
#include <vector>
namespace NetRumble {
/** @brief Fixed reusable particle pool. */ class ParticleCache {
public:
  /** @brief Allocates the pool. @param count Particle count. */
  explicit ParticleCache(int count);
  /** @brief Gets total capacity. @return Capacity. */ [[nodiscard]] int
  getTotalCountProperty() const;
  /** @brief Gets free count. @return Free count. */ [[nodiscard]] int
  getFreeCountProperty() const;
  /** @brief Gets used count. @return Used count. */ [[nodiscard]] int
  getUsedCountProperty() const;
  /** @brief Resets all particles. */ void Reset();
  /** @brief Obtains a free particle. @return Particle or null when exhausted.
   */
  Particle *
  GetNextParticle();
  /** @brief Returns a particle to the pool. @param particle Particle. */ void
  ReleaseParticle(Particle *particle);
  /** @brief Gets all particles. @return Pool storage. */
  [[nodiscard]] const std::vector<std::unique_ptr<Particle>> &
  getParticlesProperty() const;

private:
  std::vector<std::unique_ptr<Particle>> particles_;
  std::queue<Particle *> freeParticles_;
};
} // namespace NetRumble
