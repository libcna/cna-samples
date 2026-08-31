// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <cmath>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "System/Int32.hpp"
#include "System/Random.hpp"

#include "../Players/AI.hpp"
#include "../Players/Human.hpp"
#include "../ScreenManager/ScreenManager.hpp"
#include "../Utility/AudioManager.hpp"

namespace CatapultGame {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::BoundingBox;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Point;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::Viewport;
using Microsoft::Xna::Framework::Input::Touch::GestureSample;
using Microsoft::Xna::Framework::Input::Touch::GestureType;
using GameStateManagement::GameScreen;
using GameStateManagement::InputState;

class GameplayScreen : public GameScreen {
public:
    GameplayScreen() {
        setEnabledGestures(GestureType::FreeDrag | GestureType::DragComplete |
            GestureType::Flick | GestureType::Tap | GestureType::Pinch |
            GestureType::PinchComplete);
    }

    int getCameraMinXOffsetProperty() const {
        return -(ScaleToCamera(foregroundTexture_->getWidthProperty()) -
                 GetViewport().getWidthProperty());
    }

    int getCameraMaxXOffsetProperty() const { return 0; }

    int getCameraMinYOffsetProperty() const {
        return -(ScaleToCamera(foregroundTexture_->getHeightProperty()) -
                 GetViewport().getHeightProperty());
    }

    int getCameraMaxYOffsetProperty() const {
        return ScaleToCamera(skyTexture_->getHeightProperty()) -
               ScaleToCamera(foregroundTexture_->getHeightProperty());
    }

    Vector2 getDrawOffsetProperty() const { return drawOffset_; }
    void setDrawOffsetProperty(Vector2 value) {
        player_->setDrawOffsetProperty(value);
        computer_->setDrawOffsetProperty(value);
        drawOffset_ = value;
    }

    float getDrawScaleProperty() const { return drawScale_; }
    void setDrawScaleProperty(float value) {
        player_->setDrawScaleProperty(value);
        computer_->setDrawScaleProperty(value);
        drawScale_ = value;
    }

    Viewport GetViewport() const {
        return GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
    }

    Vector2 getScreenCenterProperty() const {
        const auto viewport = GetViewport();
        return Vector2(viewport.getWidthProperty() / 2.0f - drawOffset_.X,
                       viewport.getHeightProperty() / 2.0f - drawOffset_.Y);
    }

    Vector2 getMountainPositionProperty() const { return ScaleToCamera(mountainPosition_); }

    Vector2 getSkyPositionProperty() const {
        return Vector2(0.0f, -static_cast<float>(
            ScaleToCamera(skyTexture_->getHeightProperty()) -
            ScaleToCamera(foregroundTexture_->getHeightProperty())));
    }

    Vector2 getCloud1PositionProperty() const { return cloud1Position_ * drawScale_; }
    Vector2 getCloud2PositionProperty() const { return cloud2Position_ * drawScale_; }

    void LoadContent() override {
        GameScreen::LoadContent();
        Start();
    }

