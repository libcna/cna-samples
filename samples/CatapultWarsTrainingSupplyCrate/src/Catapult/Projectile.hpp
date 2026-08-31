// SPDX-License-Identifier: MS-PL
#pragma once

#include <cmath>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "../Utility/Animation.hpp"

namespace CatapultGame {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::DrawableGameComponent;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using Microsoft::Xna::Framework::Graphics::Texture2D;

enum class ProjectileState {
    InFlight,
    HitGround,
    Destroyed,
};

class Projectile : public DrawableGameComponent {
public:
    explicit Projectile(Game& game) : DrawableGameComponent(game), curGame_(&game) {}

    Projectile(Game& game, SpriteBatch& screenSpriteBatch,
               std::vector<std::shared_ptr<Projectile>>& activeProjectiles,
               std::string textureName, Vector2 startPosition, float groundHitOffset,
               bool isAI, float gravity)
        : Projectile(game) {
        spriteBatch_ = &screenSpriteBatch;
        activeProjectiles_ = &activeProjectiles;
        projectileStartPosition_ = startPosition;
        textureName_ = std::move(textureName);
        isAI_ = isAI;
        hitOffset_ = groundHitOffset;
        gravity_ = gravity;
    }

    Vector2 getProjectileStartPositionProperty() const { return projectileStartPosition_; }
    void setProjectileStartPositionProperty(Vector2 value) { projectileStartPosition_ = value; }

    Vector2 getCurrentVelocityProperty() const { return currentVelocity_; }

    Vector2 getProjectilePositionProperty() const { return projectilePosition_; }
    void setProjectilePositionProperty(Vector2 value) { projectilePosition_ = value; }

    Vector2 getProjectileHitPositionProperty() const { return projectileHitPosition_; }

    ProjectileState getStateProperty() const { return state_; }

    const Texture2D& getProjectileTextureProperty() const { return *projectileTexture_; }
    void setProjectileTextureProperty(Texture2D value) { projectileTexture_ = std::move(value); }

    Animation* getHitAnimationProperty() const { return hitAnimation_; }
    void setHitAnimationProperty(Animation& value) { hitAnimation_ = &value; }

    bool getHitHandledProperty() const { return hitHandled_; }
    void setHitHandledProperty(bool value) { hitHandled_ = value; }

    virtual float getWindProperty() const { return wind_; }
    virtual void setWindProperty(float value) { wind_ = value; }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CatapultGame.Projectile";
        return name;
    }

    void Initialize() override {
        projectileTexture_.emplace(curGame_->getContentProperty().Load<Texture2D>(textureName_));
    }

    void Update(GameTime& gameTime) override {
        switch (state_) {
        case ProjectileState::InFlight:
            UpdateProjectileFlight(gameTime);
            break;
        case ProjectileState::HitGround:
            UpdateProjectileHit(gameTime);
            break;
        default:
            break;
        }
        DrawableGameComponent::Update(gameTime);
    }

    void Draw(const GameTime& gameTime) override {
        switch (state_) {
        case ProjectileState::InFlight:
            spriteBatch_->Draw(*projectileTexture_, projectilePosition_, std::nullopt,
                Color::White, projectileRotation_,
                Vector2(projectileTexture_->getWidthProperty() / 2.0f,
                        projectileTexture_->getHeightProperty() / 2.0f),
                1.0f, SpriteEffects::None, 0.0f);
            break;
        case ProjectileState::HitGround:
            hitAnimation_->Draw(*spriteBatch_, projectileHitPosition_, SpriteEffects::None);
            break;
        default:
            break;
        }
        DrawableGameComponent::Draw(gameTime);
    }

    void Fire(float velocityX, float velocityY) {
        projectilePosition_ = projectileStartPosition_;
        projectileInitialVelocity_ = Vector2(velocityX, velocityY);
        currentVelocity_ = Vector2(velocityX, velocityY);
        flightTime_ = 0.0f;
        state_ = ProjectileState::InFlight;
        hitHandled_ = false;
    }

protected:
    void UpdateProjectileHit(GameTime&) {
        if (!hitAnimation_->getIsActiveProperty()) {
            state_ = ProjectileState::Destroyed;
            return;
        }
        hitAnimation_->Update();
    }

    virtual void UpdateProjectileFlight(GameTime& gameTime) {
        UpdateProjectileFlightData(gameTime, getWindProperty(), gravity_);
    }

    void UpdateProjectileFlightData(const GameTime& gameTime, float wind, float gravity) {
        flightTime_ += static_cast<float>(
            gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());
        const int direction = isAI_ ? -1 : 1;
        const float previousXPosition = projectilePosition_.X;
        const float previousYPosition = projectilePosition_.Y;

        projectilePosition_.X = projectileStartPosition_.X +
            direction * projectileInitialVelocity_.X * flightTime_ +
            0.5f * (8.0f * wind * static_cast<float>(std::pow(flightTime_, 2)));
        currentVelocity_.X = projectileInitialVelocity_.X + 8.0f * wind * flightTime_;
        projectilePosition_.Y = projectileStartPosition_.Y -
            projectileInitialVelocity_.Y * flightTime_ +
            0.5f * (gravity * static_cast<float>(std::pow(flightTime_, 2)));
        currentVelocity_.Y = projectileInitialVelocity_.Y - gravity * flightTime_;
        projectileRotation_ += MathHelper::ToRadians(projectileInitialVelocity_.X * 0.5f);

        if (projectilePosition_.Y >= 332.0f + hitOffset_) {
            projectilePosition_ = Vector2(previousXPosition, previousYPosition);
            projectileHitPosition_ = Vector2(previousXPosition, 332.0f);
            state_ = ProjectileState::HitGround;
        }
    }

    SpriteBatch* spriteBatch_ = nullptr;
    Game* curGame_ = nullptr;
    std::vector<std::shared_ptr<Projectile>>* activeProjectiles_ = nullptr;
    Vector2 projectileInitialVelocity_ = Vector2::Zero;
    float gravity_ = 0.0f;
    float wind_ = 0.0f;
    float flightTime_ = 0.0f;
    bool isAI_ = false;
    float hitOffset_ = 0.0f;
    Vector2 currentVelocity_ = Vector2::Zero;

private:
    std::string textureName_;
    Vector2 projectileRotationPosition_ = Vector2::Zero;
    float projectileRotation_ = 0.0f;
    Vector2 projectileStartPosition_ = Vector2::Zero;
    Vector2 projectilePosition_ = Vector2::Zero;
    Vector2 projectileHitPosition_ = Vector2::Zero;
    ProjectileState state_ = ProjectileState::InFlight;
    std::optional<Texture2D> projectileTexture_;
    Animation* hitAnimation_ = nullptr;
    bool hitHandled_ = false;
};

} // namespace CatapultGame
