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
#include "System/Boolean.hpp"
#include "System/InvalidOperationException.hpp"

namespace AccelerometerSample {

using Microsoft::Devices::DeviceType;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Input::Keyboard;
using Microsoft::Xna::Framework::Input::Keys;

/** @brief Encapsulates the current accelerometer value and active state. */
struct AccelerometerState {
private:
    Vector3 acceleration_{};
    bool isActive_ = false;

public:
    /** @brief Creates an inactive state with a zero acceleration vector. */
    AccelerometerState() = default;

    /**
     * @brief Creates an accelerometer state.
     *
     * @param acceleration Current acceleration in G-force.
     * @param isActive Whether the accelerometer is active.
     */
    AccelerometerState(Vector3 acceleration, bool isActive)
        : acceleration_(acceleration), isActive_(isActive) {
    }

    /**
     * @brief Gets the current acceleration in G-force.
     *
     * @return Current acceleration vector.
     */
    [[nodiscard]] Vector3 getAccelerationProperty() const {
        return acceleration_;
    }

    /**
     * @brief Gets whether the accelerometer is active and running.
     *
     * @return True when the accelerometer is active.
     */
    [[nodiscard]] bool getIsActiveProperty() const {
        return isActive_;
    }

    /**
     * @brief Returns text containing the acceleration and active state.
     *
     * @return Description of this state.
     */
    [[nodiscard]] std::string ToString() const {
        return "Acceleration: " + acceleration_.ToString() +
            ", IsActive: " + System::Boolean::ToString(isActive_);
    }
};

/** @brief Provides polling-based accelerometer input for the sample. */
class Accelerometer final {
public:
    /** @brief Accelerometer is a static class and cannot be instantiated. */
    Accelerometer() = delete;

    /**
     * @brief Initializes accelerometer input for the current game.
     *
     * @throws System::InvalidOperationException If initialization was already attempted.
     */
    static void Initialize() {
        if (isInitialized_) {
            throw System::InvalidOperationException("Initialize can only be called once");
        }

        if (Microsoft::Devices::Environment::getDeviceTypeProperty() == DeviceType::Device) {
            try {
                accelerometer_.ReadingChanged += [](
                    System::Object*,
                    const Microsoft::Devices::Sensors::AccelerometerReadingEventArgs& eventArgs) {
                    std::scoped_lock lock(threadLock_);
                    nextValue_ = Vector3(
                        static_cast<float>(eventArgs.getXProperty()),
                        static_cast<float>(eventArgs.getYProperty()),
                        static_cast<float>(eventArgs.getZProperty()));
                };
                accelerometer_.Start();
                isActive_ = true;
            } catch (const Microsoft::Devices::Sensors::AccelerometerFailedException&) {
                isActive_ = false;
            }
        } else {
            isActive_ = true;
        }

        isInitialized_ = true;
    }

    /**
     * @brief Gets the current accelerometer state.
     *
     * @return Current acceleration and active state.
     * @throws System::InvalidOperationException If Initialize has not been called.
     */
    [[nodiscard]] static AccelerometerState GetState() {
        if (!isInitialized_) {
            throw System::InvalidOperationException(
                "You must Initialize before you can call GetState");
        }

        Vector3 stateValue{};
        if (isActive_) {
            if (Microsoft::Devices::Environment::getDeviceTypeProperty() == DeviceType::Device) {
                std::scoped_lock lock(threadLock_);
                stateValue = nextValue_;
            } else {
                const auto keyboardState = Keyboard::GetState();
                stateValue.Z = -1.0f;

                if (keyboardState.IsKeyDown(Keys::Left)) stateValue.X -= 1.0f;
                if (keyboardState.IsKeyDown(Keys::Right)) stateValue.X += 1.0f;
                if (keyboardState.IsKeyDown(Keys::Up)) stateValue.Y += 1.0f;
                if (keyboardState.IsKeyDown(Keys::Down)) stateValue.Y -= 1.0f;

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

} // namespace AccelerometerSample