    void LoadAssets() {
        foregroundTexture_.emplace(Load<Texture2D>("Textures/Backgrounds/gameplay_screen"));
        cloud1Texture_.emplace(Load<Texture2D>("Textures/Backgrounds/cloud1"));
        cloud2Texture_.emplace(Load<Texture2D>("Textures/Backgrounds/cloud2"));
        mountainTexture_.emplace(Load<Texture2D>("Textures/Backgrounds/mountain"));
        skyTexture_.emplace(Load<Texture2D>("Textures/Backgrounds/sky"));
        defeatTexture_.emplace(Load<Texture2D>("Textures/Backgrounds/defeat"));
        victoryTexture_.emplace(Load<Texture2D>("Textures/Backgrounds/victory"));
        hudBackgroundTexture_.emplace(Load<Texture2D>("Textures/HUD/hudBackground"));
        windArrowTexture_.emplace(Load<Texture2D>("Textures/HUD/windArrow"));
        ammoTypeTexture_.emplace(Load<Texture2D>("Textures/HUD/ammoType"));
        hudFont_.emplace(Load<SpriteFont>("Fonts/HUDFont"));

        cloud1Position_ = Vector2(
            static_cast<float>(224 - cloud1Texture_->getWidthProperty()), 0.0f);
        cloud2Position_ = Vector2(64.0f, 90.0f);
        playerHUDPosition_ = Vector2(7.0f, 7.0f);
        computerHUDPosition_ = Vector2(613.0f, 7.0f);
        windArrowPosition_ = Vector2(345.0f, 46.0f);
        mountainPosition_ = Vector2(400.0f, 0.0f);
        isCameraMoving_ = true;
        catapultCenterOffset_ = Vector2(100.0f, 0.0f);

        Game& game = GetScreenManager()->getGameProperty();
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        player_ = std::make_shared<Human>(game, spriteBatch);
        player_->Initialize();
        player_->setNameProperty("Player");

        computer_ = std::make_shared<AI>(game, spriteBatch);
        computer_->Initialize();
        computer_->setNameProperty("Phone");

        player_->setEnemyProperty(computer_.get());
        computer_->setEnemyProperty(player_.get());

        player_->getCatapultProperty()->setScreenProperty(this);
        computer_->getCatapultProperty()->setScreenProperty(this);
        setDrawOffsetProperty(Vector2(-400.0f, 0.0f));
        setDrawScaleProperty(1.0f);
        CenterOnPosition(player_->getCatapultProperty()->getPositionProperty() +
                         catapultCenterOffset_);
    }

    void Update(GameTime& gameTime, bool otherScreenHasFocus,
                bool coveredByOtherScreen) override {
        const float elapsed = static_cast<float>(
            gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());

        if ((player_->getCatapultProperty()->getGameOverProperty() ||
             computer_->getCatapultProperty()->getGameOverProperty()) && !gameOver_) {
            gameOver_ = true;
            if (player_->getScoreProperty() > computer_->getScoreProperty())
                AudioManager::PlaySound("gameOver_Win");
            else
                AudioManager::PlaySound("gameOver_Lose");
            return;
        }

        if (isFlying_) {
            if (getScreenCenterProperty() == flightDestination_) {
                isFlying_ = false;
            } else {
                const Vector2 flightVector = flightDestination_ - getScreenCenterProperty();
                Vector2 flightMovementVector = flightVector;
                flightMovementVector.Normalize();
                flightMovementVector *= maxCameraSpeed;
                flightMovementVector *= static_cast<float>(
                    0.25 * (2.0 + std::log(flightVector.Length() + 0.2f)));
                if (flightMovementVector.Length() > flightVector.Length())
                    setDrawOffsetProperty(drawOffset_ - flightVector);
                else
                    setDrawOffsetProperty(drawOffset_ - flightMovementVector);
                CorrectScreenPosition(40, 30);
            }
        } else {
            CorrectScreenPosition(0, 0);
        }

        if ((player_->getCatapultProperty()->getCurrentStateProperty() == CatapultState::Reset ||
             computer_->getCatapultProperty()->getCurrentStateProperty() == CatapultState::Reset) &&
            !(player_->getCatapultProperty()->getAnimationRunningProperty() ||
              computer_->getCatapultProperty()->getAnimationRunningProperty())) {
            changeTurn_ = true;

            if (player_->getIsActiveProperty()) {
                CenterOnPosition(computer_->getCatapultProperty()->getPositionProperty() -
                                 catapultCenterOffset_);
                player_->setIsActiveProperty(false);
                computer_->setIsActiveProperty(true);
                isHumanTurn_ = false;
                player_->getCatapultProperty()->setCurrentStateProperty(CatapultState::Idle);
                computer_->getCatapultProperty()->setCurrentStateProperty(CatapultState::Aiming);
            } else {
                isCameraMoving_ = true;
                player_->setIsActiveProperty(true);
                computer_->setIsActiveProperty(false);
                isHumanTurn_ = true;
                computer_->getCatapultProperty()->setCurrentStateProperty(CatapultState::Idle);
                player_->getCatapultProperty()->setCurrentStateProperty(CatapultState::Idle);
            }
        }

        if (changeTurn_) {
            wind_ = Vector2(static_cast<float>(random_.Next(-1, 2)),
                            static_cast<float>(random_.Next(minWind, maxWind + 1)));
            const float value = wind_.X > 0.0f ? wind_.Y : -wind_.Y;
            player_->getCatapultProperty()->setWindProperty(value);
            computer_->getCatapultProperty()->setWindProperty(value);
            changeTurn_ = false;
        }

        player_->Update(gameTime);
        computer_->Update(gameTime);
        UpdateClouds(elapsed);
        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
    }

