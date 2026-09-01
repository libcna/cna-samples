// SPDX-License-Identifier: MS-PL
#pragma once

#include <cmath>
#include <memory>

#include "Microsoft/Devices/DeviceType.hpp"
#include "Microsoft/Devices/Environment.hpp"
#include "Microsoft/Devices/Sensors/Accelerometer.hpp"
#include "Microsoft/Devices/Sensors/AccelerometerFailedException.hpp"
#include "Microsoft/Devices/Sensors/AccelerometerReadingEventArgs.hpp"
#include "Microsoft/Xna/Framework/GameComponent.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "System/DateTimeOffset.hpp"

namespace TiltPerspectiveSample {

using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameComponent;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Devices::DeviceType;
using Microsoft::Devices::Environment;
using Microsoft::Devices::Sensors::Accelerometer;
using Microsoft::Devices::Sensors::AccelerometerFailedException;
using Microsoft::Devices::Sensors::AccelerometerReadingEventArgs;

class IAccelerometerService {
public:
    virtual ~IAccelerometerService() = default;

    [[nodiscard]] virtual Vector3 getRawAccelerationProperty() const = 0;
    [[nodiscard]] virtual Vector3 getSmoothAccelerationProperty() const = 0;
};

class AccelerometerHelper : public GameComponent, public IAccelerometerService {
public:
    explicit AccelerometerHelper(Game& game) : GameComponent(game) {
        game.getServicesProperty().AddService<IAccelerometerService>(this);
    }

    void Initialize() override {
        rawAcceleration_ = Vector3(0.0f, 0.0f, -1.0f);
        smoothAcceleration_ = Vector3(0.0f, 0.0f, -1.0f);

        sensor_ = std::make_unique<Accelerometer>();
        sensor_->ReadingChanged +=
            [this](System::Object*, const AccelerometerReadingEventArgs& args) {
                OnReadingChanged(args);
            };

        if (Environment::getDeviceTypeProperty() == DeviceType::Device) {
            try {
                sensor_->Start();
            } catch (const AccelerometerFailedException&) {
                sensor_.reset();
            }
        } else {
            sensor_.reset();
        }

        GameComponent::Initialize();
        setEnabledProperty(sensor_ == nullptr);
    }

    void Update(GameTime& gameTime) override {
        if (!sensor_) {
            fakeRollTheta_ += static_cast<float>(
                gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty()) * fakeRollSpeed_;
            fakeRollTheta_ = MathHelper::WrapAngle(fakeRollTheta_);

            rawAcceleration_ = Vector3(
                std::sin(fakeRollPhi_) * std::cos(fakeRollTheta_),
                std::sin(fakeRollPhi_) * std::sin(fakeRollTheta_),
                -std::cos(fakeRollPhi_));
            smoothAcceleration_ = rawAcceleration_;
        }

        GameComponent::Update(gameTime);
    }

    [[nodiscard]] Vector3 getRawAccelerationProperty() const override { return rawAcceleration_; }
    [[nodiscard]] Vector3 getSmoothAccelerationProperty() const override { return smoothAcceleration_; }

    float Smoothing = 0.1f;

private:
    void OnReadingChanged(const AccelerometerReadingEventArgs& args) {
        rawAcceleration_ = Vector3(
            static_cast<float>(args.getXProperty()),
            static_cast<float>(args.getYProperty()),
            static_cast<float>(args.getZProperty()));
        rawAcceleration_ -= sensorError_;

        float dt = static_cast<float>(
            args.getTimestampProperty().Subtract(lastSensorTime_).getTotalSecondsProperty());
        lastSensorTime_ = args.getTimestampProperty();
        dt = MathHelper::Clamp(dt, 0.0f, 1.0f);

        const float p = std::exp(-dt / Smoothing);
        smoothAcceleration_ = Vector3::Lerp(rawAcceleration_, smoothAcceleration_, p);
    }

    std::unique_ptr<Accelerometer> sensor_;
    System::DateTimeOffset lastSensorTime_;
    Vector3 rawAcceleration_{0.0f, 0.0f, -1.0f};
    Vector3 smoothAcceleration_{0.0f, 0.0f, -1.0f};
    Vector3 sensorError_{-0.09f, -0.02f, 0.04f};

    float fakeRollPhi_ = MathHelper::Pi / 8.0f;
    float fakeRollTheta_ = 0.0f;
    float fakeRollSpeed_ = 1.0f;
};

} // namespace TiltPerspectiveSample
