// SPDX-License-Identifier: MS-PL
#pragma once

#include <cmath>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocationState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/Int32.hpp"
#include "System/NotImplementedException.hpp"
#include "System/Random.hpp"
#include "System/String.hpp"
#include "System/TimeSpan.hpp"

#if defined(WINDOWS_PHONE)
#include "Microsoft/Devices/Sensors/Accelerometer.hpp"
#include "Microsoft/Devices/Sensors/AccelerometerReadingEventArgs.hpp"
#include "Microsoft/Devices/Sensors/SensorState.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#endif

namespace SnowShovel {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::GraphicsDeviceManager;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::PlayerIndex;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Audio::SoundEffect;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::SpriteSortMode;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Input::ButtonState;
using Microsoft::Xna::Framework::Input::Buttons;
using Microsoft::Xna::Framework::Input::GamePad;
using Microsoft::Xna::Framework::Input::GamePadState;
using Microsoft::Xna::Framework::Input::Keyboard;
using Microsoft::Xna::Framework::Input::KeyboardState;
using Microsoft::Xna::Framework::Input::Keys;
using Microsoft::Xna::Framework::Input::Touch::TouchCollection;
using Microsoft::Xna::Framework::Input::Touch::TouchLocationState;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;
using XnaGame = Microsoft::Xna::Framework::Game;

/**
 * @brief Simple accelerometer game in which the player catches snowflakes with a shovel.
 */
class Game final : public XnaGame {
private:
    /** @brief Encapsulates the state used to draw and move one snowflake. */
    class Snowflake {
    public:
        Vector2 Position;
        Vector2 Velocity;
        float Scale = 0.0f;
        float Rotation = 0.0f;
        float AngularVelocity = 0.0f;
        int TextureIndex = 0;
        Color Tint;

        /**
         * @brief Creates a snowflake with the supplied position, velocity and sprite index.
         *
         * @param position Initial position in the 272 by 480 world.
         * @param velocity Initial per-frame velocity.
         * @param index Sprite-sheet frame index.
         */
        Snowflake(Vector2 position, Vector2 velocity, int index) {
            Position = position;
            Velocity = velocity;
            TextureIndex = index;

            Scale = 1.0f - static_cast<float>(0.5 * random_.NextDouble());
            AngularVelocity = static_cast<float>(random_.NextDouble()) * 0.05f;

            Tint.setAProperty(255);
            Tint.setRProperty(static_cast<SharpRuntime::bytecs>(255 - (40.0 * random_.NextDouble())));
            Tint.setGProperty(static_cast<SharpRuntime::bytecs>(255 - (40.0 * random_.NextDouble())));
            Tint.setBProperty(static_cast<SharpRuntime::bytecs>(255 - (20.0 * random_.NextDouble())));
        }

        /**
         * @brief Advances the snowflake and bounces it off the world bounds.
         *
         * @param screen World rectangle used for collision bounds.
         */
        void Update(Rectangle screen) {
            Position += Velocity;

            if (Position.X < 0) {
                Velocity.X = -Velocity.X;
                Position.X = 0.0f;
            } else if (Position.X > screen.Width) {
                Velocity.X = -Velocity.X;
                Position.X = static_cast<float>(screen.Width);
            }

            if (Position.Y < 0) {
                Velocity.Y = -Velocity.Y;
                Position.Y = 0.0f;
            } else if (Position.Y > screen.Height) {
                Velocity.Y = -Velocity.Y;
                Position.Y = static_cast<float>(screen.Height);
            }

            Rotation += AngularVelocity;
        }

    private:
        inline static System::Random random_;
    };

