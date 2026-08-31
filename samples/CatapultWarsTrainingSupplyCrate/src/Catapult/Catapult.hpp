// SPDX-License-Identifier: MS-PL
#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

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
#include "SplitProjectile.hpp"
#include "SupplyCrate.hpp"

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
    Idle,
    Aiming,
    Firing,
    Hit,
    Reset,
    Stalling,
    ProjectilesFalling,
};

enum class HitCheckResult {
    Nothing,
    SelfCatapult,
    EnemyCatapult,
    SelfCrate,
    EnemyCrate,
};

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

    std::shared_ptr<SupplyCrate> getCrateProperty() const { return crate_; }

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
        activeProjectiles_.reserve(maxActiveProjectiles);
        activeProjectilesCopy_.reserve(maxActiveProjectiles);
        destroyedProjectiles_.reserve(maxActiveProjectiles);

        normalProjectile_ = std::make_shared<Projectile>(
            *curGame_, *spriteBatch_, activeProjectiles_, "Textures/Ammo/rock_ammo",
            projectileStartPosition,
            static_cast<float>(animations_.at("Fire").getFrameSizeProperty().Y),
            isAI_, gravity);
        normalProjectile_->Initialize();

        splitProjectile_ = std::make_shared<SplitProjectile>(
            *curGame_, *spriteBatch_, activeProjectiles_, "Textures/Ammo/split_ammo",
            projectileStartPosition,
            static_cast<float>(animations_.at("Fire").getFrameSizeProperty().Y),
            isAI_, gravity);
        splitProjectile_->Initialize();

        crate_ = std::make_shared<SupplyCrate>(
            *curGame_, *spriteBatch_, "Textures/Crate/box",
            catapultPosition_ + Vector2(
                animations_.at("Fire").getFrameSizeProperty().X / 2.0f, 0.0f),
            isAI_);
        crate_->Initialize();

        random_ = System::Random();
        DrawableGameComponent::Initialize();
    }

    void Update(GameTime& gameTime) override {
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
            if (animations_.at("Fire").getFrameIndexProperty() == splitFrames_.at("Fire"))
                Fire(shotVelocity_);
            if (!animations_.at("Fire").getIsActiveProperty())
                postUpdateStateChange = CatapultState::ProjectilesFalling;
            break;
        case static_cast<int>(CatapultState::ProjectilesFalling):
            if (activeProjectiles_.empty())
                postUpdateStateChange = CatapultState::Reset;
            break;
        case static_cast<int>(CatapultState::Hit):
            if (!animations_.at("Destroyed").getIsActiveProperty()) {
                if (getEnemyScore() >= winScore) {
                    gameOver_ = true;
                    break;
                }
                postUpdateStateChange = CatapultState::Reset;
            }
            animations_.at("Destroyed").Update();
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

        destroyedProjectiles_.clear();
        activeProjectilesCopy_.clear();
        activeProjectilesCopy_.insert(activeProjectilesCopy_.end(),
                                      activeProjectiles_.begin(), activeProjectiles_.end());
        for (const auto& projectile : activeProjectilesCopy_) {
            projectile->Update(gameTime);
            if (projectile->getStateProperty() == ProjectileState::HitGround &&
                !projectile->getHitHandledProperty())
                HandleProjectileHit(*projectile);
            if (projectile->getStateProperty() == ProjectileState::Destroyed)
                destroyedProjectiles_.push_back(projectile);
        }
        for (const auto& projectile : destroyedProjectiles_) {
            std::erase(activeProjectiles_, projectile);
        }

        crate_->Update(gameTime);

        DrawableGameComponent::Update(gameTime);
    }

    void Draw(const GameTime& gameTime) override {
        switch (static_cast<int>(lastUpdateState_)) {
        case static_cast<int>(CatapultState::ProjectilesFalling):
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
        case static_cast<int>(CatapultState::Hit):
            animations_.at("Destroyed").Draw(*spriteBatch_, catapultPosition_, spriteEffects_);
            break;
        case static_cast<int>(CatapultState::Reset):
            DrawIdleCatapult();
            break;
        default:
            break;
        }

        for (const auto& projectile : activeProjectiles_)
            projectile->Draw(gameTime);
        crate_->Draw(gameTime);
        DrawableGameComponent::Draw(gameTime);
    }

    void Hit();

    void Fire(float velocity);

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

    void HandleProjectileHit(Projectile& projectile);
    void PerformNothingHit(Projectile& projectile);
    HitCheckResult CheckHit(const Projectile& projectile) const;
    int getEnemyScore() const;

    void DrawIdleCatapult() {
        spriteBatch_->Draw(*idleTexture_, catapultPosition_, std::nullopt, Color::White,
                           0.0f, Vector2::Zero, 1.0f, spriteEffects_, 0.0f);
    }

    static constexpr float gravity = 500.0f;
    static constexpr int winScore = 5;
    static constexpr std::size_t maxActiveProjectiles = 3;

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
    std::shared_ptr<Projectile> normalProjectile_;
    std::shared_ptr<Projectile> splitProjectile_;
    std::vector<std::shared_ptr<Projectile>> activeProjectiles_;
    std::vector<std::shared_ptr<Projectile>> activeProjectilesCopy_;
    std::vector<std::shared_ptr<Projectile>> destroyedProjectiles_;
    std::shared_ptr<SupplyCrate> crate_;
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
