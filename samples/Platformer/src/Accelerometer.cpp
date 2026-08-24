// SPDX-License-Identifier: MS-PL

#include "Accelerometer.hpp"

#include "System/InvalidOperationException.hpp"

#if defined(WINDOWS_PHONE)
#include <mutex>

#include "Microsoft/Devices/Sensors/Accelerometer.hpp"
#include "Microsoft/Devices/Sensors/AccelerometerFailedException.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#endif

namespace
{
    bool isInitialized = false;
    bool isActive = false;

#if defined(WINDOWS_PHONE)
    Microsoft::Devices::Sensors::Accelerometer accelerometer;
    std::mutex threadLock;
    Microsoft::Xna::Framework::Vector3 nextValue;
#endif
}

namespace Platformer
{
    AccelerometerState::AccelerometerState(
        const Microsoft::Xna::Framework::Vector3 acceleration, const bool isActive)
        : acceleration_(acceleration), isActive_(isActive)
    {
    }

    Microsoft::Xna::Framework::Vector3 AccelerometerState::getAccelerationProperty() const
    {
        return acceleration_;
    }

    bool AccelerometerState::getIsActiveProperty() const
    {
        return isActive_;
    }

    std::string AccelerometerState::ToString() const
    {
        return "Acceleration: " + acceleration_.ToString() +
               ", IsActive: " + (isActive_ ? "True" : "False");
    }

    void Accelerometer::Initialize()
    {
        if (isInitialized)
            throw System::InvalidOperationException("Initialize can only be called once");

#if defined(WINDOWS_PHONE)
#if defined(CNA_WINDOWS_PHONE_EMULATOR)
        isActive = true;
#else
        try
        {
            accelerometer.ReadingChanged += [](
                System::Object*,
                const Microsoft::Devices::Sensors::AccelerometerReadingEventArgs& e)
            {
                const std::lock_guard lock(threadLock);
                nextValue = Microsoft::Xna::Framework::Vector3(
                    static_cast<float>(e.getXProperty()),
                    static_cast<float>(e.getYProperty()),
                    static_cast<float>(e.getZProperty()));
            };
            accelerometer.Start();
            isActive = true;
        }
        catch (const Microsoft::Devices::Sensors::AccelerometerFailedException&)
        {
            isActive = false;
        }
#endif
#endif

        isInitialized = true;
    }

    AccelerometerState Accelerometer::GetState()
    {
        using Microsoft::Xna::Framework::Vector3;

        if (!isInitialized)
            throw System::InvalidOperationException(
                "You must Initialize before you can call GetState");

        Vector3 stateValue;

#if defined(WINDOWS_PHONE)
        if (isActive)
        {
#if defined(CNA_WINDOWS_PHONE_EMULATOR)
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
#else
            const std::lock_guard lock(threadLock);
            stateValue = nextValue;
#endif
        }
#endif

        return {stateValue, isActive};
    }
}
