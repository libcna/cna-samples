// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/Random.hpp"

namespace NetRumble {
/** @brief Supplies the random values used throughout gameplay. */
class RandomMath final {
public:
  RandomMath() = delete;
  /** @brief Gets the shared random generator. @return Shared generator. */
  [[nodiscard]] static System::Random &getRandomProperty();
  /** @brief Selects a value from the requested range. @param minimum Inclusive
   * lower bound. @param maximum Exclusive upper bound. @return Random value. */
  [[nodiscard]] static float RandomBetween(float minimum, float maximum);
  /** @brief Selects a direction over the full circle. @return Unit direction.
   */
  [[nodiscard]] static Microsoft::Xna::Framework::Vector2 RandomDirection();
  /** @brief Selects a direction from an angle range. @param minimumAngle
   * Minimum degrees. @param maximumAngle Maximum degrees. @return Unit
   * direction. */
  [[nodiscard]] static Microsoft::Xna::Framework::Vector2
  RandomDirection(float minimumAngle, float maximumAngle);

private:
  static System::Random random_;
};
} // namespace NetRumble
