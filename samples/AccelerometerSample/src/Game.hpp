// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Accelerometer.hpp"
#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "System/TimeSpan.hpp"

namespace AccelerometerSample {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::GraphicsDeviceManager;
using Microsoft::Xna::Framework::PlayerIndex;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteSortMode;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Graphics::Viewport;
using Microsoft::Xna::Framework::Input::ButtonState;
using Microsoft::Xna::Framework::Input::GamePad;
using XnaGame = Microsoft::Xna::Framework::Game;

/** @brief Demonstrates moving a sprite with accelerometer input. */
class Game final : public XnaGame {
public:
    /** @brief Creates the sample game with its Windows Phone presentation settings. */
    Game() : graphics_(this) {
        graphics_.setPreferredBackBufferWidthProperty(480);
        graphics_.setPreferredBackBufferHeightProperty(800);
        graphics_.setIsFullScreenProperty(true);

        getContentProperty().setRootDirectoryProperty("Content");
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
    }

    /**
     * @brief Returns the fully-qualified logical type name.
     *
     * @return `AccelerometerSample.Game`.
     */
    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "AccelerometerSample.Game";
        return name;
    }

protected:
    /** @brief Initializes the sample accelerometer and base game. */
    void Initialize() override {
        Accelerometer::Initialize();
        XnaGame::Initialize();
    }

    /** @brief Loads the original asteroid and space textures and centers the asteroid. */
    void LoadContent() override {
        spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());

        auto& content = getContentProperty();
        asteroidTexture_.emplace(content.Load<Texture2D>("asteroid"));
        backgroundTexture_.emplace(content.Load<Texture2D>("space"));

        const Viewport& viewport = graphics_.getGraphicsDeviceProperty()->getViewportProperty();
        logoPosition_ = Vector2(
            static_cast<float>((viewport.getWidthProperty() -
                                asteroidTexture_->getWidthProperty()) / 2),
            static_cast<float>((viewport.getHeightProperty() -
                                asteroidTexture_->getHeightProperty()) / 2));
    }

    /**
     * @brief Polls acceleration, moves the asteroid and clamps it to the viewport.
     *
     * @param gameTime Current frame timing.
     */
    void Update(GameTime& gameTime) override {
        if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getBackProperty() ==
                ButtonState::Pressed) {
            Exit();
        }

        const Vector3 acceleration = Accelerometer::GetState().getAccelerationProperty();
        logoVelocity_.X += acceleration.X;
        logoVelocity_.Y += -acceleration.Y;
        logoPosition_ += logoVelocity_;

        const Viewport& viewport = graphics_.getGraphicsDeviceProperty()->getViewportProperty();

        if (logoPosition_.X < 0.0f) {
            logoPosition_.X = 0.0f;
            logoVelocity_.X = 0.0f;
        } else if (logoPosition_.X >
                   viewport.getWidthProperty() - asteroidTexture_->getWidthProperty()) {
            logoPosition_.X = static_cast<float>(
                viewport.getWidthProperty() - asteroidTexture_->getWidthProperty());
            logoVelocity_.X = 0.0f;
        }

        if (logoPosition_.Y < 0.0f) {
            logoPosition_.Y = 0.0f;
            logoVelocity_.Y = 0.0f;
        } else if (logoPosition_.Y >
                   viewport.getHeightProperty() - asteroidTexture_->getHeightProperty()) {
            logoPosition_.Y = static_cast<float>(
                viewport.getHeightProperty() - asteroidTexture_->getHeightProperty());
            logoVelocity_.Y = 0.0f;
        }

        XnaGame::Update(gameTime);
    }

    /**
     * @brief Draws the space background and asteroid.
     *
     * @param gameTime Current frame timing.
     */
    void Draw(const GameTime& gameTime) override {
        getGraphicsDeviceProperty().Clear(Color::White);

        spriteBatch_->Begin(SpriteSortMode::Immediate, BlendState::AlphaBlend);
        spriteBatch_->Draw(*backgroundTexture_, Vector2::Zero, Color::White);
        spriteBatch_->Draw(*asteroidTexture_, logoPosition_, Color::White);
        spriteBatch_->End();

        XnaGame::Draw(gameTime);
    }

private:
    GraphicsDeviceManager graphics_;
    std::unique_ptr<SpriteBatch> spriteBatch_;
    std::optional<Texture2D> asteroidTexture_;
    std::optional<Texture2D> backgroundTexture_;
    Vector2 logoPosition_;
    Vector2 logoVelocity_;
};

} // namespace AccelerometerSample
