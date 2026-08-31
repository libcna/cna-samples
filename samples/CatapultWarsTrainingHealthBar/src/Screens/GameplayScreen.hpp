// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "System/Int32.hpp"
#include "System/Random.hpp"

#include "../Players/AI.hpp"
#include "../Players/Human.hpp"
#include "../ScreenManager/ScreenManager.hpp"
#include "../Utility/AudioManager.hpp"

namespace CatapultGame {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Rectangle;
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
        blankTexture_.emplace(Load<Texture2D>("Textures/Backgrounds/blank"));
        hudFont_.emplace(Load<SpriteFont>("Fonts/HUDFont"));

        cloud1Position_ = Vector2(
            static_cast<float>(224 - cloud1Texture_->getWidthProperty()), 32.0f);
        cloud2Position_ = Vector2(64.0f, 90.0f);
        playerHUDPosition_ = Vector2(7.0f, 7.0f);
        computerHUDPosition_ = Vector2(613.0f, 7.0f);
        windArrowPosition_ = Vector2(345.0f, 46.0f);
        const Vector2 healthBarOffset(25.0f, 82.0f);
        playerHealthBarPosition_ = playerHUDPosition_ + healthBarOffset;
        computerHealthBarPosition_ = computerHUDPosition_ + healthBarOffset;
        healthBarFullSize_ = Vector2(130.0f, 20.0f);

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

        if ((player_->getCatapultProperty()->getCurrentStateProperty() == CatapultState::Reset ||
             computer_->getCatapultProperty()->getCurrentStateProperty() == CatapultState::Reset) &&
            !(player_->getCatapultProperty()->getAnimationRunningProperty() ||
              computer_->getCatapultProperty()->getAnimationRunningProperty())) {
            changeTurn_ = true;

            if (player_->getIsActiveProperty()) {
                player_->setIsActiveProperty(false);
                computer_->setIsActiveProperty(true);
                isHumanTurn_ = false;
                player_->getCatapultProperty()->setCurrentStateProperty(CatapultState::Idle);
                computer_->getCatapultProperty()->setCurrentStateProperty(CatapultState::Aiming);
            } else {
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

    void DrawPlayer(const GameTime& gameTime) {
        if (!gameOver_) player_->Draw(gameTime);
    }

    void DrawComputer(const GameTime& gameTime) {
        if (!gameOver_) computer_->Draw(gameTime);
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

        Rectangle rectangle(
            static_cast<int>(playerHealthBarPosition_.X),
            static_cast<int>(playerHealthBarPosition_.Y),
            static_cast<int>(healthBarFullSize_.X) * player_->getHealthProperty() / 100,
            static_cast<int>(healthBarFullSize_.Y));
        Rectangle underRectangle(rectangle.X, rectangle.Y, rectangle.Width + 1,
                                 rectangle.Height + 1);
        spriteBatch.Draw(*blankTexture_, underRectangle, Color::Black);
        spriteBatch.Draw(*blankTexture_, rectangle, Color::Blue);

        spriteBatch.Draw(*hudBackgroundTexture_, computerHUDPosition_, Color::White);
        spriteBatch.Draw(*ammoTypeTexture_, computerHUDPosition_ + Vector2(33.0f, 35.0f),
                         Color::White);
        DrawString(*hudFont_, System::Int32::ToString(computer_->getScoreProperty()),
                   computerHUDPosition_ + Vector2(123.0f, 35.0f), Color::White);
        DrawString(*hudFont_, computer_->getNameProperty(),
                   computerHUDPosition_ + Vector2(40.0f, 1.0f), Color::Red);

        rectangle = Rectangle(
            static_cast<int>(computerHealthBarPosition_.X),
            static_cast<int>(computerHealthBarPosition_.Y),
            static_cast<int>(healthBarFullSize_.X) * computer_->getHealthProperty() / 100,
            static_cast<int>(healthBarFullSize_.Y));
        underRectangle = Rectangle(rectangle.X, rectangle.Y, rectangle.Width + 1,
                                   rectangle.Height + 1);
        spriteBatch.Draw(*blankTexture_, underRectangle, Color::Black);
        spriteBatch.Draw(*blankTexture_, rectangle, Color::Red);

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

        if (isHumanTurn_)
            text = !isDragging_ ? "Drag Anywhere to Fire" : "Release to Fire!";
        else
            text = "I'll get you yet!";
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
    std::optional<Texture2D> blankTexture_;
    std::optional<SpriteFont> hudFont_;
    Vector2 cloud1Position_ = Vector2::Zero;
    Vector2 cloud2Position_ = Vector2::Zero;
    Vector2 playerHUDPosition_ = Vector2::Zero;
    Vector2 computerHUDPosition_ = Vector2::Zero;
    Vector2 windArrowPosition_ = Vector2::Zero;
    Vector2 playerHealthBarPosition_ = Vector2::Zero;
    Vector2 computerHealthBarPosition_ = Vector2::Zero;
    Vector2 healthBarFullSize_ = Vector2::Zero;
    std::shared_ptr<Human> player_;
    std::shared_ptr<AI> computer_;
    Vector2 wind_ = Vector2::Zero;
    bool changeTurn_ = false;
    bool isHumanTurn_ = false;
    bool gameOver_ = false;
    System::Random random_;
    static constexpr int minWind = 0;
    static constexpr int maxWind = 10;
    bool isDragging_ = false;
};

} // namespace CatapultGame
