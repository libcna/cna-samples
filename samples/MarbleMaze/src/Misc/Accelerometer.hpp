// SPDX-License-Identifier: MS-PL
#pragma once

#include <mutex>
#include <string>

#include "Microsoft/Devices/DeviceType.hpp"
#include "Microsoft/Devices/Environment.hpp"
#include "Microsoft/Devices/Sensors/Accelerometer.hpp"
#include "Microsoft/Devices/Sensors/AccelerometerFailedException.hpp"
#include "Microsoft/Devices/Sensors/AccelerometerReadingEventArgs.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/InvalidOperationException.hpp"

namespace MarbleMazeGame
{
    using Microsoft::Devices::DeviceType;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Input::Keyboard;
    using Microsoft::Xna::Framework::Input::Keys;

    struct AccelerometerState
    {
        Vector3 Acceleration{};
        bool IsActive = false;

        AccelerometerState() = default;
        AccelerometerState(Vector3 acceleration, bool isActive)
            : Acceleration(acceleration), IsActive(isActive)
        {
        }

        [[nodiscard]] std::string ToString() const
        {
            return "Acceleration: " + Acceleration.ToString() +
                ", IsActive: " + (IsActive ? "True" : "False");
        }
    };

    class Accelerometer final
    {
    public:
        Accelerometer() = delete;

        static void Initialize()
        {
            if (isInitialized_)
            {
                return;
            }

            if (Microsoft::Devices::Environment::getDeviceTypeProperty() == DeviceType::Device)
            {
                try
                {
                    accelerometer_.ReadingChanged +=
                        [](System::Object*,
                           const Microsoft::Devices::Sensors::AccelerometerReadingEventArgs& e)
                        {
                            std::scoped_lock lock(threadLock_);
                            nextValue_ = Vector3(
                                static_cast<float>(e.getXProperty()),
                                static_cast<float>(e.getYProperty()),
                                static_cast<float>(e.getZProperty()));
                        };
                    accelerometer_.Start();
                    isActive_ = true;
                }
                catch (const Microsoft::Devices::Sensors::AccelerometerFailedException&)
                {
                    isActive_ = false;
                }
            }
            else
            {
                isActive_ = true;
            }

            isInitialized_ = true;
        }

        [[nodiscard]] static AccelerometerState GetState()
        {
            if (!isInitialized_)
            {
                throw System::InvalidOperationException(
                    "You must Initialize before you can call GetState");
            }

            Vector3 stateValue{};
            if (isActive_)
            {
                if (Microsoft::Devices::Environment::getDeviceTypeProperty() == DeviceType::Device)
                {
                    std::scoped_lock lock(threadLock_);
                    stateValue = nextValue_;
                }
                else
                {
                    const auto keyboardState = Keyboard::GetState();
                    stateValue.Z = -1.0f;
                    if (keyboardState.IsKeyDown(Keys::Left)) stateValue.X = -0.1f;
                    if (keyboardState.IsKeyDown(Keys::Right)) stateValue.X = 0.1f;
                    if (keyboardState.IsKeyDown(Keys::Up)) stateValue.Y = 0.1f;
                    if (keyboardState.IsKeyDown(Keys::Down)) stateValue.Y = -0.1f;
                    stateValue.Normalize();
                }
            }

            return AccelerometerState(stateValue, isActive_);
        }

    private:
        inline static Microsoft::Devices::Sensors::Accelerometer accelerometer_{};
        inline static bool isInitialized_ = false;
        inline static std::mutex threadLock_;
        inline static Vector3 nextValue_{};
        inline static bool isActive_ = false;
    };
}
