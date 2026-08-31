// SPDX-License-Identifier: MS-PL
#pragma once

#include <map>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/Int32.hpp"
#include "System/TimeSpan.hpp"
#include "System/Xml/Linq/XDocument.hpp"
#include "System/Xml/Linq/XName.hpp"

#include "../Utility/Animation.hpp"

namespace CatapultGame {

enum class CrateState {
    Idle,
    Hit,
};

class SupplyCrate : public DrawableGameComponent {
public:
    explicit SupplyCrate(Game& game) : DrawableGameComponent(game), curGame_(&game) {}

    SupplyCrate(Game& game, SpriteBatch& screenSpriteBatch, std::string idleTextureName,
                Vector2 catapultCenterPosition, bool isAI)
        : SupplyCrate(game) {
        idleTextureName_ = std::move(idleTextureName);
        spriteBatch_ = &screenSpriteBatch;
        catapultCenter_ = catapultCenterPosition;
        isAI_ = isAI;
    }

    bool getAnimationRunningProperty() const { return animationRunning_; }
    void setAnimationRunningProperty(bool value) { animationRunning_ = value; }

    bool getIsDestroyedProperty() const { return isDestroyed_; }
    void setIsDestroyedProperty(bool value) { isDestroyed_ = value; }

    CrateState getCurrentStateProperty() const { return currentState_; }
    void setCurrentStateProperty(CrateState value) { currentState_ = value; }

    Vector2 getPositionProperty() const { return position_; }
    void setPositionProperty(Vector2 value) { position_ = value; }

    int getWidthProperty() const { return idleTexture_->getWidthProperty(); }
    int getHeightProperty() const { return idleTexture_->getHeightProperty(); }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CatapultGame.SupplyCrate";
        return name;
    }

    void Initialize() override {
        animationRunning_ = false;
        currentState_ = CrateState::Idle;

        const auto document = System::Xml::Linq::XDocument::Load(
            "Content/Textures/Crate/AnimationsDef.xml");
        const auto definitions = document->Descendants(System::Xml::Linq::XName::Get("Definition"));
        auto& content = curGame_->getContentProperty();
        for (const auto& definition : definitions) {
            const std::string alias = definition->Attribute("Alias")->getValueProperty();
            textures_.emplace(alias, content.Load<Texture2D>(
                definition->Attribute("SheetName")->getValueProperty()));
            const Point frameSize(
                System::Int32::Parse(definition->Attribute("FrameWidth")->getValueProperty()),
                System::Int32::Parse(definition->Attribute("FrameHeight")->getValueProperty()));
            const Point sheetSize(
                System::Int32::Parse(definition->Attribute("SheetColumns")->getValueProperty()),
                System::Int32::Parse(definition->Attribute("SheetRows")->getValueProperty()));
            const auto frameInterval = System::TimeSpan::FromSeconds(
                1.0 / System::Int32::Parse(
                    definition->Attribute("Speed")->getValueProperty()));
            (void)frameInterval;
            Animation animation(textures_.at(alias), frameSize, sheetSize);
            const auto offsetX = definition->Attribute("OffsetX");
            const auto offsetY = definition->Attribute("OffsetY");
            if (offsetX && offsetY) {
                animation.setOffsetProperty(Vector2(
                    static_cast<float>(System::Int32::Parse(offsetX->getValueProperty())),
                    static_cast<float>(System::Int32::Parse(offsetY->getValueProperty()))));
            }
            animations_.emplace(alias, std::move(animation));
        }

        idleTexture_.emplace(content.Load<Texture2D>(idleTextureName_));
        const int xOffset = isAI_ ? positionXOffset
                                  : -positionXOffset - idleTexture_->getWidthProperty();
        position_ = catapultCenter_ + Vector2(static_cast<float>(xOffset),
                                              static_cast<float>(positionYOffset));
        DrawableGameComponent::Initialize();
    }

    void Update(GameTime& gameTime) override {
        if (isDestroyed_) {
            DrawableGameComponent::Update(gameTime);
            return;
        }
        if (currentState_ == CrateState::Hit) {
            if (!animations_.at("explode").getIsActiveProperty())
                isDestroyed_ = true;
            animations_.at("explode").Update();
        }
        lastUpdateState_ = currentState_;
        DrawableGameComponent::Update(gameTime);
    }

    void Draw(const GameTime& gameTime) override {
        if (isDestroyed_) {
            DrawableGameComponent::Draw(gameTime);
            return;
        }
        if (lastUpdateState_ == CrateState::Idle)
            spriteBatch_->Draw(*idleTexture_, position_, Color::White);
        else if (lastUpdateState_ == CrateState::Hit)
            animations_.at("explode").Draw(*spriteBatch_, position_, SpriteEffects::None);
        DrawableGameComponent::Draw(gameTime);
    }

    void Hit() {
        animationRunning_ = true;
        animations_.at("explode").PlayFromFrameIndex(0);
        currentState_ = CrateState::Hit;
    }

private:
    static constexpr int positionXOffset = -100;
    static constexpr int positionYOffset = 35;

    Game* curGame_ = nullptr;
    SpriteBatch* spriteBatch_ = nullptr;
    Vector2 catapultCenter_ = Vector2::Zero;
    bool isAI_ = false;
    bool animationRunning_ = false;
    bool isDestroyed_ = false;
    std::optional<Texture2D> idleTexture_;
    std::string idleTextureName_;
    std::map<std::string, Texture2D> textures_;
    std::map<std::string, Animation> animations_;
    CrateState lastUpdateState_ = CrateState::Idle;
    CrateState currentState_ = CrateState::Idle;
    Vector2 position_ = Vector2::Zero;
};

} // namespace CatapultGame
