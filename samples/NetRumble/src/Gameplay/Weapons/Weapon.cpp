// SPDX-License-Identifier: MS-PL
#include "Gameplay/Weapons/Weapon.hpp"
#include "AudioManager.hpp"
#include "Gameplay/Ship.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "System/ArgumentNullException.hpp"
namespace NetRumble {
Weapon::Weapon(Ship *o) : owner_(o) {
  if (!o)
    throw System::ArgumentNullException("owner");
}
void Weapon::Update(float e) {
  if (timeToNextFire_ > 0)
    timeToNextFire_ =
        Microsoft::Xna::Framework::MathHelper::Max(timeToNextFire_ - e, 0.f);
}
void Weapon::Fire(Microsoft::Xna::Framework::Vector2 d) {
  if (timeToNextFire_ > 0)
    return;
  owner_->setSafeProperty(false);
  timeToNextFire_ = fireDelay_;
  CreateProjectiles(d);
  if (!fireSoundEffect_.empty())
    AudioManager::PlaySoundEffect(fireSoundEffect_);
}
} // namespace NetRumble
