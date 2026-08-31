// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"

#include "Player.hpp"

namespace CatapultGame {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Input::Touch::GestureSample;
using Microsoft::Xna::Framework::Input::Touch::GestureType;

enum class PlayerSide {
    Left,
    Right,
};

class Human : public Player {
public:
    explicit Human(Game& game) : Player(game) {}

    Human(Game& game, SpriteBatch& screenSpriteBatch, PlayerSide playerSide)
        : Player(game, screenSpriteBatch), playerSide_(playerSide) {
        std::string idleTextureName;
        if (playerSide_ == PlayerSide::Left) {
            catapultPosition_ = Vector2(140.0f, 332.0f);
            idleTextureName = "Textures/Catapults/Blue/blueIdle/blueIdle";
        } else {
            catapultPosition_ = Vector2(600.0f, 332.0f);
            spriteEffect_ = SpriteEffects::FlipHorizontally;
            idleTextureName = "Textures/Catapults/Red/redIdle/redIdle";
        }
        setCatapultProperty(std::make_shared<Catapult>(
            game, screenSpriteBatch, idleTextureName, catapultPosition_, spriteEffect_,
            playerSide_ == PlayerSide::Right));
    }

    bool getIsDraggingProperty() const { return isDragging_; }
    void setIsDraggingProperty(bool value) { isDragging_ = value; }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CatapultGame.Human";
        return name;
    }

    void Initialize() override {
        arrow_.emplace(curGame_->getContentProperty().Load<Texture2D>("Textures/HUD/arrow"));
        catapult_->Initialize();
        Player::Initialize();
    }

    void HandleInput(const GestureSample& gestureSample) {
        if (!getIsActiveProperty()) return;

        if (gestureSample.getGestureTypeProperty() == GestureType::FreeDrag) {
            if (!firstSample_.has_value()) {
                firstSample_ = gestureSample;
                catapult_->setCurrentStateProperty(CatapultState::Aiming);
            }

            prevSample_ = gestureSample;
            const Vector2 delta = prevSample_->getPositionProperty() -
                                  firstSample_->getPositionProperty();
            catapult_->setShotStrengthProperty(delta.Length() / maxDragDelta_);
            constexpr float baseScale = 0.001f;
            arrowScale_ = baseScale * delta.Length();
            isDragging_ = true;
        } else if (gestureSample.getGestureTypeProperty() == GestureType::DragComplete) {
            if (firstSample_.has_value()) {
                const Vector2 delta = prevSample_->getPositionProperty() -
                                      firstSample_->getPositionProperty();
                (void)delta;
                catapult_->setShotVelocityProperty(
                    MinShotStrength + catapult_->getShotStrengthProperty() *
                    (MaxShotStrength - MinShotStrength));
                catapult_->Fire(catapult_->getShotVelocityProperty());
                catapult_->setCurrentStateProperty(CatapultState::Firing);
            }
            ResetDragState();
        }
    }

    void Draw(const GameTime& gameTime) override {
        if (isDragging_) DrawDragArrow(arrowScale_);
        Player::Draw(gameTime);
    }

    void DrawDragArrow(float arrowScale) {
        if (playerSide_ == PlayerSide::Left) {
            spriteBatch_->Draw(*arrow_, catapultPosition_ + Vector2(0.0f, -40.0f),
                std::nullopt, Color::Blue, 0.0f, Vector2::Zero,
                Vector2(arrowScale, 0.1f), spriteEffect_, 0.0f);
        } else {
            spriteBatch_->Draw(*arrow_,
                catapultPosition_ + Vector2(-arrow_->getWidthProperty() * arrowScale + 40.0f,
                                            -40.0f),
                std::nullopt, Color::Red, 0.0f, Vector2::Zero,
                Vector2(arrowScale, 0.1f), spriteEffect_, 0.0f);
        }
    }

    void ResetDragState() {
        firstSample_.reset();
        prevSample_.reset();
        isDragging_ = false;
        arrowScale_ = 0.0f;
        catapult_->setShotStrengthProperty(0.0f);
    }

private:
    std::optional<GestureSample> prevSample_;
    std::optional<GestureSample> firstSample_;
    bool isDragging_ = false;
    const float maxDragDelta_ = Vector2(480.0f, 800.0f).Length();
    std::optional<Texture2D> arrow_;
    float arrowScale_ = 0.0f;
    Vector2 catapultPosition_ = Vector2(140.0f, 332.0f);
    PlayerSide playerSide_ = PlayerSide::Left;
    SpriteEffects spriteEffect_ = SpriteEffects::None;
};

} // namespace CatapultGame
