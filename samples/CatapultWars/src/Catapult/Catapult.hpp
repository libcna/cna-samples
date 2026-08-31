// SPDX-License-Identifier: MS-PL
#pragma once

#include <algorithm>
#include <map>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Devices/VibrateController.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/Boolean.hpp"
#include "System/Convert.hpp"
#include "System/Int32.hpp"
#include "System/Random.hpp"
#include "System/TimeSpan.hpp"
#include "System/Xml/Linq/XDocument.hpp"
#include "System/Xml/Linq/XName.hpp"

#include "../Utility/Animation.hpp"
#include "../Utility/AudioManager.hpp"
#include "Projectile.hpp"

namespace CatapultGame {

using Microsoft::Xna::Framework::BoundingBox;
using Microsoft::Xna::Framework::BoundingSphere;
using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::DrawableGameComponent;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Point;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using Microsoft::Xna::Framework::Graphics::Texture2D;

class Player;

enum class CatapultState : int {
    Idle = 0x0,
    Aiming = 0x1,
    Firing = 0x2,
    ProjectileFlying = 0x4,
    ProjectileHit = 0x8,
    Hit = 0x10,
    Reset = 0x20,
    Stalling = 0x40,
};

constexpr CatapultState operator|(CatapultState left, CatapultState right) {
    return static_cast<CatapultState>(static_cast<int>(left) | static_cast<int>(right));
}

class Catapult : public DrawableGameComponent {
public:
    explicit Catapult(Game& game) : DrawableGameComponent(game), curGame_(&game) {}

    Catapult(Game& game, SpriteBatch& screenSpriteBatch, std::string idleTexture,
             Vector2 catapultPosition, SpriteEffects spriteEffect, bool isAI)
        : Catapult(game) {
        idleTextureName_ = std::move(idleTexture);
        catapultPosition_ = catapultPosition;
        spriteEffects_ = spriteEffect;
        spriteBatch_ = &screenSpriteBatch;
        isAI_ = isAI;
    }

    bool getAnimationRunningProperty() const { return animationRunning_; }
    void setAnimationRunningProperty(bool value) { animationRunning_ = value; }

    const std::string& getNameProperty() const { return name_; }
    void setNameProperty(const std::string& value) { name_ = value; }

    bool getIsActiveProperty() const { return isActive_; }
    void setIsActiveProperty(bool value) { isActive_ = value; }

    CatapultState getCurrentStateProperty() const { return currentState_; }
    void setCurrentStateProperty(CatapultState value) { currentState_ = value; }

    void setWindProperty(float value) { wind_ = value; }
    void setEnemyProperty(Player* value) { enemy_ = value; }
    void setSelfProperty(Player* value) { self_ = value; }

    Vector2 getPositionProperty() const { return catapultPosition_; }

    float getShotStrengthProperty() const { return shotStrength_; }
    void setShotStrengthProperty(float value) { shotStrength_ = value; }

    float getShotVelocityProperty() const { return shotVelocity_; }
    void setShotVelocityProperty(float value) { shotVelocity_ = value; }

    bool getGameOverProperty() const { return gameOver_; }
    void setGameOverProperty(bool value) { gameOver_ = value; }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CatapultGame.Catapult";
        return name;
    }

    void Initialize() override {
        isActive_ = true;
        animationRunning_ = false;
        currentState_ = CatapultState::Idle;
        stallUpdateCycles_ = 0;

        const auto document = System::Xml::Linq::XDocument::Load(
            "Content/Textures/Catapults/AnimationsDef.xml");
        const auto definitions = document->Descendants(System::Xml::Linq::XName::Get("Definition"));

        auto& content = curGame_->getContentProperty();
        for (const auto& animationDefinition : definitions) {
            std::optional<bool> toLoad;
            bool value = false;
            const auto isAIAttribute = animationDefinition->Attribute("IsAI");
            if (isAIAttribute && System::Boolean::TryParse(isAIAttribute->getValueProperty(), value))
                toLoad = value;

            if (!toLoad.has_value() || toLoad.value() == isAI_) {
                const std::string animationAlias =
                    animationDefinition->Attribute("Alias")->getValueProperty();
                const std::string sheetName =
                    animationDefinition->Attribute("SheetName")->getValueProperty();
                animTextures_.emplace(animationAlias, content.Load<Texture2D>(sheetName));

                const Point frameSize(
                    System::Int32::Parse(animationDefinition->Attribute("FrameWidth")->getValueProperty()),
                    System::Int32::Parse(animationDefinition->Attribute("FrameHeight")->getValueProperty()));
                const Point sheetSize(
                    System::Int32::Parse(animationDefinition->Attribute("SheetColumns")->getValueProperty()),
                    System::Int32::Parse(animationDefinition->Attribute("SheetRows")->getValueProperty()));

                if (const auto splitFrame = animationDefinition->Attribute("SplitFrame")) {
                    splitFrames_.emplace(animationAlias,
                        System::Int32::Parse(splitFrame->getValueProperty()));
                }

                const System::TimeSpan frameInterval = System::TimeSpan::FromSeconds(
                    1.0 / System::Int32::Parse(
                        animationDefinition->Attribute("Speed")->getValueProperty()));
                (void)frameInterval;

                Animation animation(animTextures_.at(animationAlias), frameSize, sheetSize);
                const auto offsetX = animationDefinition->Attribute("OffsetX");
                const auto offsetY = animationDefinition->Attribute("OffsetY");
                if (offsetX && offsetY) {
                    animation.setOffsetProperty(Vector2(
                        static_cast<float>(System::Int32::Parse(offsetX->getValueProperty())),
                        static_cast<float>(System::Int32::Parse(offsetY->getValueProperty()))));
                }
                animations_.emplace(animationAlias, std::move(animation));
            }
        }

        idleTexture_.emplace(content.Load<Texture2D>(idleTextureName_));

        const Vector2 projectileStartPosition = isAI_ ? Vector2(630.0f, 340.0f)
                                                       : Vector2(175.0f, 340.0f);
        projectile_.emplace(*curGame_, *spriteBatch_, "Textures/Ammo/rock_ammo",
                            projectileStartPosition,
                            static_cast<float>(animations_.at("Fire").getFrameSizeProperty().Y),
                            isAI_, gravity);
        projectile_->Initialize();

        random_ = System::Random();
        DrawableGameComponent::Initialize();
    }