    enum class GameState {
        PreGame,
        Game,
        PostGame
    };

public:
    /** @brief Creates the game and configures its 480 by 800 presentation. */
    Game() : graphics_(this) {
        random_ = System::Random();

        graphics_.setPreferredBackBufferWidthProperty(480);
        graphics_.setPreferredBackBufferHeightProperty(800);

#if defined(WINDOWS_PHONE)
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
        graphics_.setIsFullScreenProperty(true);
#endif

        getContentProperty().setRootDirectoryProperty("Content");
    }

#if defined(WINDOWS_PHONE)
    /**
     * @brief Stores the newest accelerometer reading supplied by the Phone sensor event.
     *
     * @param sender Sensor that raised the event.
     * @param eventArgs Latest acceleration values.
     */
    void AccelerometerReadingChanged(
        System::Object* sender,
        const Microsoft::Devices::Sensors::AccelerometerReadingEventArgs& eventArgs) {
        (void)sender;
        CurrentAccelerometerReading_.X = static_cast<float>(eventArgs.getXProperty());
        CurrentAccelerometerReading_.Y = static_cast<float>(eventArgs.getYProperty());
        CurrentAccelerometerReading_.Z = static_cast<float>(eventArgs.getZProperty());
    }
#endif

    /**
     * @brief Returns the fully-qualified logical type name.
     *
     * @return `SnowShovel.Game`.
     */
    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "SnowShovel.Game";
        return name;
    }

protected:
    /** @brief Initializes the sensor branch, logical world and initial game state. */
    void Initialize() override {
#if defined(WINDOWS_PHONE)
        Accelerometer_.emplace();
        if (Accelerometer_->getStateProperty() == Microsoft::Devices::Sensors::SensorState::Ready) {
            Accelerometer_->ReadingChanged += [this](
                System::Object* sender,
                const Microsoft::Devices::Sensors::AccelerometerReadingEventArgs& eventArgs) {
                AccelerometerReadingChanged(sender, eventArgs);
            };
            Accelerometer_->Start();
            CurrentAccelerometerState_ = Microsoft::Devices::Sensors::SensorState::Ready;
        }
#endif

        worldRect_ = Rectangle(0, 0, 272, 480);
        const auto& viewport = getGraphicsDeviceProperty().getViewportProperty();
        worldToScreenMatrix_ = Matrix::CreateScale(
            static_cast<float>(viewport.getWidthProperty()) / static_cast<float>(worldRect_.Width),
            static_cast<float>(viewport.getHeightProperty()) / static_cast<float>(worldRect_.Height),
            1.0f);

        SetGameState(GameState::PreGame);
        XnaGame::Initialize();
    }

    /** @brief Loads the original textures, fonts and sound effect. */
    void LoadContent() override {
        spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        auto& content = getContentProperty();
        shovelTexture_.emplace(content.Load<Texture2D>("shovel"));
        snowTexture_.emplace(content.Load<Texture2D>("snowflakes"));
        titleFont_.emplace(content.Load<SpriteFont>("TitleFont"));
        scoreFont_.emplace(content.Load<SpriteFont>("ScoreFont"));
        sound_.emplace(content.Load<SoundEffect>("plink"));
    }

    /**
     * @brief Updates input, timing, snowflakes and the active game state.
     *
     * @param gameTime Current frame timing.
     */
    void Update(GameTime& gameTime) override {
        touchCollection_ = TouchPanel::GetState();
        gamepadState_ = GamePad::GetState(PlayerIndex::One);
#if defined(WINDOWS_PHONE)
        keyboardState_ = KeyboardState();
#else
        keyboardState_ = Keyboard::GetState();
#endif

        if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getBackProperty() ==
                ButtonState::Pressed ||
            keyboardState_.IsKeyDown(Keys::Escape)) {
            Exit();
        }

        if (timeRemaining_.getTotalMillisecondsProperty() > 0) {
            timeElapsed_ += gameTime.getElapsedGameTimeProperty();
            timeRemaining_ -= gameTime.getElapsedGameTimeProperty();
        }

        for (Snowflake& snowflake : snowFlakes_)
            snowflake.Update(worldRect_);

        switch (gameState_) {
            case GameState::PreGame:
                UpdatePreGame();
                break;
            case GameState::PostGame:
                UpdatePostGame();
                break;
            case GameState::Game:
                UpdateGame(gameTime);
                break;
            default:
                throw System::NotImplementedException();
        }

