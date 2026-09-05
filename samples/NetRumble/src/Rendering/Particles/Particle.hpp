// SPDX-License-Identifier: MS-PL
#pragma once
#include "Microsoft/Xna/Framework/Vector2.hpp"
namespace NetRumble {
/** @brief One reusable particle instance. */ class Particle {
public:
  /** @brief Remaining particle lifetime in seconds. */
  float TimeRemaining = 0;
  /** @brief Current world position. */
  Microsoft::Xna::Framework::Vector2 Position;
  /** @brief Current linear velocity. */
  Microsoft::Xna::Framework::Vector2 Velocity;
  /** @brief Constant linear acceleration. */
  Microsoft::Xna::Framework::Vector2 Acceleration;
  /** @brief Current sprite scale. */
  float Scale = 1;
  /** @brief Current sprite rotation. */
  float Rotation = 0;
  /** @brief Current sprite opacity. */
  float Opacity = 1;
  /** @brief Advances particle kinematics and appearance. @param elapsedTime
   * Elapsed seconds. @param angularVelocity Rotation velocity. @param
   * scaleDeltaPerSecond Scale velocity. @param opacityDeltaPerSecond Opacity
   * velocity. */
  void Update(float elapsedTime, float angularVelocity,
              float scaleDeltaPerSecond, float opacityDeltaPerSecond);
};
} // namespace NetRumble
