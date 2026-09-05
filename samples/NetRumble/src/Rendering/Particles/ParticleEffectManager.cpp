// SPDX-License-Identifier: MS-PL
#include "Rendering/Particles/ParticleEffectManager.hpp"
#include "Gameplay/GameplayObject.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Rendering/Particles/ParticleEffect.hpp"
#include "System/IO/Path.hpp"
#include "System/ArgumentNullException.hpp"
#include <algorithm>
namespace NetRumble {
ParticleEffectManager::ParticleEffectManager(
    Microsoft::Xna::Framework::Content::ContentManager &c)
    : content_(&c) {}
void ParticleEffectManager::Update(float e) {
  for (auto &x : active_)
    if (x->getActiveProperty()) {
      x->Update(e);
      if (!x->getActiveProperty())
        active_.QueuePendingRemoval(x);
    }
  active_.ApplyPendingRemovals();
}
void ParticleEffectManager::Draw(
    Microsoft::Xna::Framework::Graphics::SpriteBatch &b, SpriteBlendMode m) {
  for (auto &x : active_)
    if (x->getActiveProperty())
      x->Draw(b, m);
}
std::shared_ptr<ParticleEffect>
ParticleEffectManager::SpawnEffect(ParticleEffectType t,
                                   Microsoft::Xna::Framework::Vector2 p) {
  return SpawnEffect(t, p, nullptr);
}
std::shared_ptr<ParticleEffect>
ParticleEffectManager::SpawnEffect(ParticleEffectType t, GameplayObject *o) {
  if (o == nullptr) {
    throw System::ArgumentNullException("gameplayObject");
  }
  return SpawnEffect(t, o->getPositionProperty(), o);
}
std::shared_ptr<ParticleEffect>
ParticleEffectManager::SpawnEffect(ParticleEffectType t,
                                   Microsoft::Xna::Framework::Vector2 p,
                                   GameplayObject *o) {
  std::shared_ptr<ParticleEffect> x;
  auto it = cache_.find(t);
  if (it != cache_.end()) {
    for (auto &a : it->second)
      if (!a->getActiveProperty()) {
        x = a;
        break;
      }
    if (!x) {
      x = it->second.front()->Clone();
      x->Initialize(*content_);
      it->second.push_back(x);
    }
  }
  if (x) {
    x->Reset();
    x->setGameplayObjectProperty(o);
    x->setPositionProperty(p);
    active_.Add(x);
  }
  return x;
}
void ParticleEffectManager::RegisterParticleEffect(ParticleEffectType t,
                                                   const std::string &f,
                                                   int count) {
  if (cache_.contains(t))
    return;
  auto x = ParticleEffect::Load(
      System::IO::Path::Combine(content_->getRootDirectoryProperty(), f));
  x->Initialize(*content_);
  x->Stop(true);
  auto &list = cache_[t];
  list.push_back(x);
  for (int i = 1; i < count; ++i) {
    auto c = x->Clone();
    c->Initialize(*content_);
    c->Stop(true);
    list.push_back(c);
  }
}
void ParticleEffectManager::UnregisterParticleEffect(ParticleEffectType t) {
  auto it = cache_.find(t);
  if (it == cache_.end())
    return;
  for (auto &x : it->second)
    active_.erase(std::remove(active_.begin(), active_.end(), x),
                  active_.end());
  cache_.erase(it);
}
} // namespace NetRumble