        XnaGame::Update(gameTime);
    }

    /**
     * @brief Draws the shovel, snowflakes, HUD and state instructions.
     *
     * @param gameTime Current frame timing.
     */
    void Draw(const GameTime& gameTime) override {
        getGraphicsDeviceProperty().Clear(Color::Black);

        spriteBatch_->Begin(
            SpriteSortMode::Immediate,
            BlendState::NonPremultiplied,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            worldToScreenMatrix_);

        spriteBatch_->Draw(
            *shovelTexture_,
            shovelPosition_,
            std::nullopt,
            Color::White,
            shovelRotation_,
            Vector2(
                static_cast<float>(shovelTexture_->getWidthProperty() / 2),
                static_cast<float>(shovelTexture_->getHeightProperty() / 2)),
            1.0f,
            SpriteEffects::None,
            0.0f);

        for (const Snowflake& snowflake : snowFlakes_) {
            const int frameSize = snowTexture_->getHeightProperty();
            spriteBatch_->Draw(
                *snowTexture_,
                snowflake.Position,
                Rectangle(snowflake.TextureIndex * frameSize, 0, frameSize, frameSize),
                snowflake.Tint,
                snowflake.Rotation,
                Vector2(static_cast<float>(frameSize / 2), static_cast<float>(frameSize / 2)),
                snowflake.Scale,
                SpriteEffects::None,
                0.0f);
        }

        DrawStringHelper(*spriteBatch_, *titleFont_, "Snow Shovel", 0, 0, Color::Wheat);

        const std::string timeRemainingString = System::String::Format(
            "Time: {0:00}:{1:00.0}",
            timeRemaining_.getMinutesProperty(),
            static_cast<float>(timeRemaining_.getSecondsProperty()) +
                static_cast<float>(timeRemaining_.getMillisecondsProperty()) / 1000.0f);

        Color timeColor = Color::White;
        if (timeRemaining_.getTotalMillisecondsProperty() < 10000) {
            timeColor.setBProperty(static_cast<SharpRuntime::bytecs>(
                255 * timeRemaining_.getTotalMillisecondsProperty() / 10000));
            timeColor.setGProperty(static_cast<SharpRuntime::bytecs>(
                255 * timeRemaining_.getTotalMillisecondsProperty() / 10000));
        }

        DrawStringHelper(*spriteBatch_, *scoreFont_, timeRemainingString, 0, 30, timeColor);

        const std::string scoreString = "Score: " + System::Int32::ToString(score_);
        Vector2 stringDimensions = scoreFont_->MeasureString(scoreString);
        DrawStringHelper(
            *spriteBatch_, *scoreFont_, scoreString,
            static_cast<int>(worldRect_.Width - stringDimensions.X), 30, Color::White);

        const std::string timeElapsedString = System::String::Format(
            "Elapsed Time: {0:00}:{1:00.0}",
            timeElapsed_.getMinutesProperty(),
            static_cast<float>(timeElapsed_.getSecondsProperty()) +
                static_cast<float>(timeElapsed_.getMillisecondsProperty()) / 1000.0f);
        stringDimensions = scoreFont_->MeasureString(timeElapsedString);
        DrawStringHelper(
            *spriteBatch_, *scoreFont_, timeElapsedString,
            static_cast<int>(worldRect_.Width - stringDimensions.X) / 2,
            worldRect_.Height - static_cast<int>(stringDimensions.Y),
            Color::White);

        switch (gameState_) {
            case GameState::PreGame: {
                static const std::string instructionStrings[] = {
                    "Shovel snow before", "time runs out!", "Tap screen to start"};
                for (int i = 0; i < 3; ++i) {
                    stringDimensions = scoreFont_->MeasureString(instructionStrings[i]);
                    DrawStringHelper(
                        *spriteBatch_, *scoreFont_, instructionStrings[i],
                        static_cast<int>(worldRect_.Width - stringDimensions.X) / 2,
                        100 + i * 30,
                        Color::White);
                }
                break;
            }
            case GameState::PostGame: {
                static const std::string instructionStrings[] = {
                    "GAME OVER", "Old Man Winter has Pwned you", "Tap screen to restart"};
                for (int i = 0; i < 3; ++i) {
                    stringDimensions = scoreFont_->MeasureString(instructionStrings[i]);
                    DrawStringHelper(
                        *spriteBatch_, *scoreFont_, instructionStrings[i],
                        static_cast<int>(worldRect_.Width - stringDimensions.X) / 2,
                        100 + i * 30,
                        Color::Aqua);
                }
                break;
            }
            default:
                break;
        }

        spriteBatch_->End();
        XnaGame::Draw(gameTime);
    }

