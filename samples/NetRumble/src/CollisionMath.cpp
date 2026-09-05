// SPDX-License-Identifier: MS-PL
#include "CollisionMath.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
bool CollisionMath::LineLineIntersect(Vector2 a, Vector2 b, Vector2 c,
                                      Vector2 d, Vector2 &point) {
  point = Vector2::Zero;
  const double denominator =
      (b.X - a.X) * (d.Y - c.Y) - (b.Y - a.Y) * (d.X - c.X);
  if (denominator == 0.0)
    return false;
  const double r =
      ((a.Y - c.Y) * (d.X - c.X) - (a.X - c.X) * (d.Y - c.Y)) / denominator;
  const double s =
      ((a.Y - c.Y) * (b.X - a.X) - (a.X - c.X) * (b.Y - a.Y)) / denominator;
  if (r < 0.0 || r > 1.0 || s < 0.0 || s > 1.0)
    return false;
  point.X = static_cast<float>(a.X + r * (b.X - a.X));
  point.Y = static_cast<float>(a.Y + r * (b.Y - a.Y));
  return true;
}
bool CollisionMath::CircleCircleIntersect(Vector2 center1, float radius1,
                                          Vector2 center2, float radius2) {
  const Vector2 line = center2 - center1;
  return line.LengthSquared() <= (radius1 + radius2) * (radius1 + radius2);
}
bool CollisionMath::CircleRectangleCollide(Vector2 center, float radius,
                                           const Rectangle &rectangle,
                                           CircleLineCollisionResult &result) {
  float x = center.X;
  if (x < rectangle.getLeftProperty())
    x = static_cast<float>(rectangle.getLeftProperty());
  if (x > rectangle.getRightProperty())
    x = static_cast<float>(rectangle.getRightProperty());
  float y = center.Y;
  if (y < rectangle.getTopProperty())
    y = static_cast<float>(rectangle.getTopProperty());
  if (y > rectangle.getBottomProperty())
    y = static_cast<float>(rectangle.getBottomProperty());
  const Vector2 direction(center.X - x, center.Y - y);
  const float distance = direction.Length();
  result.Collision = distance > 0.0f && distance < radius;
  if (result.Collision) {
    result.Distance = radius - distance;
    result.Normal = Vector2::Normalize(direction);
    result.Point = Vector2(x, y);
  }
  return result.Collision;
}
bool CollisionMath::CircleLineCollide(Vector2 center, float radius,
                                      Vector2 lineStart, Vector2 lineEnd,
                                      CircleLineCollisionResult &result) {
  const Vector2 ac = center - lineStart;
  const Vector2 ab = lineEnd - lineStart;
  const float ab2 = ab.LengthSquared();
  if (ab2 <= 0.0f)
    return false;
  float t = Vector2::Dot(ac, ab) / ab2;
  if (t < 0.0f)
    t = 0.0f;
  else if (t > 1.0f)
    t = 1.0f;
  result.Point = lineStart + t * ab;
  result.Normal = center - result.Point;
  const float h2 = result.Normal.LengthSquared();
  result.Collision = h2 > 0.0f && h2 <= radius * radius;
  if (result.Collision) {
    result.Normal.Normalize();
    result.Distance = radius - (center - result.Point).Length();
  }
  return result.Collision;
}
} // namespace NetRumble
