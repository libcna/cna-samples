// SPDX-License-Identifier: MS-PL
#pragma once

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"

#include "../Catapult/Catapult.hpp"
#include "../Utility/AudioManager.hpp"

namespace CatapultGame {

using Microsoft::Xna::Framework::DrawableGameComponent;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;

enum class WeaponType {
    Normal,
    Split,
};

class Player : public DrawableGameComponent {
public:
    static constexpr float MinShotVelocity = 200.0f;
    static constexpr float MaxShotVelocity = 665.0f;
    static constexpr float MinShotAngle = 0.0f;
    static constexpr float MaxShotAngle = 1.3962634f;

    explicit Player(Game& game) : DrawableGameComponent(game), curGame_(&game) {}
    Player(Game& game, SpriteBatch& screenSpriteBatch) : Player(game) {
        spriteBatch_ = &screenSpriteBatch;
    }

    std::shared_ptr<Catapult> getCatapultProperty() const { return catapult_; }
    void setCatapultProperty(std::shared_ptr<Catapult> value) { catapult_ = std::move(value); }

    int getScoreProperty() const { return score_; }
    void setScoreProperty(int value) { score_ = value; }

    const std::string& getNameProperty() const { return name_; }
    void setNameProperty(const std::string& value) { name_ = value; }

    int getHealthProperty() const { return health_; }
    void setHealthProperty(int value) { health_ = value; }

    void setEnemyProperty(Player* value) {
        catapult_->setEnemyProperty(value);
        catapult_->setSelfProperty(this);
    }

    bool getIsActiveProperty() const { return isActive_; }
    void setIsActiveProperty(bool value) { isActive_ = value; }

    WeaponType getWeaponProperty() const { return weapon_; }
    void setWeaponProperty(WeaponType value) { weapon_ = value; }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CatapultGame.Player";
        return name;
    }

    void Initialize() override {
        score_ = 0;
        health_ = 100;
        weapon_ = WeaponType::Normal;
        DrawableGameComponent::Initialize();
    }

    void Update(GameTime& gameTime) override {
        catapult_->Update(gameTime);
        DrawableGameComponent::Update(gameTime);
    }

    void Draw(const GameTime& gameTime) override {
        catapult_->Draw(gameTime);
        DrawableGameComponent::Draw(gameTime);
    }

protected:
    Game* curGame_ = nullptr;
    SpriteBatch* spriteBatch_ = nullptr;
    std::shared_ptr<Catapult> catapult_;

private:
    int score_ = 0;
    std::string name_;
    int health_ = 0;
    bool isActive_ = false;
    WeaponType weapon_ = WeaponType::Normal;
};

inline int Catapult::getEnemyScore() const { return enemy_->getScoreProperty(); }

inline void Catapult::setSelfHealth(int value) { self_->setHealthProperty(value); }

inline HitCheckResult Catapult::CheckHit(const Projectile& projectile) {
    const Vector2 projectilePosition = projectile.getProjectilePositionProperty();
    const Vector3 center(projectilePosition, 0.0f);
    const float radius = static_cast<float>(std::max(
        projectile.getProjectileTextureProperty().getWidthProperty() / 2,
        projectile.getProjectileTextureProperty().getHeightProperty() / 2));
    const BoundingSphere sphere(center, radius);
    const float fireWidth = static_cast<float>(animations_.at("Fire").getFrameSizeProperty().X);
    const float fireHeight = static_cast<float>(animations_.at("Fire").getFrameSizeProperty().Y);
    const BoundingBox selfBox(
        Vector3(catapultPosition_, 0.0f),
        Vector3(catapultPosition_ + Vector2(fireWidth, fireHeight), 0.0f));
    const Vector2 enemyPosition = enemy_->getCatapultProperty()->getPositionProperty();
    const BoundingBox enemyBox(
        Vector3(enemyPosition, 0.0f),
        Vector3(enemyPosition + Vector2(fireWidth, fireHeight), 0.0f));
    const Vector2 selfCratePosition = crate_->getPositionProperty();
    const BoundingBox selfCrateBox(
        Vector3(selfCratePosition, 0.0f),
        Vector3(selfCratePosition + Vector2(
            static_cast<float>(crate_->getWidthProperty()),
            static_cast<float>(crate_->getHeightProperty())), 0.0f));
    const auto enemyCrate = enemy_->getCatapultProperty()->getCrateProperty();
    const Vector2 enemyCratePosition = enemyCrate->getPositionProperty();
    const BoundingBox enemyCrateBox(
        Vector3(enemyCratePosition, 0.0f),
        Vector3(enemyCratePosition + Vector2(
            static_cast<float>(enemyCrate->getWidthProperty()),
            static_cast<float>(enemyCrate->getHeightProperty())), 0.0f));

    if (sphere.Intersects(selfBox) && currentState_ != CatapultState::HitKill) {
        AudioManager::PlaySound("catapultExplosion");
        UpdateHealth(*self_, sphere, selfBox);
        if (self_->getHealthProperty() <= 0) {
            Hit(true);
            enemy_->setScoreProperty(enemy_->getScoreProperty() + 1);
        }
        return HitCheckResult::SelfCatapult;
    }
    if (sphere.Intersects(enemyBox) &&
        enemy_->getCatapultProperty()->getCurrentStateProperty() != CatapultState::HitKill &&
        enemy_->getCatapultProperty()->getCurrentStateProperty() != CatapultState::Reset) {
        AudioManager::PlaySound("catapultExplosion");
        UpdateHealth(*enemy_, sphere, enemyBox);
        if (enemy_->getHealthProperty() <= 0) {
            enemy_->getCatapultProperty()->Hit(true);
            self_->setScoreProperty(self_->getScoreProperty() + 1);
        }
        currentState_ = CatapultState::Reset;
        return HitCheckResult::EnemyCatapult;
    }
    if (sphere.Intersects(selfCrateBox)) return HitCheckResult::SelfCrate;
    if (sphere.Intersects(enemyCrateBox)) return HitCheckResult::EnemyCrate;
    return HitCheckResult::Nothing;
}

