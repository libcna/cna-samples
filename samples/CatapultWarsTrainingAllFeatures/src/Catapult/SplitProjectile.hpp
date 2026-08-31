// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Projectile.hpp"

namespace CatapultGame {

class SplitProjectile : public Projectile {
public:
    explicit SplitProjectile(Game& game) : Projectile(game) {}

    SplitProjectile(Game& game, SpriteBatch& screenSpriteBatch,
                    std::vector<std::shared_ptr<Projectile>>& activeProjectiles,
                    std::string textureName, Vector2 startPosition, float groundHitOffset,
                    bool isAI, float gravity)
        : Projectile(game, screenSpriteBatch, activeProjectiles, std::move(textureName),
                     startPosition, groundHitOffset, isAI, gravity) {}

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CatapultGame.SplitProjectile";
        return name;
    }

    float getWindProperty() const override { return Projectile::getWindProperty(); }

    void setWindProperty(float value) override {
        subProjectile1_->setWindProperty(value);
        subProjectile2_->setWindProperty(value);
        subProjectile3_->setWindProperty(value);
        Projectile::setWindProperty(value);
    }

    void Initialize() override {
        subProjectile1_ = CreateSubProjectile();
        subProjectile2_ = CreateSubProjectile();
        subProjectile3_ = CreateSubProjectile();
        Projectile::Initialize();
    }

protected:
    void UpdateProjectileFlight(GameTime& gameTime) override {
        Projectile::UpdateProjectileFlight(gameTime);
        const float projectileYCurrentSpeed =
            projectileInitialVelocity_.Y - gravity_ * flightTime_;
        if (projectileYCurrentSpeed > 0.0f)
            return;

        activeProjectiles_->clear();
        if (isRightPlayer_) {
            FireSubProjectile(*subProjectile1_, Vector2(-30.0f, 8.0f), 40.0f);
            FireSubProjectile(*subProjectile2_, Vector2(-15.0f, 4.0f), 0.0f);
            FireSubProjectile(*subProjectile3_, Vector2(0.0f, 0.0f), -20.0f);
        } else {
            FireSubProjectile(*subProjectile1_, Vector2(30.0f, 8.0f), 40.0f);
            FireSubProjectile(*subProjectile2_, Vector2(15.0f, 4.0f), 0.0f);
            FireSubProjectile(*subProjectile3_, Vector2(0.0f, 0.0f), -20.0f);
        }
        activeProjectiles_->push_back(subProjectile1_);
        activeProjectiles_->push_back(subProjectile2_);
        activeProjectiles_->push_back(subProjectile3_);
    }

private:
    std::shared_ptr<Projectile> CreateSubProjectile() {
        auto result = std::make_shared<Projectile>(
            *curGame_, *spriteBatch_, *activeProjectiles_, "Textures/Ammo/rock_ammo",
            getProjectileStartPositionProperty(), hitOffset_, isRightPlayer_, gravity_);
        result->Initialize();
        return result;
    }

    void FireSubProjectile(Projectile& projectile, Vector2 offset, float velocityXOffset) {
        projectile.setProjectileStartPositionProperty(getProjectilePositionProperty() + offset);
        projectile.Fire(currentVelocity_.X + velocityXOffset, currentVelocity_.Y);
    }

    std::shared_ptr<Projectile> subProjectile1_;
    std::shared_ptr<Projectile> subProjectile2_;
    std::shared_ptr<Projectile> subProjectile3_;
};

} // namespace CatapultGame
