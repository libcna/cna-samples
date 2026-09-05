// SPDX-License-Identifier: MS-PL
#include "Gameplay/CollisionManager.hpp"
#include "Gameplay/GameplayObject.hpp"
#include "Gameplay/Projectiles/Projectile.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "RandomMath.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/ArgumentOutOfRangeException.hpp"
#include <algorithm>
#include <cmath>
namespace NetRumble {
using namespace Microsoft::Xna::Framework;
CollisionManager CollisionManager::instance_;
BatchRemovalCollection<GameplayObject *> &
CollisionManager::getCollectionProperty() {
  return instance_;
}
Rectangle CollisionManager::getDimensionsProperty() {
  return instance_.dimensions_;
}
void CollisionManager::setDimensionsProperty(Rectangle v) {
  instance_.dimensions_ = v;
}
std::vector<Rectangle> &CollisionManager::getBarriersProperty() {
  return instance_.barriers_;
}
void CollisionManager::Update(float e) {
  for (auto *o : instance_) {
    if (!o->getActiveProperty())
      continue;
    Vector2 m = o->getVelocityProperty() * e;
    if (!o->getCollidedThisFrameProperty())
      m = MoveAndCollide(o, m);
    o->setPositionProperty(o->getPositionProperty() + m);
    for (const auto &r : instance_.barriers_) {
      CollisionMath::CircleLineCollisionResult x;
      if (dynamic_cast<Projectile *>(o)) {
        CollisionMath::CircleRectangleCollide(o->getPositionProperty() - m,
                                              o->getRadiusProperty(), r, x);
        if (x.Collision) {
          o->setPositionProperty(o->getPositionProperty() - m);
          o->Die(nullptr, false);
        }
      } else if (CollisionMath::CircleRectangleCollide(
                     o->getPositionProperty(), o->getRadiusProperty(), r, x)) {
        float vn = Vector2::Dot(o->getVelocityProperty(), x.Normal);
        o->setVelocityProperty(o->getVelocityProperty() - 2.f * vn * x.Normal);
        o->setPositionProperty(o->getPositionProperty() +
                               x.Normal * x.Distance);
      }
    }
  }
  instance_.ApplyPendingRemovals();
}
Vector2 CollisionManager::MoveAndCollide(GameplayObject *o, Vector2 m) {
  if (o == nullptr) {
    throw System::ArgumentNullException("gameplayObject");
  }
  if (!o->getActiveProperty() || m.LengthSquared() <= 0)
    return m;
  Collide(o, m);
  std::sort(instance_.results_.begin(), instance_.results_.end(),
            [](auto &a, auto &b) { return a.Distance < b.Distance; });
  for (auto &c : instance_.results_)
    if (o->Touch(c.Object) && c.Object->Touch(o)) {
      o->setCollidedThisFrameProperty(true);
      c.Object->setCollidedThisFrameProperty(true);
      AdjustVelocities(o, c.Object);
      return Vector2::Zero;
    }
  return m;
}
void CollisionManager::Collide(GameplayObject *o, Vector2 m) {
  instance_.results_.clear();
  if (o == nullptr) {
    throw System::ArgumentNullException("gameplayObject");
  }
  if (!o->getActiveProperty())
    return;
  float ml = m.Length();
  if (ml <= 0)
    return;
  for (auto *x : instance_) {
    if (x == o || !x->getActiveProperty())
      continue;
    Vector2 v = x->getPositionProperty() - o->getPositionProperty();
    float vl = v.Length();
    if (vl <= 0)
      continue;
    float d = MathHelper::Max(
        vl - (x->getRadiusProperty() + o->getRadiusProperty()), 0.f);
    if (ml < d || Vector2::Dot(m, v) < d)
      continue;
    instance_.results_.push_back({d, Vector2::Normalize(v), x});
  }
}
void CollisionManager::AdjustVelocities(GameplayObject *a, GameplayObject *b) {
  if (a->getMassProperty() <= 0 || b->getMassProperty() <= 0)
    return;
  Vector2 n = b->getPositionProperty() - a->getPositionProperty();
  if (n.LengthSquared() <= 0)
    return;
  n.Normalize();
  Vector2 t(-n.Y, n.X);
  float an = Vector2::Dot(a->getVelocityProperty(), n),
        at = Vector2::Dot(a->getVelocityProperty(), t),
        bn = Vector2::Dot(b->getVelocityProperty(), n),
        bt = Vector2::Dot(b->getVelocityProperty(), t);
  float x = (an * (a->getMassProperty() - b->getMassProperty()) +
             2 * b->getMassProperty() * bn) /
            (a->getMassProperty() + b->getMassProperty());
  float y = (bn * (b->getMassProperty() - a->getMassProperty()) +
             2 * a->getMassProperty() * an) /
            (a->getMassProperty() + b->getMassProperty());
  a->setVelocityProperty(x * n + at * t);
  b->setVelocityProperty(y * n + bt * t);
}
Vector2 CollisionManager::FindSpawnPoint(GameplayObject *o, float radius) {
  auto d = instance_.dimensions_;
  if (radius < 0.0f || radius > static_cast<float>(d.Width) / 2.0f) {
    throw System::ArgumentOutOfRangeException("radius");
  }

  const auto randomPoint = [radius, d]() {
    return Vector2(radius + d.X +
                       RandomMath::getRandomProperty().Next(
                           static_cast<int>(std::floor(d.Width - radius))),
                   radius + d.Y +
                       RandomMath::getRandomProperty().Next(
                           static_cast<int>(std::floor(d.Height - radius))));
  };

  Vector2 p = randomPoint();
  for (int attempt = 0; attempt < findSpawnPointAttempts_; ++attempt) {
    bool valid = true;
    for (auto &r : instance_.barriers_) {
      CollisionMath::CircleLineCollisionResult x;
      if (CollisionMath::CircleRectangleCollide(p, radius, r, x)) {
        valid = false;
        break;
      }
    }
    if (valid)
      for (auto *x : instance_)
        if (x->getActiveProperty() && x != o &&
            CollisionMath::CircleCircleIntersect(
                p, radius, x->getPositionProperty(), x->getRadiusProperty())) {
          valid = false;
          break;
        }
    if (valid) {
      break;
    }
    p = randomPoint();
  }
  return p;
}
void CollisionManager::Explode(GameplayObject *s, GameplayObject *t,
                               float damage, Vector2 p, float radius,
                               bool damageOwner) {
  if (radius <= 0)
    return;
  for (auto *o : instance_) {
    if (!o->getActiveProperty() || o == t || (o == s && !damageOwner))
      continue;
    Vector2 d = o->getPositionProperty() - p;
    float ds = d.LengthSquared();
    if (ds > 0 && ds <= radius * radius) {
      float adjusted = damage * (radius - std::sqrt(ds)) / radius;
      if (adjusted > 0)
        o->Damage(s, adjusted);
      if (o != s) {
        d.Normalize();
        o->setVelocityProperty(o->getVelocityProperty() +
                               d * adjusted * speedDamageRatio_);
      }
    }
  }
}
} // namespace NetRumble
