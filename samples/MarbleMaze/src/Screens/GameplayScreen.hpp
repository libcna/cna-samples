// SPDX-License-Identifier: MS-PL
#pragma once

#include <any>
#include <cmath>
#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Devices/DeviceType.hpp"
#include "Microsoft/Devices/Environment.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "System/Collections/Generic/LinkedList.hpp"
#include "System/Int32.hpp"
#include "System/String.hpp"
#include "System/TimeSpan.hpp"

#include "../Misc/Accelerometer.hpp"
#include "../Misc/AudioManager.hpp"
#include "../Objects/Camera.hpp"
#include "../Objects/Marble.hpp"
#include "../Objects/Maze.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"

namespace MarbleMazeGame
{
    using GameStateManagement::GameScreen;
    using GameStateManagement::InputState;
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::GamerServices;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input::Touch;
    using Microsoft::Devices::DeviceType;
    using System::Collections::Generic::LinkedListNode;
    using System::TimeSpan;

    class BackgroundScreen;
    class CalibrationScreen;
    class HighScoreScreen;
    class PauseScreen;

    class GameplayScreen : public GameScreen
    {
    public:
        bool IsActive = true;
        Vector3 AccelerometerCalibrationData = Vector3::Zero;

        GameplayScreen()
        {
            setTransitionOnTime(TimeSpan::FromSeconds(0.0));
            setTransitionOffTime(TimeSpan::FromSeconds(0.0));
            setEnabledGestures(GestureType::Tap | GestureType::DoubleTap);
        }

        void LoadContent() override
        {
            timeFont_.emplace(Load<SpriteFont>("Fonts/MenuFont"));
            Accelerometer::Initialize();
            GameScreen::LoadContent();
        }

        void LoadAssets()
        {
            InitializeCamera();
            InitializeMaze();
            InitializeMarble();
        }

        void HandleInput(InputState& input) override
        {
            if (input.IsPauseGame(std::nullopt))
            {
                if (!gameOver_) PauseCurrentGame();
                else FinishCurrentGame();
            }

            if (IsActive && !startScreen_)
            {
                if (!input.Gestures.empty() &&
                    input.Gestures[0].getGestureTypeProperty() == GestureType::Tap && gameOver_)
                {
                    FinishCurrentGame();
                }

                if (!gameOver_)
                {
                    if (Microsoft::Devices::Environment::getDeviceTypeProperty() == DeviceType::Device &&
                        !input.Gestures.empty() &&
                        input.Gestures[0].getGestureTypeProperty() == GestureType::DoubleTap)
                    {
                        CalibrateGame();
                        input.Gestures.clear();
                    }

                    Vector3 currentAccelerometerState = Accelerometer::GetState().Acceleration;
                    currentAccelerometerState.X -= AccelerometerCalibrationData.X;
                    currentAccelerometerState.Y -= AccelerometerCalibrationData.Y;
                    currentAccelerometerState.Z -= AccelerometerCalibrationData.Z;

                    if (Microsoft::Devices::Environment::getDeviceTypeProperty() == DeviceType::Device)
                    {
                        maze_->Rotation.Z = static_cast<float>(std::round(
                            MathHelper::ToRadians(currentAccelerometerState.Y * 30.0f) * 100.0f) / 100.0f);
                        maze_->Rotation.X = -static_cast<float>(std::round(
                            MathHelper::ToRadians(currentAccelerometerState.X * 30.0f) * 100.0f) / 100.0f);
                    }
                    else if (Microsoft::Devices::Environment::getDeviceTypeProperty() == DeviceType::Emulator)
                    {
                        Vector3 rotation = Vector3::Zero;
                        if (currentAccelerometerState.X != 0.0f)
                        {
                            rotation += currentAccelerometerState.X > 0.0f
                                ? Vector3(0, 0, -angularVelocity_)
                                : Vector3(0, 0, angularVelocity_);
                        }
                        if (currentAccelerometerState.Y != 0.0f)
                        {
                            rotation += currentAccelerometerState.Y > 0.0f
                                ? Vector3(-angularVelocity_, 0, 0)
                                : Vector3(angularVelocity_, 0, 0);
                        }

                        maze_->Rotation.X = MathHelper::Clamp(
                            maze_->Rotation.X + rotation.X,
                            MathHelper::ToRadians(-30.0f), MathHelper::ToRadians(30.0f));
                        maze_->Rotation.Z = MathHelper::Clamp(
                            maze_->Rotation.Z + rotation.Z,
                            MathHelper::ToRadians(-30.0f), MathHelper::ToRadians(30.0f));
                    }
                }
            }
        }

        void Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen) override
        {
            if (IsActive && !gameOver_)
            {
                if (!startScreen_)
                {
                    elapsedGameTime_ = elapsedGameTime_ + gameTime.getElapsedGameTimeProperty();
                    CheckFallInPit();
                    UpdateLastCheackpoint();
                }

                maze_->Update(gameTime);
                marble_->Update(gameTime);
                camera_->Update(gameTime);
                CheckGameFinish();
                GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
            }

            if (startScreen_)
            {
                if (startScreenTime_.getTicksProperty() > 0)
                    startScreenTime_ = startScreenTime_ - gameTime.getElapsedGameTimeProperty();
                else
                    startScreen_ = false;
            }
        }

