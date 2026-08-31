// SPDX-License-Identifier: MS-PL
#pragma once

#include <cmath>
#include <optional>
#include <string>

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
#include "System/Random.hpp"

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

class Projectile : public DrawableGameComponent {
public:
    explicit Projectile(Game& game)
        : DrawableGameComponent(game), curGame_(&game), random_() {}

    Projectile(Game& game, SpriteBatch& screenSpriteBatch, std::string textureName,
               Vector2 startPosition, float groundHitOffset, bool isAi, float gravity)
        : Projectile(game) {
        spriteBatch_ = &screenSpriteBatch;
        projectileStartPosition_ = startPosition;
        textureName_ = std::move(textureName);
        isAI_ = isAi;
        hitOffset_ = groundHitOffset;
        gravity_ = gravity;
    }

    Vector2 getProjectileStartPositionProperty() const { return projectileStartPosition_; }
    void setProjectileStartPositionProperty(Vector2 value) { projectileStartPosition_ = value; }

    Vector2 getProjectilePositionProperty() const { return projectilePosition_; }
    void setProjectilePositionProperty(Vector2 value) { projectilePosition_ = value; }

    Vector2 getProjectileHitPositionProperty() const { return projectileHitPosition_; }

    const Texture2D& getProjectileTextureProperty() const { return *projectileTexture_; }
    void setProjectileTextureProperty(Texture2D value) { projectileTexture_ = std::move(value); }

    Vector2 getDrawOffsetProperty() const { return drawOffset_; }
    void setDrawOffsetProperty(Vector2 value) { drawOffset_ = value; }

    float getDrawScaleProperty() const { return drawScale_; }
    void setDrawScaleProperty(float value) { drawScale_ = value; }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CatapultGame.Projectile";
        return name;
    }

    void Initialize() override {
        projectileTexture_.emplace(curGame_->getContentProperty().Load<Texture2D>(textureName_));
    }

    void Draw(const GameTime& gameTime) override {
        spriteBatch_->Draw(*projectileTexture_, projectilePosition_ * drawScale_ + drawOffset_,
                           std::nullopt, Color::White,
                           projectileRotation_,
                           Vector2(static_cast<float>(projectileTexture_->getWidthProperty() / 2),
                                   static_cast<float>(projectileTexture_->getHeightProperty() / 2)),
                           drawScale_, SpriteEffects::None, 0.0f);
        DrawableGameComponent::Draw(gameTime);
    }

    void UpdateProjectileFlightData(const GameTime& gameTime, float wind, float gravity,
                                    bool& groundHit) {
        flightTime_ += static_cast<float>(
            gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());

        const int direction = isAI_ ? -1 : 1;
        const float previousXPosition = projectilePosition_.X;
        const float previousYPosition = projectilePosition_.Y;

        projectilePosition_.X = projectileStartPosition_.X +
            direction * projectileVelocity_.X * flightTime_ +
            0.5f * (8.0f * wind * static_cast<float>(std::pow(flightTime_, 2)));
        projectilePosition_.Y = projectileStartPosition_.Y -
            projectileVelocity_.Y * flightTime_ +
            0.5f * (gravity * static_cast<float>(std::pow(flightTime_, 2)));

        projectileRotation_ += MathHelper::ToRadians(projectileVelocity_.X * 0.05f);

        if (projectilePosition_.Y >= 332.0f + hitOffset_) {
            projectilePosition_.X = previousXPosition;
            projectilePosition_.Y = previousYPosition;
            projectileHitPosition_ = Vector2(previousXPosition, 332.0f);
            groundHit = true;
        } else {
            groundHit = false;
        }
    }

    void Fire(float velocityX, float velocityY) {
        projectileVelocity_.X = velocityX;
        projectileVelocity_.Y = velocityY;
        projectileInitialVelocityY_ = projectileVelocity_.Y;
        flightTime_ = 0.0f;
    }

private:
    SpriteBatch* spriteBatch_ = nullptr;
    Game* curGame_ = nullptr;
    System::Random random_;
    std::string textureName_;
    Vector2 projectileVelocity_ = Vector2::Zero;
    float projectileInitialVelocityY_ = 0.0f;
    Vector2 projectileRotationPosition_ = Vector2::Zero;
    float projectileRotation_ = 0.0f;
    float flightTime_ = 0.0f;
    bool isAI_ = false;
    float hitOffset_ = 0.0f;
    float gravity_ = 0.0f;
    Vector2 projectileStartPosition_ = Vector2::Zero;
    Vector2 projectilePosition_ = Vector2::Zero;
    Vector2 projectileHitPosition_ = Vector2::Zero;
    std::optional<Texture2D> projectileTexture_;
    Vector2 drawOffset_ = Vector2::Zero;
    float drawScale_ = 0.0f;
};

} // namespace CatapultGame
