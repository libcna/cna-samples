// SPDX-License-Identifier: MS-PL
#pragma once

#include <mutex>
#include <optional>
#include <string>

#include "Microsoft/Devices/Sensors/Accelerometer.hpp"
#include "Microsoft/Devices/Sensors/AccelerometerFailedException.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "System/InvalidOperationException.hpp"

namespace Bounce
{
    struct AccelerometerState
    {
        AccelerometerState() = default;

        AccelerometerState(
            Microsoft::Xna::Framework::Vector3 acceleration,
            bool isActive)
            : acceleration_(acceleration), isActive_(isActive)
        {
        }

        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getAccelerationProperty() const
        {
            return acceleration_;
        }

        [[nodiscard]] bool getIsActiveProperty() const
        {
            return isActive_;
        }

        [[nodiscard]] std::string ToString() const
        {
            return "Acceleration: " + acceleration_.ToString() +
                   ", IsActive: " + (isActive_ ? "True" : "False");
        }

    private:
        Microsoft::Xna::Framework::Vector3 acceleration_;
        bool isActive_ = false;
    };

    class Accelerometer final
    {
    public:
        Accelerometer() = delete;

        static void Initialize()
        {
            if (isInitialized_)
                throw System::InvalidOperationException(
                    "Initialize can only be called once");

            if (Microsoft::Devices::Sensors::Accelerometer::getIsSupportedProperty())
            {
                try
                {
                    sensor_.emplace();
                    sensor_->ReadingChanged += [](
                        System::Object*,
                        const Microsoft::Devices::Sensors::AccelerometerReadingEventArgs& eventArgs)
                    {
                        const std::lock_guard lock(threadLock_);
                        nextValue_ = Microsoft::Xna::Framework::Vector3(
                            static_cast<float>(eventArgs.getXProperty()),
                            static_cast<float>(eventArgs.getYProperty()),
                            static_cast<float>(eventArgs.getZProperty()));
                    };
                    sensor_->Start();
                    isActive_ = true;
                }
                catch (const Microsoft::Devices::Sensors::AccelerometerFailedException&)
                {
                    sensor_.reset();
                    isActive_ = false;
                }
            }
            else
            {
                isEmulator_ = true;
                isActive_ = true;
            }

            isInitialized_ = true;
        }

        [[nodiscard]] static AccelerometerState GetState()
        {
            using Microsoft::Xna::Framework::Vector3;

            if (!isInitialized_)
                throw System::InvalidOperationException(
                    "You must Initialize before you can call GetState");

            Vector3 stateValue;
            if (isActive_)
            {
                if (isEmulator_)
                {
                    using namespace Microsoft::Xna::Framework::Input;
                    const KeyboardState keyboardState = Keyboard::GetState();
                    stateValue.Z = -1.0f;
                    if (keyboardState.IsKeyDown(Keys::Left))
                        --stateValue.X;
                    if (keyboardState.IsKeyDown(Keys::Right))
                        ++stateValue.X;
                    if (keyboardState.IsKeyDown(Keys::Up))
                        ++stateValue.Y;
                    if (keyboardState.IsKeyDown(Keys::Down))
                        --stateValue.Y;
                    stateValue.Normalize();
                }
                else
                {
                    const std::lock_guard lock(threadLock_);
                    stateValue = nextValue_;
                }
            }

            return AccelerometerState(stateValue, isActive_);
        }

    private:
        inline static std::optional<Microsoft::Devices::Sensors::Accelerometer> sensor_;
        inline static std::mutex threadLock_;
        inline static Microsoft::Xna::Framework::Vector3 nextValue_;
        inline static bool isInitialized_ = false;
        inline static bool isActive_ = false;
        inline static bool isEmulator_ = false;
    };
}