    void Draw(const GameTime& gameTime) override {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        spriteBatch.Begin();
        DrawBackground();
        DrawComputer(gameTime);
        DrawPlayer(gameTime);
        DrawHud();
        spriteBatch.End();
    }

    void HandleInput(InputState& input) override {
        if (gameOver_) {
            if (input.IsPauseGame(std::nullopt)) FinishCurrentGame();
            for (const auto& gestureSample : input.Gestures) {
                if (gestureSample.getGestureTypeProperty() == GestureType::Tap)
                    FinishCurrentGame();
            }
            return;
        }

        if (input.IsPauseGame(std::nullopt)) {
            PauseCurrentGame();
        } else if (isCameraMoving_) {
            for (const auto& gestureSample : input.Gestures) {
                switch (gestureSample.getGestureTypeProperty()) {
                case GestureType::FreeDrag:
                    if (CatapultTapped(gestureSample.getPositionProperty())) {
                        isCameraMoving_ = false;
                        CenterOnPosition(player_->getCatapultProperty()->getPositionProperty() +
                                         catapultCenterOffset_);
                    } else {
                        isDragging_ = true;
                        setDrawOffsetProperty(ClampDrawOffset(
                            drawOffset_ + gestureSample.getDeltaProperty()));
                    }
                    break;
                case GestureType::DragComplete:
                    ResetDragState();
                    break;
                case GestureType::Tap:
                    if (isCameraMoving_)
                        isFlying_ = false;
                    break;
                case GestureType::Flick:
                    if (!lastGestureType_.has_value() ||
                        lastGestureType_.value() != GestureType::PinchComplete) {
                        FlyToPositionNoScale(
                            getScreenCenterProperty() - gestureSample.getDeltaProperty());
                    }
                    break;
                case GestureType::Pinch:
                    if (!prevSample_.has_value())
                        prevSample_ = gestureSample;
                    else
                        prevSample_ = currentSample_;
                    currentSample_ = gestureSample;
                    if (prevSample_.has_value()) {
                        const float currentLength =
                            (currentSample_->getPositionProperty() -
                             currentSample_->getPosition2Property()).Length();
                        const float previousLength =
                            (prevSample_->getPositionProperty() -
                             prevSample_->getPosition2Property()).Length();
                        const float scaleChange =
                            (currentLength - previousLength) * 0.05f;
                        const Vector2 previousCenter = getScreenCenterProperty();
                        const float previousScale = drawScale_;
                        setDrawScaleProperty(MathHelper::Clamp(
                            drawScale_ + scaleChange, minScale, maxScale));
                        CenterOnPositionNoScale(
                            previousCenter * drawScale_ / previousScale);
                    }
                    break;
                case GestureType::PinchComplete:
                    ResetPinchState();
                    break;
                default:
                    break;
                }
                lastGestureType_ = gestureSample.getGestureTypeProperty();
            }
        } else if (isHumanTurn_ &&
                   (player_->getCatapultProperty()->getCurrentStateProperty() ==
                        CatapultState::Idle ||
                    player_->getCatapultProperty()->getCurrentStateProperty() ==
                        CatapultState::Aiming)) {
            for (const auto& gestureSample : input.Gestures) {
                if (gestureSample.getGestureTypeProperty() == GestureType::FreeDrag)
                    isDragging_ = true;
                else if (gestureSample.getGestureTypeProperty() == GestureType::DragComplete)
                    isDragging_ = false;
                player_->HandleInput(gestureSample);
            }
        }
    }

    int ScaleToCamera(int value) const {
        return static_cast<int>(value * drawScale_);
    }

    Vector2 ScaleToCamera(Vector2 value) const { return value * drawScale_; }

    void CenterOnPosition(Vector2 centerLocation) {
        CenterOnPositionNoScale(centerLocation * drawScale_);
    }