private:
    void SetGameState(GameState state) {
        gameState_ = state;
        snowFlakes_.clear();

        switch (state) {
            case GameState::PreGame:
                score_ = 0;
                timeElapsed_ = System::TimeSpan::Zero;
                nextWaveMilliseconds_ = 10000;
                nextWaveSnowflakeCount_ = 5;
                shovelPosition_.X = static_cast<float>(worldRect_.getCenterProperty().X);
                shovelPosition_.Y = static_cast<float>(worldRect_.getCenterProperty().Y);
                shovelVelocity_.X = 0.0f;
                shovelVelocity_.Y = 0.0f;
                break;
            case GameState::Game:
                timeRemaining_ = System::TimeSpan::FromSeconds(10);
                break;
            case GameState::PostGame:
                timeRemaining_ = System::TimeSpan::Zero;
                break;
        }

        Snow(nextWaveSnowflakeCount_);
    }

    void UpdatePreGame() {
        if (touchCollection_.getCountProperty() > 0 ||
            gamepadState_.IsButtonDown(Buttons::A) ||
            keyboardState_.IsKeyDown(Keys::Space)) {
            SetGameState(GameState::Game);
        }
    }

    void UpdatePostGame() {
        if ((touchCollection_.getCountProperty() > 0 &&
             touchCollection_[0].getStateProperty() == TouchLocationState::Pressed) ||
            gamepadState_.IsButtonDown(Buttons::A) ||
            keyboardState_.IsKeyDown(Keys::Space)) {
            SetGameState(GameState::PreGame);
        }
    }

    void UpdateGame(GameTime& gameTime) {
        if (timeRemaining_ < System::TimeSpan::Zero) {
            SetGameState(GameState::PostGame);
            return;
        }

        if (snowFlakes_.empty()) {
            nextWaveSnowflakeCount_ += 5;
            if (nextWaveMilliseconds_ > 500)
                nextWaveMilliseconds_ -= 500;
            timeRemaining_ += System::TimeSpan::FromMilliseconds(nextWaveMilliseconds_);
            Snow(nextWaveSnowflakeCount_);
        }

        const float pixelSeconds = 10.0f *
            static_cast<float>(gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());
        float accelX = gamepadState_.getThumbSticksProperty().getLeftProperty().X;
        float accelY = -gamepadState_.getThumbSticksProperty().getLeftProperty().Y;

        if (touchCollection_.getCountProperty() > 0) {
            const Vector2 shovelPositionScreen =
                Vector2::Transform(shovelPosition_, worldToScreenMatrix_);
            const Vector2 touchDifference =
                touchCollection_[0].getPositionProperty() - shovelPositionScreen;
            const auto& viewport = getGraphicsDeviceProperty().getViewportProperty();
            accelX = MathHelper::Clamp(
                touchDifference.X / static_cast<float>(viewport.getWidthProperty()) * 2.0f,
                -1.0f,
                1.0f);
            accelY = MathHelper::Clamp(
                touchDifference.Y / static_cast<float>(viewport.getHeightProperty()) * 2.0f,
                -1.0f,
                1.0f);
        }

        if (keyboardState_.IsKeyDown(Keys::Left))
            accelX = -1.0f;
        if (keyboardState_.IsKeyDown(Keys::Right))
            accelX = 1.0f;
        if (keyboardState_.IsKeyDown(Keys::Up))
            accelY = -1.0f;
        if (keyboardState_.IsKeyDown(Keys::Down))
            accelY = 1.0f;

#if defined(WINDOWS_PHONE)
        if (CurrentAccelerometerState_ == Microsoft::Devices::Sensors::SensorState::Ready) {
            accelX = CurrentAccelerometerReading_.X;
            accelY = -CurrentAccelerometerReading_.Y;
        }
#endif

        shovelVelocity_.X += accelX * pixelSeconds;
        shovelVelocity_.Y += accelY * pixelSeconds;
        shovelPosition_.X += shovelVelocity_.X;
        shovelPosition_.Y += shovelVelocity_.Y;

        if (accelX != 0 || accelY != 0)
            shovelRotation_ = static_cast<float>(
                MathHelper::TwoPi - std::atan2(accelX, accelY));

        if (shovelPosition_.X < 0) {
            shovelVelocity_.X = 0.0f;
            shovelPosition_.X = 0.0f;
        } else if (shovelPosition_.X > worldRect_.Width) {
            shovelVelocity_.X = 0.0f;
            shovelPosition_.X = static_cast<float>(worldRect_.Width);
        }

        if (shovelPosition_.Y < 0) {
            shovelVelocity_.Y = 0.0f;
            shovelPosition_.Y = 0.0f;
        } else if (shovelPosition_.Y > worldRect_.Height) {
            shovelVelocity_.Y = 0.0f;
            shovelPosition_.Y = static_cast<float>(worldRect_.Height);
        }

        const Rectangle shovelRect(
            static_cast<int>(shovelPosition_.X) - shovelTexture_->getWidthProperty() / 2,
            static_cast<int>(shovelPosition_.Y) - shovelTexture_->getHeightProperty() / 2,
            shovelTexture_->getWidthProperty(),
            shovelTexture_->getHeightProperty());

        for (auto iterator = snowFlakes_.begin(); iterator != snowFlakes_.end();) {
            if (shovelRect.Contains(
                    static_cast<int>(iterator->Position.X),
                    static_cast<int>(iterator->Position.Y))) {
                score_ += pointsPerSnowflake_;
                sound_->Play();
                iterator = snowFlakes_.erase(iterator);
            } else {
                ++iterator;
            }
        }
    }

    void Snow(int snowflakeCount) {
        for (int i = 0; i < snowflakeCount; ++i) {
            const float positionX =
                static_cast<float>(random_.NextDouble()) * worldRect_.Width;
            const float positionY =
                static_cast<float>(random_.NextDouble()) * worldRect_.Height;
            const float velocityX =
                static_cast<float>(random_.NextDouble()) * 2.0f - 1.0f;
            const float velocityY =
                static_cast<float>(random_.NextDouble()) * 2.0f - 1.0f;
            const int textureIndex = random_.Next(5);
            snowFlakes_.emplace_back(
                Vector2(positionX, positionY),
                Vector2(velocityX, velocityY),
                textureIndex);
        }
    }

    void DrawStringHelper(
        SpriteBatch& batch,
        const SpriteFont& font,
        const std::string& text,
        int x,
        int y,
        Color color) {
        batch.DrawString(font, text, Vector2(static_cast<float>(x + 1), static_cast<float>(y + 1)), Color::Black);
        batch.DrawString(font, text, Vector2(static_cast<float>(x), static_cast<float>(y)), color);
    }

    GameState gameState_ = GameState::PreGame;
    GraphicsDeviceManager graphics_;
    std::unique_ptr<SpriteBatch> spriteBatch_;

    TouchCollection touchCollection_;
    GamePadState gamepadState_;
    KeyboardState keyboardState_;

    std::optional<Texture2D> shovelTexture_;
    std::optional<Texture2D> snowTexture_;

    Vector2 shovelPosition_;
    Vector2 shovelVelocity_;
    float shovelRotation_ = 0.0f;

    Rectangle worldRect_;
    Matrix worldToScreenMatrix_;

    std::optional<SpriteFont> titleFont_;
    std::optional<SpriteFont> scoreFont_;

    std::optional<SoundEffect> sound_;

    System::Random random_;
    std::vector<Snowflake> snowFlakes_;

    System::TimeSpan timeRemaining_;
    System::TimeSpan timeElapsed_;

    inline static constexpr int pointsPerSnowflake_ = 100;

    int score_ = 0;
    int nextWaveSnowflakeCount_ = 0;
    int nextWaveMilliseconds_ = 0;

#if defined(WINDOWS_PHONE)
    std::optional<Microsoft::Devices::Sensors::Accelerometer> Accelerometer_;
    Microsoft::Xna::Framework::Vector3 CurrentAccelerometerReading_;
    Microsoft::Devices::Sensors::SensorState CurrentAccelerometerState_ =
        Microsoft::Devices::Sensors::SensorState::NotSupported;
#endif
};

} // namespace SnowShovel
