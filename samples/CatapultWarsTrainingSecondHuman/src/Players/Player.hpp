// SPDX-License-Identifier: MS-PL
#pragma once

#include <algorithm>
#include <memory>
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

class Player : public DrawableGameComponent {
public:
    static constexpr float MinShotStrength = 150.0f;
    static constexpr float MaxShotStrength = 400.0f;

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

    void setEnemyProperty(Player* value) {
        catapult_->setEnemyProperty(value);
        catapult_->setSelfProperty(this);
    }

    bool getIsActiveProperty() const { return isActive_; }
    void setIsActiveProperty(bool value) { isActive_ = value; }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CatapultGame.Player";
        return name;
    }

    void Initialize() override {
        score_ = 0;
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
    bool isActive_ = false;
};

inline int Catapult::getEnemyScore() const { return enemy_->getScoreProperty(); }

inline bool Catapult::CheckHit() {
    bool result = false;

    const Vector2 projectilePosition = projectile_->getProjectilePositionProperty();
    const Vector3 center(projectilePosition, 0.0f);
    const float radius = static_cast<float>(std::max(
        projectile_->getProjectileTextureProperty().getWidthProperty() / 2,
        projectile_->getProjectileTextureProperty().getHeightProperty() / 2));
    const BoundingSphere sphere(center, radius);

    const float fireWidth = static_cast<float>(
        animations_.at("Fire").getFrameSizeProperty().X);
    const float fireHeight = static_cast<float>(
        animations_.at("Fire").getFrameSizeProperty().Y);

    const BoundingBox selfBox(
        Vector3(catapultPosition_, 0.0f),
        Vector3(catapultPosition_ + Vector2(fireWidth, fireHeight), 0.0f));

    const Vector2 enemyPosition = enemy_->getCatapultProperty()->getPositionProperty();
    const BoundingBox enemyBox(
        Vector3(enemyPosition, 0.0f),
        Vector3(enemyPosition + Vector2(fireWidth, fireHeight), 0.0f));

    if (sphere.Intersects(selfBox) && currentState_ != CatapultState::Hit) {
        AudioManager::PlaySound("catapultExplosion");
        Hit();
        enemy_->setScoreProperty(enemy_->getScoreProperty() + 1);
        result = true;
    } else if (sphere.Intersects(enemyBox) &&
               enemy_->getCatapultProperty()->getCurrentStateProperty() != CatapultState::Hit &&
               enemy_->getCatapultProperty()->getCurrentStateProperty() != CatapultState::Reset) {
        AudioManager::PlaySound("catapultExplosion");
        enemy_->getCatapultProperty()->Hit();
        self_->setScoreProperty(self_->getScoreProperty() + 1);
        result = true;
        currentState_ = CatapultState::Reset;
    }

    return result;
}

} // namespace CatapultGame