    void CenterOnPositionNoScale(Vector2 centerLocation) {
        const auto viewport = GetViewport();
        const Vector2 newOffset(
            viewport.getWidthProperty() / 2.0f - centerLocation.X,
            viewport.getHeightProperty() / 2.0f - centerLocation.Y);
        setDrawOffsetProperty(ClampDrawOffset(newOffset));
    }

    void FlyToPosition(Vector2 destination) {
        FlyToPositionNoScale(destination * drawScale_);
    }

private:
    void CorrectScreenPosition(int xTolerance, int yTolerance) {
        Vector2 correctionVector = getScreenCenterProperty();
        bool needCorrection = false;
        const auto viewport = GetViewport();
        if (drawOffset_.X > getCameraMaxXOffsetProperty() + xTolerance) {
            correctionVector.X = getCameraMaxXOffsetProperty() +
                                 viewport.getWidthProperty() / 2.0f;
            needCorrection = true;
        } else if (drawOffset_.X < getCameraMinXOffsetProperty() - xTolerance) {
            correctionVector.X = ScaleToCamera(foregroundTexture_->getWidthProperty()) -
                                 viewport.getWidthProperty() / 2.0f;
            needCorrection = true;
        }

        if (drawOffset_.Y > getCameraMaxYOffsetProperty() + yTolerance) {
            correctionVector.Y = getSkyPositionProperty().Y +
                                 viewport.getHeightProperty() / 2.0f;
            needCorrection = true;
        } else if (drawOffset_.Y < getCameraMinYOffsetProperty() - yTolerance) {
            correctionVector.Y = ScaleToCamera(foregroundTexture_->getHeightProperty()) -
                                 viewport.getHeightProperty() / 2.0f;
            needCorrection = true;
        }
        if (needCorrection)
            FlyToPositionNoScale(correctionVector);
    }

    void FlyToPositionNoScale(Vector2 destination) {
        flightDestinationPoint_ = Point(static_cast<int>(destination.X),
                                        static_cast<int>(destination.Y));
        flightDestination_ = destination;
        isFlying_ = true;
    }

    Vector2 ClampDrawOffset(Vector2 offset) const {
        offset.X = MathHelper::Clamp(offset.X,
            static_cast<float>(getCameraMinXOffsetProperty()),
            static_cast<float>(getCameraMaxXOffsetProperty()));
        offset.Y = MathHelper::Clamp(offset.Y,
            static_cast<float>(getCameraMinYOffsetProperty()),
            static_cast<float>(getCameraMaxYOffsetProperty()));
        return offset;
    }

    void ResetDragState() { isDragging_ = false; }

    void ResetPinchState() {
        prevSample_.reset();
        currentSample_.reset();
    }

    bool CatapultTapped(Vector2 tapPoint) const {
        tapPoint -= drawOffset_;
        const Vector2 catapultPosition =
            player_->getCatapultProperty()->getPositionProperty() * drawScale_;
        const BoundingBox catapultBox(
            Vector3(catapultPosition, 0.0f),
            Vector3(catapultPosition + Vector2(
                player_->getCatapultProperty()->getWidthProperty() * drawScale_,
                player_->getCatapultProperty()->getHeightProperty() * drawScale_), 0.0f));
        const BoundingBox tapBox(
            Vector3(tapPoint.X - ScaleToCamera(10),
                    tapPoint.Y + ScaleToCamera(10), 0.0f),
            Vector3(tapPoint.X + ScaleToCamera(10),
                    tapPoint.Y - ScaleToCamera(10), 0.0f));
        return catapultBox.Intersects(tapBox);
    }

    void UpdateClouds(float elapsedTime) {
        const int windDirection = wind_.X > 0.0f ? 1 : -1;
        const int foregroundWidth = foregroundTexture_->getWidthProperty();

        cloud1Position_ += Vector2(24.0f, 0.0f) * elapsedTime *
                           static_cast<float>(windDirection) * wind_.Y;
        if (cloud1Position_.X > foregroundWidth)
            cloud1Position_.X = -cloud1Texture_->getWidthProperty() * 2.0f;
        else if (cloud1Position_.X < -cloud1Texture_->getWidthProperty() * 2.0f)
            cloud1Position_.X = static_cast<float>(foregroundWidth);

        cloud2Position_ += Vector2(16.0f, 0.0f) * elapsedTime *
                           static_cast<float>(windDirection) * wind_.Y;
        if (cloud2Position_.X > foregroundWidth)
            cloud2Position_.X = -cloud2Texture_->getWidthProperty() * 2.0f;
        else if (cloud2Position_.X < -cloud2Texture_->getWidthProperty() * 2.0f)
            cloud2Position_.X = static_cast<float>(foregroundWidth);
    }