    void Update(GameTime& gameTime) override {
        bool isGroundHit = false;
        bool startStall = false;
        CatapultState postUpdateStateChange = CatapultState::Idle;

        if (!isActive_) {
            DrawableGameComponent::Update(gameTime);
            return;
        }

        switch (static_cast<int>(currentState_)) {
        case static_cast<int>(CatapultState::Idle):
            break;
        case static_cast<int>(CatapultState::Aiming):
            if (lastUpdateState_ != CatapultState::Aiming) {
                AudioManager::PlaySound("ropeStretch", true);
                animationRunning_ = true;
                if (isAI_) {
                    animations_.at("Aim").PlayFromFrameIndex(0);
                    stallUpdateCycles_ = 20;
                    startStall = false;
                }
            }
            if (!isAI_) {
                UpdateAimAccordingToShotStrength();
            } else {
                animations_.at("Aim").Update();
                startStall = AimReachedShotStrength();
                currentState_ = startStall ? CatapultState::Stalling : CatapultState::Aiming;
            }
            break;
        case static_cast<int>(CatapultState::Stalling):
            if (stallUpdateCycles_-- <= 0) {
                Fire(shotVelocity_);
                postUpdateStateChange = CatapultState::Firing;
            }
            break;
        case static_cast<int>(CatapultState::Firing):
            if (lastUpdateState_ != CatapultState::Firing) {
                AudioManager::StopSound("ropeStretch");
                AudioManager::PlaySound("catapultFire");
                StartFiringFromLastAimPosition();
            }
            animations_.at("Fire").Update();
            if (animations_.at("Fire").getFrameIndexProperty() == splitFrames_.at("Fire")) {
                postUpdateStateChange = currentState_ | CatapultState::ProjectileFlying;
                projectile_->setProjectilePositionProperty(
                    projectile_->getProjectileStartPositionProperty());
            }
            break;
        case static_cast<int>(CatapultState::Firing) |
             static_cast<int>(CatapultState::ProjectileFlying):
            animations_.at("Fire").Update();
            projectile_->UpdateProjectileFlightData(gameTime, wind_, gravity, isGroundHit);
            if (isGroundHit) {
                postUpdateStateChange = CatapultState::ProjectileHit;
                animations_.at("fireMiss").PlayFromFrameIndex(0);
            }
            break;
        case static_cast<int>(CatapultState::ProjectileFlying):
            projectile_->UpdateProjectileFlightData(gameTime, wind_, gravity, isGroundHit);
            if (isGroundHit) {
                postUpdateStateChange = CatapultState::ProjectileHit;
                animations_.at("fireMiss").PlayFromFrameIndex(0);
            }
            break;
        case static_cast<int>(CatapultState::ProjectileHit):
            if (!CheckHit()) {
                if (lastUpdateState_ != CatapultState::ProjectileHit) {
                    Microsoft::Devices::VibrateController::getDefaultProperty()->Start(
                        System::TimeSpan::FromMilliseconds(100.0));
                    AudioManager::PlaySound("boulderHit");
                }
                if (!animations_.at("fireMiss").getIsActiveProperty())
                    postUpdateStateChange = CatapultState::Reset;
                animations_.at("fireMiss").Update();
            } else {
                Microsoft::Devices::VibrateController::getDefaultProperty()->Start(
                    System::TimeSpan::FromMilliseconds(500.0));
            }
            break;
        case static_cast<int>(CatapultState::Hit):
            if (!animations_.at("Destroyed").getIsActiveProperty() &&
                !animations_.at("hitSmoke").getIsActiveProperty()) {
                if (getEnemyScore() >= winScore) {
                    gameOver_ = true;
                    break;
                }
                postUpdateStateChange = CatapultState::Reset;
            }
            animations_.at("Destroyed").Update();
            animations_.at("hitSmoke").Update();
            break;
        case static_cast<int>(CatapultState::Reset):
            animationRunning_ = false;
            break;
        default:
            break;
        }

        lastUpdateState_ = currentState_;
        if (postUpdateStateChange != CatapultState::Idle)
            currentState_ = postUpdateStateChange;

        DrawableGameComponent::Update(gameTime);
    }

