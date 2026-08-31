// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "System/Int32.hpp"
#include "System/Random.hpp"

#include "../Players/Human.hpp"
#include "../ScreenManager/ScreenManager.hpp"
#include "../Utility/AudioManager.hpp"

namespace CatapultGame {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Input::Touch::GestureType;
using GameStateManagement::GameScreen;
using GameStateManagement::InputState;

class GameplayScreen : public GameScreen {
public:
    GameplayScreen() {
        setEnabledGestures(GestureType::FreeDrag | GestureType::DragComplete | GestureType::Tap);
    }

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
            static_cast<float>(224 - cloud1Texture_->getWidthProperty()), 32.0f);
        cloud2Position_ = Vector2(64.0f, 90.0f);
        playerOneHUDPosition_ = Vector2(7.0f, 7.0f);
        playerTwoHUDPosition_ = Vector2(613.0f, 7.0f);
        windArrowPosition_ = Vector2(345.0f, 46.0f);

        Game& game = GetScreenManager()->getGameProperty();
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        playerOne_ = std::make_shared<Human>(game, spriteBatch, PlayerSide::Left);
        playerOne_->Initialize();
        playerOne_->setNameProperty("Player 1");

        playerTwo_ = std::make_shared<Human>(game, spriteBatch, PlayerSide::Right);
        playerTwo_->Initialize();
        playerTwo_->setNameProperty("Player 2");

        playerOne_->setEnemyProperty(playerTwo_.get());
        playerTwo_->setEnemyProperty(playerOne_.get());
    }

    void Update(GameTime& gameTime, bool otherScreenHasFocus,
                bool coveredByOtherScreen) override {
        const float elapsed = static_cast<float>(
            gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());

        if ((playerOne_->getCatapultProperty()->getGameOverProperty() ||
             playerTwo_->getCatapultProperty()->getGameOverProperty()) && !gameOver_) {
            gameOver_ = true;
            if (playerOne_->getScoreProperty() > playerTwo_->getScoreProperty())
                AudioManager::PlaySound("gameOver_Win");
            else
                AudioManager::PlaySound("gameOver_Lose");
            return;
        }

        if ((playerOne_->getCatapultProperty()->getCurrentStateProperty() == CatapultState::Reset ||
             playerTwo_->getCatapultProperty()->getCurrentStateProperty() == CatapultState::Reset) &&
            !(playerOne_->getCatapultProperty()->getAnimationRunningProperty() ||
              playerTwo_->getCatapultProperty()->getAnimationRunningProperty())) {
            changeTurn_ = true;

            if (playerOne_->getIsActiveProperty()) {
                playerOne_->setIsActiveProperty(false);
                playerTwo_->setIsActiveProperty(true);
                isFirstPlayerTurn_ = false;
                playerOne_->getCatapultProperty()->setCurrentStateProperty(CatapultState::Idle);
                playerTwo_->getCatapultProperty()->setCurrentStateProperty(CatapultState::Idle);
            } else {
                playerOne_->setIsActiveProperty(true);
                playerTwo_->setIsActiveProperty(false);
                isFirstPlayerTurn_ = true;
                playerTwo_->getCatapultProperty()->setCurrentStateProperty(CatapultState::Idle);
                playerOne_->getCatapultProperty()->setCurrentStateProperty(CatapultState::Idle);
            }
        }

        if (changeTurn_) {
            wind_ = Vector2(static_cast<float>(random_.Next(-1, 2)),
                            static_cast<float>(random_.Next(minWind, maxWind + 1)));
            const float value = wind_.X > 0.0f ? wind_.Y : -wind_.Y;
            playerOne_->getCatapultProperty()->setWindProperty(value);
            playerTwo_->getCatapultProperty()->setWindProperty(value);
            changeTurn_ = false;
        }

        playerOne_->Update(gameTime);
        playerTwo_->Update(gameTime);
        UpdateClouds(elapsed);
        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
    }

    void Draw(const GameTime& gameTime) override {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        spriteBatch.Begin();
        DrawBackground();
        DrawPlayerTwo(gameTime);
        DrawPlayerOne(gameTime);
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
        } else if (isFirstPlayerTurn_ &&
                   (playerOne_->getCatapultProperty()->getCurrentStateProperty() ==
                        CatapultState::Idle ||
                    playerOne_->getCatapultProperty()->getCurrentStateProperty() ==
                        CatapultState::Aiming)) {
            for (const auto& gestureSample : input.Gestures) {
                if (gestureSample.getGestureTypeProperty() == GestureType::FreeDrag)
                    isDragging_ = true;
                else if (gestureSample.getGestureTypeProperty() == GestureType::DragComplete)
                    isDragging_ = false;
                playerOne_->HandleInput(gestureSample);
            }
        } else if (!isFirstPlayerTurn_ &&
                   (playerTwo_->getCatapultProperty()->getCurrentStateProperty() ==
                        CatapultState::Idle ||
                    playerTwo_->getCatapultProperty()->getCurrentStateProperty() ==
                        CatapultState::Aiming)) {
            for (const auto& gestureSample : input.Gestures) {
                if (gestureSample.getGestureTypeProperty() == GestureType::FreeDrag)
                    isDragging_ = true;
                else if (gestureSample.getGestureTypeProperty() == GestureType::DragComplete)
                    isDragging_ = false;
                playerTwo_->HandleInput(gestureSample);
            }
        }
    }