        void Draw(const GameTime& gameTime) override
        {
            auto& screenManager = *GetScreenManager();
            auto& device = screenManager.getGraphicsDeviceProperty();
            auto& spriteBatch = screenManager.getSpriteBatch();

            device.Clear(Color::Black);
            spriteBatch.Begin();

            if (startScreen_) DrawStartGame();

            if (IsActive)
            {
                spriteBatch.DrawString(
                    *timeFont_,
                    System::String::Format(
                        "{0:00}:{1:00}",
                        static_cast<int>(elapsedGameTime_.getMinutesProperty()),
                        static_cast<int>(elapsedGameTime_.getSecondsProperty())),
                    Vector2(20, 20), Color::YellowGreen);

                DepthStencilState depthStensilState;
                depthStensilState.setDepthBufferEnableProperty(true);
                device.setDepthStencilStateProperty(depthStensilState);

                maze_->Draw(gameTime);
                marble_->Draw(gameTime);
            }

            if (gameOver_)
            {
                AudioManager::StopSounds();
                DrawEndGame();
            }

            spriteBatch.End();
            GameScreen::Draw(gameTime);
        }

        void Restart()
        {
            marble_->Position = maze_->StartPoistion;
            marble_->Velocity = Vector3::Zero;
            marble_->Acceleration = Vector3::Zero;
            maze_->Rotation = Vector3::Zero;
            IsActive = true;
            gameOver_ = false;
            elapsedGameTime_ = TimeSpan::Zero;
            startScreen_ = true;
            startScreenTime_ = TimeSpan::FromSeconds(4);
            lastCheackpointNode_ = maze_->Checkpoints.getFirstProperty();
        }

    private:
        void InitializeCamera()
        {
            camera_ = std::make_unique<Camera>(
                GetScreenManager()->getGameProperty(),
                GetScreenManager()->getGraphicsDeviceProperty());
            camera_->Initialize();
        }

        void InitializeMaze()
        {
            maze_ = std::make_unique<Maze>(GetScreenManager()->getGameProperty());
            maze_->Position = Vector3::Zero;
            maze_->Camera = camera_.get();
            maze_->Initialize();
            lastCheackpointNode_ = maze_->Checkpoints.getFirstProperty();
        }

        void InitializeMarble()
        {
            marble_ = std::make_unique<Marble>(GetScreenManager()->getGameProperty());
            marble_->Position = maze_->StartPoistion;
            marble_->Camera = camera_.get();
            marble_->Maze = maze_.get();
            marble_->Initialize();
        }

        void UpdateLastCheackpoint()
        {
            const BoundingSphere marblePosition = marble_->BoundingSphereTransformed();
            auto current = lastCheackpointNode_;
            while (current.getNextProperty())
            {
                const auto next = current.getNextProperty();
                if (std::abs(Vector3::Distance(
                        marblePosition.Center, next.getValueProperty())) <= marblePosition.Radius * 3.0f)
                {
                    AudioManager::PlaySound("checkpoint");
                    lastCheackpointNode_ = next;
                    return;
                }
                current = next;
            }
        }

        void CheckFallInPit()
        {
            if (marble_->Position.Y < -150.0f)
            {
                marble_->Position = lastCheackpointNode_.getValueProperty();
                maze_->Rotation = Vector3::Zero;
                marble_->Acceleration = Vector3::Zero;
                marble_->Velocity = Vector3::Zero;
            }
        }

        void CheckGameFinish()
        {
            const BoundingSphere marblePosition = marble_->BoundingSphereTransformed();
            if (std::abs(Vector3::Distance(marblePosition.Center, maze_->End)) <=
                marblePosition.Radius * 3.0f)
            {
                gameOver_ = true;
            }
        }

        void FinishCurrentGame();
        void PauseCurrentGame();
        void CalibrateGame();

        void DrawEndGame()
        {
            std::string text = HighScoreIsInHighscores()
                ? "    You got a High Score!"
                : "          Game Over";
            text += "\nTouch the screen to continue";

            auto& manager = *GetScreenManager();
            const Vector2 size = timeFont_->MeasureString(text);
            const auto& viewport = manager.getGraphicsDeviceProperty().getViewportProperty();
            const Vector2 textPosition =
                (Vector2(static_cast<float>(viewport.getWidthProperty()),
                         static_cast<float>(viewport.getHeightProperty())) - size) / 2.0f;
            manager.getSpriteBatch().DrawString(*timeFont_, text, textPosition, Color::White);
        }

        void DrawStartGame()
        {
            const std::string text = startScreenTime_.getSecondsProperty() == 0
                ? "Go!"
                : System::Int32::ToString(static_cast<int>(startScreenTime_.getSecondsProperty()));
            auto& manager = *GetScreenManager();
            const Vector2 size = timeFont_->MeasureString(text);
            const auto& viewport = manager.getGraphicsDeviceProperty().getViewportProperty();
            const Vector2 textPosition =
                (Vector2(static_cast<float>(viewport.getWidthProperty()),
                         static_cast<float>(viewport.getHeightProperty())) - size) / 2.0f;
            manager.getSpriteBatch().DrawString(*timeFont_, text, textPosition, Color::White);
        }

        [[nodiscard]] bool HighScoreIsInHighscores() const;

        bool gameOver_ = false;
        bool startScreen_ = true;
        TimeSpan startScreenTime_ = TimeSpan::FromSeconds(4);
        std::unique_ptr<Maze> maze_;
        std::unique_ptr<Marble> marble_;
        std::unique_ptr<Camera> camera_;
        LinkedListNode<Vector3> lastCheackpointNode_;
        const float angularVelocity_ = MathHelper::ToRadians(1.5f);
        std::optional<SpriteFont> timeFont_;
        TimeSpan elapsedGameTime_ = TimeSpan::Zero;
    };
}
