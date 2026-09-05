// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace NetRumble {
/** @brief Collision-related geometric helpers. */
class CollisionMath final {
public:
  /** @brief Static helper type; construction is not supported. */
  CollisionMath() = delete;
  /** @brief Result of a circle/line or circle/rectangle test. */
  struct CircleLineCollisionResult {
    /** @brief Whether a collision was detected. */
    bool Collision = false;
    /** @brief Closest collision point. */
    Microsoft::Xna::Framework::Vector2 Point;
    /** @brief Collision surface normal. */
    Microsoft::Xna::Framework::Vector2 Normal;
    /** @brief Penetration or separation distance reported by the query. */
    float Distance = 0.0f;
  };
  /** @brief Tests two finite line segments. @param a First start. @param b
   * First end. @param c Second start. @param d Second end. @param point
   * Receives intersection. @return True on intersection. */
  static bool LineLineIntersect(Microsoft::Xna::Framework::Vector2 a,
                                Microsoft::Xna::Framework::Vector2 b,
                                Microsoft::Xna::Framework::Vector2 c,
                                Microsoft::Xna::Framework::Vector2 d,
                                Microsoft::Xna::Framework::Vector2 &point);
  /** @brief Tests two circles. @param center1 First center. @param radius1
   * First radius. @param center2 Second center. @param radius2 Second radius.
   * @return True when touching or overlapping. */
  static bool CircleCircleIntersect(Microsoft::Xna::Framework::Vector2 center1,
                                    float radius1,
                                    Microsoft::Xna::Framework::Vector2 center2,
                                    float radius2);
  /** @brief Tests a circle and rectangle. @param center Circle center. @param
   * radius Radius. @param rectangle Rectangle. @param result Receives collision
   * details. @return True on collision. */
  static bool
  CircleRectangleCollide(Microsoft::Xna::Framework::Vector2 center,
                         float radius,
                         const Microsoft::Xna::Framework::Rectangle &rectangle,
                         CircleLineCollisionResult &result);
  /** @brief Tests a circle and line. @param center Circle center. @param radius
   * Radius. @param lineStart Segment start. @param lineEnd Segment end. @param
   * result Receives details. @return True on collision. */
  static bool CircleLineCollide(Microsoft::Xna::Framework::Vector2 center,
                                float radius,
                                Microsoft::Xna::Framework::Vector2 lineStart,
                                Microsoft::Xna::Framework::Vector2 lineEnd,
                                CircleLineCollisionResult &result);
};
} // namespace NetRumble