    void DrawPlayer(const GameTime& gameTime) {
        if (!gameOver_) player_->Draw(gameTime);
    }

    void DrawComputer(const GameTime& gameTime) {
        if (!gameOver_) computer_->Draw(gameTime);
    }

    void DrawBackground() {
        GetScreenManager()->getGameProperty().getGraphicsDeviceProperty().Clear(Color::Black);
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        spriteBatch.Draw(*skyTexture_, getSkyPositionProperty() + Vector2(0.0f, drawOffset_.Y),
            std::nullopt, Color::White, 0.0f, Vector2::Zero, drawScale_,
            SpriteEffects::None, 0.0f);
        spriteBatch.Draw(*cloud1Texture_, cloud1Position_ * drawScale_ + drawOffset_,
            std::nullopt, Color::White, 0.0f, Vector2::Zero, drawScale_,
            SpriteEffects::None, 0.0f);
        spriteBatch.Draw(*mountainTexture_, getMountainPositionProperty() + drawOffset_,
            std::nullopt, Color::White, 0.0f, Vector2::Zero, drawScale_,
            SpriteEffects::None, 0.0f);
        spriteBatch.Draw(*cloud2Texture_, cloud2Position_ * drawScale_ + drawOffset_,
            std::nullopt, Color::White, 0.0f, Vector2::Zero, drawScale_,
            SpriteEffects::None, 0.0f);
        spriteBatch.Draw(*foregroundTexture_, drawOffset_, std::nullopt, Color::White,
            0.0f, Vector2::Zero, drawScale_, SpriteEffects::None, 0.0f);
    }

    void DrawHud() {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        const auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();

        if (gameOver_) {
            const Texture2D& texture = player_->getScoreProperty() > computer_->getScoreProperty()
                                           ? *victoryTexture_
                                           : *defeatTexture_;
            spriteBatch.Draw(
                texture,
                Vector2(static_cast<float>(viewport.getWidthProperty() / 2 -
                                           texture.getWidthProperty() / 2),
                        static_cast<float>(viewport.getHeightProperty() / 2 -
                                           texture.getHeightProperty() / 2)),
                Color::White);
            return;
        }

        spriteBatch.Draw(*hudBackgroundTexture_, playerHUDPosition_, Color::White);
        spriteBatch.Draw(*ammoTypeTexture_, playerHUDPosition_ + Vector2(33.0f, 35.0f),
                         Color::White);
        DrawString(*hudFont_, System::Int32::ToString(player_->getScoreProperty()),
                   playerHUDPosition_ + Vector2(123.0f, 35.0f), Color::White);
        DrawString(*hudFont_, player_->getNameProperty(),
                   playerHUDPosition_ + Vector2(40.0f, 1.0f), Color::Blue);

        spriteBatch.Draw(*hudBackgroundTexture_, computerHUDPosition_, Color::White);
        spriteBatch.Draw(*ammoTypeTexture_, computerHUDPosition_ + Vector2(33.0f, 35.0f),
                         Color::White);
        DrawString(*hudFont_, System::Int32::ToString(computer_->getScoreProperty()),
                   computerHUDPosition_ + Vector2(123.0f, 35.0f), Color::White);
        DrawString(*hudFont_, computer_->getNameProperty(),
                   computerHUDPosition_ + Vector2(40.0f, 1.0f), Color::Red);

        std::string text = "WIND";
        Vector2 size = hudFont_->MeasureString(text);
        const Vector2 windArrowScale(wind_.Y / 10.0f, 1.0f);
        spriteBatch.Draw(*windArrowTexture_, windArrowPosition_, std::nullopt, Color::White,
                         0.0f, Vector2::Zero, windArrowScale,
                         wind_.X > 0.0f ? SpriteEffects::None
                                        : SpriteEffects::FlipHorizontally,
                         0.0f);
        DrawString(*hudFont_, text, windArrowPosition_ - Vector2(0.0f, size.Y), Color::Black);

        if (wind_.Y == 0.0f) {
            text = "NONE";
            DrawString(*hudFont_, text, windArrowPosition_, Color::Black);
        }

        if (!isCameraMoving_ && isHumanTurn_)
            text = !isDragging_ ? "Drag Anywhere to Fire" : "Release to Fire!";
        else if (!isCameraMoving_)
            text = "I'll get you yet!";
        else
            text = "Drag from your catapult to start shooting";
        size = hudFont_->MeasureString(text);

        DrawString(*hudFont_, text,
                   Vector2(viewport.getWidthProperty() / 2.0f - size.X / 2.0f,
                           viewport.getHeightProperty() - size.Y),
                   Color::Green);
    }

