// SPDX-License-Identifier: MS-PL
#pragma once
#include "BatchRemovalCollection.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "NetRumbleTypes.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
namespace Microsoft::Xna::Framework::Content {
class ContentManager;
}
namespace Microsoft::Xna::Framework::Graphics {
class SpriteBatch;
}
namespace NetRumble {
class GameplayObject;
class ParticleEffect;
/** @brief Caches, spawns, updates and draws named particle graphs. */
class ParticleEffectManager {
public:
  /** @brief Creates a manager. @param contentManager Content manager. */
  explicit ParticleEffectManager(
      Microsoft::Xna::Framework::Content::ContentManager &contentManager);
  /** @brief Updates active effects. @param elapsedTime Elapsed seconds. */ void
  Update(float elapsedTime);
  /** @brief Draws one blend pass. @param spriteBatch Active batch. @param
   * blendMode Blend pass. */
  virtual void
  Draw(Microsoft::Xna::Framework::Graphics::SpriteBatch &spriteBatch,
       SpriteBlendMode blendMode);
  /** @brief Spawns at a position. @param effectType Effect type. @param
   * position Position. @return Spawned effect or null. */
  std::shared_ptr<ParticleEffect>
  SpawnEffect(ParticleEffectType effectType,
              Microsoft::Xna::Framework::Vector2 position);
  /** @brief Spawns following an object. @param effectType Effect type. @param
   * gameplayObject Object. @return Spawned effect. */
  std::shared_ptr<ParticleEffect>
  SpawnEffect(ParticleEffectType effectType, GameplayObject *gameplayObject);
  /** @brief Spawns with explicit position and target. @param effectType Type.
   * @param position Position. @param gameplayObject Follow object. @return
   * Spawned effect. */
  std::shared_ptr<ParticleEffect>
  SpawnEffect(ParticleEffectType effectType,
              Microsoft::Xna::Framework::Vector2 position,
              GameplayObject *gameplayObject);
  /** @brief Registers an XML template and preallocates instances. @param
   * effectType Type. @param filename Relative XML filename. @param initialCount
   * Instance count. */
  void
  RegisterParticleEffect(ParticleEffectType effectType,
                         const std::string &filename, int initialCount);
  /** @brief Removes a registered effect. @param effectType Type. */ void
  UnregisterParticleEffect(ParticleEffectType effectType);

private:
  std::unordered_map<ParticleEffectType,
                     std::vector<std::shared_ptr<ParticleEffect>>>
      cache_;
  BatchRemovalCollection<std::shared_ptr<ParticleEffect>> active_;
  Microsoft::Xna::Framework::Content::ContentManager *content_;
};
} // namespace NetRumble