private:
    void UpdateClouds(float elapsedTime) {
        const int windDirection = wind_.X > 0.0f ? 1 : -1;
        const int viewportWidth = GetScreenManager()->getGraphicsDeviceProperty()
                                      .getViewportProperty().getWidthProperty();

        cloud1Position_ += Vector2(24.0f, 0.0f) * elapsedTime *
                           static_cast<float>(windDirection) * wind_.Y;
        if (cloud1Position_.X > viewportWidth)
            cloud1Position_.X = -cloud1Texture_->getWidthProperty() * 2.0f;
        else if (cloud1Position_.X < -cloud1Texture_->getWidthProperty() * 2.0f)
            cloud1Position_.X = static_cast<float>(viewportWidth);

        cloud2Position_ += Vector2(16.0f, 0.0f) * elapsedTime *
                           static_cast<float>(windDirection) * wind_.Y;
        if (cloud2Position_.X > viewportWidth)
            cloud2Position_.X = -cloud2Texture_->getWidthProperty() * 2.0f;
        else if (cloud2Position_.X < -cloud2Texture_->getWidthProperty() * 2.0f)
            cloud2Position_.X = static_cast<float>(viewportWidth);
    }

    void DrawPlayerOne(const GameTime& gameTime) {
        if (!gameOver_) playerOne_->Draw(gameTime);
    }

    void DrawPlayerTwo(const GameTime& gameTime) {
        if (!gameOver_) playerTwo_->Draw(gameTime);
    }

    void DrawBackground() {
        GetScreenManager()->getGameProperty().getGraphicsDeviceProperty().Clear(Color::White);
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        spriteBatch.Draw(*skyTexture_, Vector2::Zero, Color::White);
        spriteBatch.Draw(*cloud1Texture_, cloud1Position_, Color::White);
        spriteBatch.Draw(*mountainTexture_, Vector2::Zero, Color::White);
        spriteBatch.Draw(*cloud2Texture_, cloud2Position_, Color::White);
        spriteBatch.Draw(*foregroundTexture_, Vector2::Zero, Color::White);
    }

    void DrawHud() {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        const auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();

        if (gameOver_) {
            const Texture2D& texture = *victoryTexture_;
            const std::string winMessage =
                playerOne_->getScoreProperty() > playerTwo_->getScoreProperty()
                    ? "Player 1 Wins!" : "Player 2 Wins!";
            spriteBatch.Draw(
                texture,
                Vector2(static_cast<float>(viewport.getWidthProperty() / 2 -
                                           texture.getWidthProperty() / 2),
                        static_cast<float>(viewport.getHeightProperty() / 2 -
                                           texture.getHeightProperty() / 2)),
                Color::White);
            const Vector2 winSize = hudFont_->MeasureString(winMessage);
            DrawString(*hudFont_, winMessage,
                Vector2(viewport.getWidthProperty() / 2.0f - winSize.X / 2.0f,
                        viewport.getHeightProperty() / 2.0f -
                            texture.getHeightProperty() / 2.0f + 100.0f),
                Color::Red);
            return;
        }

        spriteBatch.Draw(*hudBackgroundTexture_, playerOneHUDPosition_, Color::White);
        spriteBatch.Draw(*ammoTypeTexture_, playerOneHUDPosition_ + Vector2(33.0f, 35.0f),
                         Color::White);
        DrawString(*hudFont_, System::Int32::ToString(playerOne_->getScoreProperty()),
                   playerOneHUDPosition_ + Vector2(123.0f, 35.0f), Color::White);
        DrawString(*hudFont_, playerOne_->getNameProperty(),
                   playerOneHUDPosition_ + Vector2(40.0f, 1.0f), Color::Blue);

        spriteBatch.Draw(*hudBackgroundTexture_, playerTwoHUDPosition_, Color::White);
        spriteBatch.Draw(*ammoTypeTexture_, playerTwoHUDPosition_ + Vector2(33.0f, 35.0f),
                         Color::White);
        DrawString(*hudFont_, System::Int32::ToString(playerTwo_->getScoreProperty()),
                   playerTwoHUDPosition_ + Vector2(123.0f, 35.0f), Color::White);
        DrawString(*hudFont_, playerTwo_->getNameProperty(),
                   playerTwoHUDPosition_ + Vector2(40.0f, 1.0f), Color::Red);

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

        if (isFirstPlayerTurn_)
            text = !isDragging_ ? "Player 1, Drag Anywhere to Fire" : "Release to Fire!";
        else
            text = !isDragging_ ? "Player 2, Drag Anywhere to Fire!" : "Release to Fire!";
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
        isFirstPlayerTurn_ = false;
        changeTurn_ = true;
        playerTwo_->getCatapultProperty()->setCurrentStateProperty(CatapultState::Reset);
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
    Vector2 playerOneHUDPosition_ = Vector2::Zero;
    Vector2 playerTwoHUDPosition_ = Vector2::Zero;
    Vector2 windArrowPosition_ = Vector2::Zero;
    std::shared_ptr<Human> playerOne_;
    std::shared_ptr<Human> playerTwo_;
    Vector2 wind_ = Vector2::Zero;
    bool changeTurn_ = false;
    bool isFirstPlayerTurn_ = false;
    bool gameOver_ = false;
    System::Random random_;
    static constexpr int minWind = 0;
    static constexpr int maxWind = 10;
    bool isDragging_ = false;
};

} // namespace CatapultGame