    void DrawString(SpriteFont& font, const std::string& text, Vector2 position,
                    Color color) {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        spriteBatch.DrawString(font, text, Vector2(position.X + 1.0f, position.Y + 1.0f),
                               Color::Black);
        spriteBatch.DrawString(font, text, position, color);
    }

    void DrawString(SpriteFont& font, const std::string& text, Vector2 position,
                    Color color, float fontScale) {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        const Vector2 origin(0.0f, font.getLineSpacingProperty() / 2.0f);
        spriteBatch.DrawString(font, text, Vector2(position.X + 1.0f, position.Y + 1.0f),
                               Color::Black, 0.0f, origin, fontScale, SpriteEffects::None, 0.0f);
        spriteBatch.DrawString(font, text, position, color, 0.0f, origin, fontScale,
                               SpriteEffects::None, 0.0f);
    }

    void FinishCurrentGame() { ExitScreen(); }
    void PauseCurrentGame();

    void Start() {
        wind_ = Vector2::Zero;
        isHumanTurn_ = false;
        changeTurn_ = true;
        computer_->getCatapultProperty()->setCurrentStateProperty(CatapultState::Reset);
    }

    std::optional<Texture2D> foregroundTexture_;
    std::optional<Texture2D> cloud1Texture_;
    std::optional<Texture2D> cloud2Texture_;
    std::optional<Texture2D> mountainTexture_;
    std::optional<Texture2D> skyTexture_;
    std::optional<Texture2D> hudBackgroundTexture_;
    std::optional<Texture2D> ammoTypeTexture_;
    std::optional<Texture2D> windArrowTexture_;
    std::optional<Texture2D> defeatTexture_;
    std::optional<Texture2D> victoryTexture_;
    std::optional<SpriteFont> hudFont_;
    Vector2 cloud1Position_ = Vector2::Zero;
    Vector2 cloud2Position_ = Vector2::Zero;
    Vector2 playerHUDPosition_ = Vector2::Zero;
    Vector2 computerHUDPosition_ = Vector2::Zero;
    Vector2 windArrowPosition_ = Vector2::Zero;
    Vector2 mountainPosition_ = Vector2::Zero;
    Vector2 drawOffset_ = Vector2::Zero;
    float drawScale_ = 0.0f;
    bool isFlying_ = false;
    Vector2 flightDestination_ = Vector2::Zero;
    Point flightDestinationPoint_;
    Vector2 catapultCenterOffset_ = Vector2::Zero;
    std::optional<GestureSample> currentSample_;
    std::optional<GestureSample> prevSample_;
    std::optional<GestureType> lastGestureType_;
    std::shared_ptr<Human> player_;
    std::shared_ptr<AI> computer_;
    Vector2 wind_ = Vector2::Zero;
    bool changeTurn_ = false;
    bool isHumanTurn_ = false;
    bool isCameraMoving_ = false;
    bool gameOver_ = false;
    System::Random random_;
    static constexpr int minWind = 0;
    static constexpr int maxWind = 10;
    static constexpr float maxCameraSpeed = 10.0f;
    static constexpr float minScale = 1.0f;
    static constexpr float maxScale = 2.0f;
    bool isDragging_ = false;
};

inline void Catapult::CenterScreenOnPosition(Vector2 position) {
    screen_->CenterOnPosition(position);
}

} // namespace CatapultGame