inline void Catapult::UpdateHealth(Player& player, const BoundingSphere& projectile,
                                  const BoundingBox& catapult) {
    bool isHit = false;
    const float midPoint = (catapult.Max.X - catapult.Min.X) / 2.0f;
    const BoundingBox catapultCenter(
        Vector3(catapult.Min.X + midPoint - projectile.Radius,
                projectile.Center.Y - projectile.Radius, 0.0f),
        Vector3(catapult.Min.X + midPoint + projectile.Radius,
                projectile.Center.Y + projectile.Radius, 0.0f));
    const BoundingBox catapultLeft(
        Vector3(catapult.Min.X, projectile.Center.Y - projectile.Radius, 0.0f),
        Vector3(catapult.Min.X + midPoint - projectile.Radius,
                projectile.Center.Y + projectile.Radius, 0.0f));
    const BoundingBox catapultRight(
        Vector3(catapult.Min.X + midPoint + projectile.Radius,
                projectile.Center.Y - projectile.Radius, 0.0f),
        Vector3(catapult.Max.X, projectile.Center.Y + projectile.Radius, 0.0f));
    if (projectile.Intersects(catapultCenter)) {
        player.setHealthProperty(player.getHealthProperty() - 75);
        isHit = true;
    } else if (projectile.Intersects(catapultLeft)) {
        player.setHealthProperty(player.getHealthProperty() - (isLeftSide_ ? 50 : 25));
        isHit = true;
    } else if (projectile.Intersects(catapultRight)) {
        player.setHealthProperty(player.getHealthProperty() - (isLeftSide_ ? 25 : 50));
        isHit = true;
    }
    if (isHit) {
        player.getCatapultProperty()->Hit(false);
        Microsoft::Devices::VibrateController::getDefaultProperty()->Start(
            System::TimeSpan::FromMilliseconds(250.0));
    }
}

inline void Catapult::PerformNothingHit(Projectile& projectile) {
    Microsoft::Devices::VibrateController::getDefaultProperty()->Start(
        System::TimeSpan::FromMilliseconds(100.0));
    AudioManager::PlaySound("boulderHit");
    projectile.setHitAnimationProperty(animations_.at("fireMiss"));
}

inline void Catapult::HandleProjectileHit(Projectile& projectile) {
    projectile.setHitHandledProperty(true);
    switch (CheckHit(projectile)) {
    case HitCheckResult::SelfCrate:
    case HitCheckResult::Nothing:
        PerformNothingHit(projectile);
        break;
    case HitCheckResult::SelfCatapult:
        if (currentState_ == CatapultState::HitKill ||
            currentState_ == CatapultState::HitDamage)
            projectile.setHitAnimationProperty(animations_.at("hitSmoke"));
        break;
    case HitCheckResult::EnemyCatapult:
        if (enemy_->getCatapultProperty()->getCurrentStateProperty() == CatapultState::HitKill ||
            enemy_->getCatapultProperty()->getCurrentStateProperty() == CatapultState::HitDamage)
            projectile.setHitAnimationProperty(animations_.at("hitSmoke"));
        else
            PerformNothingHit(projectile);
        break;
    case HitCheckResult::EnemyCrate: {
        const auto enemyCrate = enemy_->getCatapultProperty()->getCrateProperty();
        if (enemyCrate->getCurrentStateProperty() == CrateState::Idle) {
            AudioManager::PlaySound("catapultExplosion");
            projectile.setHitAnimationProperty(animations_.at("hitSmoke"));
            enemyCrate->Hit();
            self_->setWeaponProperty(WeaponType::Split);
        } else {
            PerformNothingHit(projectile);
        }
        break;
    }
    default:
        throw std::runtime_error("Hit invalid entity");
    }
    projectile.getHitAnimationProperty()->PlayFromFrameIndex(0);
}

inline void Catapult::Hit(bool isKilled) {
    animationRunning_ = true;
    if (isKilled)
        animations_.at("Destroyed").PlayFromFrameIndex(0);
    animations_.at("hitSmoke").PlayFromFrameIndex(0);
    currentState_ = isKilled ? CatapultState::HitKill : CatapultState::HitDamage;
    self_->setWeaponProperty(WeaponType::Normal);
}

inline void Catapult::Fire(float velocity, float angle) {
    std::shared_ptr<Projectile> firedProjectile;
    switch (self_->getWeaponProperty()) {
    case WeaponType::Normal:
        firedProjectile = normalProjectile_;
        break;
    case WeaponType::Split:
        firedProjectile = splitProjectile_;
        break;
    default:
        throw std::runtime_error("Firing invalid ammunition");
    }

    firedProjectile->setProjectilePositionProperty(
        firedProjectile->getProjectileStartPositionProperty());
    firedProjectile->Fire(velocity * std::cos(angle), velocity * std::sin(angle));
    firedProjectile->setWindProperty(wind_);
    activeProjectiles_.push_back(std::move(firedProjectile));
}

} // namespace CatapultGame
