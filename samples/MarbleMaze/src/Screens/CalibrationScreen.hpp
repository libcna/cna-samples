// SPDX-License-Identifier: MS-PL
#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>

#include "Microsoft/Devices/Sensors/Accelerometer.hpp"
#include "Microsoft/Devices/Sensors/AccelerometerReadingEventArgs.hpp"
#include "Microsoft/Devices/Sensors/SensorState.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/DateTime.hpp"
#include "System/Threading/Thread.hpp"
#include "System/TimeSpan.hpp"

#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"
#include "BackgroundScreen.hpp"
#include "GameplayScreen.hpp"

namespace MarbleMazeGame
{
    using GameStateManagement::GameScreen;
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using Microsoft::Devices::Sensors::SensorState;
    using System::Threading::Thread;

    class CalibrationScreen : public GameScreen
    {
    public:
        explicit CalibrationScreen(GameplayScreen& gameplayScreen)
            : gameplayScreen_(&gameplayScreen)
        {
            setTransitionOnTime(System::TimeSpan::FromSeconds(0));
            setTransitionOffTime(System::TimeSpan::FromSeconds(0.5));
            setIsPopup(true);
        }

        void LoadContent() override
        {
            background_.emplace(Load<Texture2D>("Images/titleScreen"));
            font_.emplace(Load<SpriteFont>("Fonts/MenuFont"));

            thread_ = std::make_unique<Thread>([this] { Calibrate(); });
            isCalibrating_.store(true);
            startTime_ = System::DateTime::getNowProperty();
            thread_->Start();
        }

        void Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen) override
        {
            if (!isCalibrating_.load())
            {
                {
                    std::scoped_lock lock(calibrationLock_);
                    gameplayScreen_->AccelerometerCalibrationData = accelerometerCalibrationData_;
                }

                for (const auto& screen : GetScreenManager()->GetScreens())
                {
                    if (dynamic_cast<BackgroundScreen*>(screen.get()) != nullptr)
                    {
                        screen->ExitScreen();
                        break;
                    }
                }

                auto gameplay = std::dynamic_pointer_cast<GameplayScreen>(
                    GetScreenManager()->GetScreens()[0]);
                gameplay->IsActive = true;
                ExitScreen();
            }

            GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
        }

        void Draw(const GameTime&) override
        {
            auto& manager = *GetScreenManager();
            auto& spriteBatch = manager.getSpriteBatch();
            spriteBatch.Begin();
            spriteBatch.Draw(*background_, Vector2(0, 0), Color::White * TransitionAlpha());

            if (isCalibrating_.load())
            {
                const std::string text = "Calibrating...";
                const Vector2 size = font_->MeasureString(text);
                const auto& viewport = manager.getGraphicsDeviceProperty().getViewportProperty();
                const Vector2 position(
                    (static_cast<float>(viewport.getWidthProperty()) - size.X) / 2.0f,
                    (static_cast<float>(viewport.getHeightProperty()) - size.Y) / 2.0f);
                spriteBatch.DrawString(*font_, text, position, Color::White);
            }

            spriteBatch.End();
        }

    private:
        void Calibrate()
        {
            accelerometer_ = std::make_unique<Microsoft::Devices::Sensors::Accelerometer>();
            if (accelerometer_->getStateProperty() == SensorState::Initializing ||
                accelerometer_->getStateProperty() == SensorState::Ready)
            {
                accelerometer_->ReadingChanged +=
                    [this](System::Object*,
                           const Microsoft::Devices::Sensors::AccelerometerReadingEventArgs& e)
                    {
                        const Vector3 accelerometerState(
                            static_cast<float>(e.getXProperty()),
                            static_cast<float>(e.getYProperty()),
                            static_cast<float>(e.getZProperty()));

                        std::scoped_lock lock(calibrationLock_);
                        ++samplesCount_;
                        accelerometerCalibrationData_ += accelerometerState;

                        if (System::DateTime::getNowProperty() >= startTime_.AddSeconds(5))
                        {
                            accelerometer_->Stop();
                            accelerometerCalibrationData_.X /= static_cast<float>(samplesCount_);
                            accelerometerCalibrationData_.Y /= static_cast<float>(samplesCount_);
                            accelerometerCalibrationData_.Z /= static_cast<float>(samplesCount_);
                            isCalibrating_.store(false);
                        }
                    };
            }
            accelerometer_->Start();
        }

        std::optional<Texture2D> background_;
        std::optional<SpriteFont> font_;
        std::atomic<bool> isCalibrating_{false};
        GameplayScreen* gameplayScreen_ = nullptr;
        std::unique_ptr<Thread> thread_;
        std::unique_ptr<Microsoft::Devices::Sensors::Accelerometer> accelerometer_;
        Vector3 accelerometerCalibrationData_ = Vector3::Zero;
        System::DateTime startTime_{};
        std::int64_t samplesCount_ = 0;
        std::mutex calibrationLock_;
    };
}
