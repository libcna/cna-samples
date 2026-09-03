// SPDX-License-Identifier: MS-PL

#include "GameLogic/MobileInput.hpp"

#include <algorithm>
#include <cmath>
#include <exception>
#include <utility>
#include <vector>

#include "Microsoft/Devices/Sensors/Accelerometer.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocation.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocationState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"

namespace RacingGame::GameLogic
{
    using Microsoft::Devices::Sensors::Accelerometer;
    using Microsoft::Xna::Framework::Point;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Input::Touch::TouchLocation;
    using Microsoft::Xna::Framework::Input::Touch::TouchLocationState;
    using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

    namespace
    {
        MobileTouchPoint::Phase ToMobilePhase(const TouchLocationState state)
        {
            switch (state)
            {
            case TouchLocationState::Pressed:
                return MobileTouchPoint::Phase::Pressed;
            case TouchLocationState::Moved:
                return MobileTouchPoint::Phase::Moved;
            case TouchLocationState::Released:
            case TouchLocationState::Invalid:
                return MobileTouchPoint::Phase::Released;
            }
            return MobileTouchPoint::Phase::Released;
        }

        Point ToPoint(const TouchLocation& touch)
        {
            const auto& position = touch.getPositionProperty();
            return Point(
                static_cast<int>(std::lround(position.X)),
                static_cast<int>(std::lround(position.Y)));
        }
    }

    MobileInput::MobileInput() = default;

    MobileInput::MobileInput(MobileControlPreferences setPreferences)
        : preferences(std::move(setPreferences))
    {
    }

    MobileInput::~MobileInput()
    {
        StopTiltSensor();
    }

    void MobileInput::SetSafeArea(const Rectangle setSafeArea)
    {
        safeArea = setSafeArea;
    }

    void MobileInput::setPreferencesProperty(
        const MobileControlPreferences& setPreferences)
    {
        if (preferences.tiltEnabled && !setPreferences.tiltEnabled)
            StopTiltSensor();
        preferences = setPreferences;
        if (!preferences.tiltEnabled)
            tiltFilter.Reset();
    }

    const MobileControlPreferences& MobileInput::getPreferencesProperty() const
    {
        return preferences;
    }

    void MobileInput::EnsureTiltSensor()
    {
        if (!preferences.tiltEnabled || tiltStartAttempted) return;
        tiltStartAttempted = true;
        try
        {
            if (!Accelerometer::getIsSupportedProperty()) return;
            accelerometer = std::make_unique<Accelerometer>();
            accelerometer->Start();
        }
        catch (const std::exception&)
        {
            accelerometer.reset();
        }
    }

    void MobileInput::StopTiltSensor()
    {
        if (accelerometer)
        {
            try
            {
                accelerometer->Stop();
            }
            catch (const std::exception&)
            {
            }
            accelerometer.reset();
        }
        tiltStartAttempted = false;
        tiltFilter.Reset();
    }

    ControlFrame MobileInput::Capture(
        const bool inGame, const bool appActive,
        const int displayWidth, const int displayHeight)
    {
        ControlFrame result = desktopInput.Capture(
            inGame, appActive, displayWidth, displayHeight);
        if (!appActive)
        {
            mapper.Reset();
            tiltFilter.Reset();
            result = ControlFrame{};
            return result;
        }

        const auto touchState = TouchPanel::GetState();
        std::vector<MobileTouchPoint> contacts;
        contacts.reserve(static_cast<std::size_t>(touchState.getCountProperty()));
        for (const TouchLocation& touch : touchState)
        {
            contacts.push_back({
                touch.getIdProperty(), touch.getPositionProperty(),
                ToMobilePhase(touch.getStateProperty())});
        }

        if (!inGame)
        {
            mapper.Reset();
            result.mobile = mapper.Map(
                {}, displayWidth, displayHeight, safeArea, preferences);
            const TouchLocation* pointer = nullptr;
            for (const TouchLocation& touch : touchState)
            {
                if (touch.getStateProperty() == TouchLocationState::Pressed)
                {
                    pointer = &touch;
                    break;
                }
                if (pointer == nullptr &&
                    touch.getStateProperty() == TouchLocationState::Moved)
                    pointer = &touch;
            }
            if (pointer != nullptr)
            {
                result.mousePosition = ToPoint(*pointer);
                result.hasMouseMoved = true;
                result.mouseLeftPressed = true;
                result.mouseLeftJustPressed =
                    result.mouseLeftJustPressed ||
                    pointer->getStateProperty() == TouchLocationState::Pressed;
            }
            return result;
        }

        result.mobile = mapper.Map(
            contacts, displayWidth, displayHeight, safeArea, preferences);
        result.mobile.overlayVisible = touchState.getIsConnectedProperty() &&
            (!preferences.hideWithGamePad || !result.car.gamePadConnected);

        float steering = result.mobile.steering;
        if (preferences.tiltEnabled && !result.mobile.hasSteeringPosition)
        {
            EnsureTiltSensor();
            if (accelerometer && accelerometer->getIsDataValidProperty())
            {
                steering = tiltFilter.Update(
                    accelerometer->getCurrentValueProperty()
                        .getAccelerationProperty().Y,
                    preferences.tiltSensitivity,
                    preferences.tiltInverted);
            }
        }

        result.car.mobileSteering = steering;
        result.car.mobileThrottle = result.mobile.throttle;
        result.car.mobileBrake = result.mobile.brake;
        result.car.mobileHandbrakePressed =
            result.mobile.handbrakePressed;
        result.car.mobileCameraPressed = result.mobile.cameraPressed;
        result.backJustPressed =
            result.backJustPressed || result.mobile.backJustPressed;
        return result;
    }
}
