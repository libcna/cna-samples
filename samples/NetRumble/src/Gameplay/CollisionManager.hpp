// SPDX-License-Identifier: MS-PL
#pragma once
#include "BatchRemovalCollection.hpp"
#include "CollisionMath.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include <vector>
namespace NetRumble {
class GameplayObject;
/** @brief Resolves world-object and barrier collisions. */
class CollisionManager final : public BatchRemovalCollection<GameplayObject *> {
public:
  CollisionManager(const CollisionManager &) = delete;
  CollisionManager &operator=(const CollisionManager &) = delete;
  /** @brief Gets registered objects. @return Shared collision collection. */
  static BatchRemovalCollection<GameplayObject *> &getCollectionProperty();
  /** @brief Gets world dimensions. @return Bounds. */ static Microsoft::Xna::
      Framework::Rectangle
      getDimensionsProperty();
  /** @brief Sets world dimensions. @param value Bounds. */ static void
  setDimensionsProperty(Microsoft::Xna::Framework::Rectangle value);
  /** @brief Gets solid barrier rectangles. @return Barrier list. */
  static std::vector<Microsoft::Xna::Framework::Rectangle> &
  getBarriersProperty();
  /** @brief Integrates all objects and collisions. @param elapsedTime Elapsed
   * seconds. */
  static void
  Update(float elapsedTime);
  /** @brief Collects possible collisions along a movement. @param object Moving
   * object. @param movement Movement vector. */
  static void
  Collide(GameplayObject *object, Microsoft::Xna::Framework::Vector2 movement);
  /** @brief Chooses a mostly clear random spawn point. @param spawnedObject
   * Object being spawned. @param radius Required radius. @return Chosen point.
   */
  static Microsoft::Xna::Framework::Vector2
  FindSpawnPoint(GameplayObject *spawnedObject, float radius);
  /** @brief Applies radial damage and impulse. @param source Explosion source.
   * @param target Direct collision target. @param damageAmount Base damage.
   * @param position Center. @param damageRadius Radius. @param damageOwner
   * Whether source can be damaged. */
  static void
  Explode(GameplayObject *source, GameplayObject *target, float damageAmount,
          Microsoft::Xna::Framework::Vector2 position, float damageRadius,
          bool damageOwner);

private:
  static constexpr float speedDamageRatio_ = 0.5f;
  static constexpr int findSpawnPointAttempts_ = 25;
  struct CollisionResult {
    float Distance;
    Microsoft::Xna::Framework::Vector2 Normal;
    GameplayObject *Object;
  };
  CollisionManager() = default;
  static Microsoft::Xna::Framework::Vector2
  MoveAndCollide(GameplayObject *, Microsoft::Xna::Framework::Vector2);
  static void AdjustVelocities(GameplayObject *, GameplayObject *);
  static CollisionManager instance_;
  Microsoft::Xna::Framework::Rectangle dimensions_{0, 0, 2048, 2048};
  std::vector<Microsoft::Xna::Framework::Rectangle> barriers_;
  std::vector<CollisionResult> results_;
};
} // namespace NetRumble