    void Draw(const GameTime& gameTime) override {
        switch (static_cast<int>(lastUpdateState_)) {
        case static_cast<int>(CatapultState::Idle):
            DrawIdleCatapult();
            break;
        case static_cast<int>(CatapultState::Aiming):
        case static_cast<int>(CatapultState::Stalling):
            animations_.at("Aim").Draw(*spriteBatch_, catapultPosition_, spriteEffects_);
            break;
        case static_cast<int>(CatapultState::Firing):
            animations_.at("Fire").Draw(*spriteBatch_, catapultPosition_, spriteEffects_);
            break;
        case static_cast<int>(CatapultState::Firing) |
             static_cast<int>(CatapultState::ProjectileFlying):
        case static_cast<int>(CatapultState::ProjectileFlying):
            animations_.at("Fire").Draw(*spriteBatch_, catapultPosition_, spriteEffects_);
            projectile_->Draw(gameTime);
            break;
        case static_cast<int>(CatapultState::ProjectileHit):
            DrawIdleCatapult();
            animations_.at("fireMiss").Draw(
                *spriteBatch_, projectile_->getProjectileHitPositionProperty(), spriteEffects_);
            break;
        case static_cast<int>(CatapultState::Hit):
            animations_.at("Destroyed").Draw(*spriteBatch_, catapultPosition_, spriteEffects_);
            animations_.at("hitSmoke").Draw(*spriteBatch_, catapultPosition_, spriteEffects_);
            break;
        case static_cast<int>(CatapultState::Reset):
            DrawIdleCatapult();
            break;
        default:
            break;
        }
        DrawableGameComponent::Draw(gameTime);
    }

    void Hit() {
        animationRunning_ = true;
        animations_.at("Destroyed").PlayFromFrameIndex(0);
        animations_.at("hitSmoke").PlayFromFrameIndex(0);
        currentState_ = CatapultState::Hit;
    }

    void Fire(float velocity) { projectile_->Fire(velocity, velocity); }

private:
    bool AimReachedShotStrength() {
        return animations_.at("Aim").getFrameIndexProperty() ==
            System::Convert::ToInt32(
                animations_.at("Aim").getFrameCountProperty() * shotStrength_) - 1;
    }

    void UpdateAimAccordingToShotStrength() {
        Animation& aimAnimation = animations_.at("Aim");
        const int frameToDisplay = System::Convert::ToInt32(
            aimAnimation.getFrameCountProperty() * shotStrength_);
        aimAnimation.setFrameIndexProperty(frameToDisplay);
    }

    void StartFiringFromLastAimPosition() {
        const int startFrame = animations_.at("Aim").getFrameCountProperty() -
                               animations_.at("Aim").getFrameIndexProperty();
        animations_.at("Fire").PlayFromFrameIndex(startFrame);
    }

    bool CheckHit();
    int getEnemyScore() const;

    void DrawIdleCatapult() {
        spriteBatch_->Draw(*idleTexture_, catapultPosition_, std::nullopt, Color::White,
                           0.0f, Vector2::Zero, 1.0f, spriteEffects_, 0.0f);
    }

    static constexpr float gravity = 500.0f;
    static constexpr int winScore = 5;

    Game* curGame_ = nullptr;
    SpriteBatch* spriteBatch_ = nullptr;
    System::Random random_;
    bool animationRunning_ = false;
    std::string name_;
    bool isActive_ = false;
    std::map<std::string, int> splitFrames_;
    std::optional<Texture2D> idleTexture_;
    std::map<std::string, Texture2D> animTextures_;
    std::map<std::string, Animation> animations_;
    SpriteEffects spriteEffects_ = SpriteEffects::None;
    std::optional<Projectile> projectile_;
    std::string idleTextureName_;
    bool isAI_ = false;
    CatapultState lastUpdateState_ = CatapultState::Idle;
    int stallUpdateCycles_ = 0;
    CatapultState currentState_ = CatapultState::Idle;
    float wind_ = 0.0f;
    Player* enemy_ = nullptr;
    Player* self_ = nullptr;
    Vector2 catapultPosition_ = Vector2::Zero;
    float shotStrength_ = 0.0f;
    float shotVelocity_ = 0.0f;
    bool gameOver_ = false;

    friend class Player;
};

} // namespace CatapultGame
