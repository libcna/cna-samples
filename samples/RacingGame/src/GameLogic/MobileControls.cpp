// SPDX-License-Identifier: MS-PL

#include "GameLogic/MobileControls.hpp"

#include <algorithm>
#include <cmath>
#include <optional>
#include <unordered_set>

namespace RacingGame::GameLogic
{
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Vector2;

    namespace
    {
        bool IsEmpty(const Rectangle& rectangle)
        {
            return rectangle.Width <= 0 || rectangle.Height <= 0;
        }

        bool Contains(const Rectangle& rectangle, const Vector2& position)
        {
            return position.X >= static_cast<float>(rectangle.X) &&
                   position.Y >= static_cast<float>(rectangle.Y) &&
                   position.X < static_cast<float>(rectangle.X + rectangle.Width) &&
                   position.Y < static_cast<float>(rectangle.Y + rectangle.Height);
        }

        Rectangle ClipSafeArea(
            const int displayWidth, const int displayHeight, Rectangle safeArea)
        {
            const Rectangle display(
                0, 0, std::max(1, displayWidth), std::max(1, displayHeight));
            if (IsEmpty(safeArea)) return display;

            const int left = std::clamp(safeArea.X, 0, display.Width);
            const int top = std::clamp(safeArea.Y, 0, display.Height);
            const int right = std::clamp(
                safeArea.X + safeArea.Width, left, display.Width);
            const int bottom = std::clamp(
                safeArea.Y + safeArea.Height, top, display.Height);
            if (right <= left || bottom <= top) return display;
            return Rectangle(left, top, right - left, bottom - top);
        }

        Rectangle Mirror(const Rectangle& rectangle, const Rectangle& safeArea)
        {
            return Rectangle(
                safeArea.X + safeArea.Width - (rectangle.X - safeArea.X) -
                    rectangle.Width,
                rectangle.Y, rectangle.Width, rectangle.Height);
        }

        float ApplyDeadZone(
            const float value, const float deadZone, const float sensitivity)
        {
            const float magnitude = std::abs(value);
            if (magnitude <= deadZone) return 0.0f;
            const float normalized = (magnitude - deadZone) / (1.0f - deadZone);
            return std::copysign(
                std::clamp(normalized * sensitivity, 0.0f, 1.0f), value);
        }

        float VerticalPedalAmount(
            const Rectangle& rectangle, const Vector2& position)
        {
            const float progress = std::clamp(
                (position.Y - static_cast<float>(rectangle.Y)) /
                    static_cast<float>(std::max(1, rectangle.Height)),
                0.0f, 1.0f);
            return 0.25f + progress * 0.75f;
        }
    }

    Rectangle MobileControlLayout::MapClientSafeAreaToDisplay(
        const Rectangle clientBounds, const Rectangle clientSafeArea,
        const int displayWidth, const int displayHeight)
    {
        if (IsEmpty(clientBounds) || IsEmpty(clientSafeArea) ||
            displayWidth <= 0 || displayHeight <= 0)
        {
            return Rectangle::Empty;
        }

        const auto mapX = [&](const int value)
        {
            return static_cast<int>(std::lround(
                static_cast<double>(value - clientBounds.X) *
                static_cast<double>(displayWidth) /
                static_cast<double>(clientBounds.Width)));
        };
        const auto mapY = [&](const int value)
        {
            return static_cast<int>(std::lround(
                static_cast<double>(value - clientBounds.Y) *
                static_cast<double>(displayHeight) /
                static_cast<double>(clientBounds.Height)));
        };

        const int left = mapX(clientSafeArea.X);
        const int top = mapY(clientSafeArea.Y);
        const int right = mapX(clientSafeArea.getRightProperty());
        const int bottom = mapY(clientSafeArea.getBottomProperty());
        return Rectangle(left, top, right - left, bottom - top);
    }

    MobileControlLayout MobileControlLayout::Create(
        const int displayWidth, const int displayHeight, Rectangle safeArea,
        const MobileControlPreferences& preferences)
    {
        safeArea = ClipSafeArea(displayWidth, displayHeight, safeArea);
        const float scale = std::clamp(preferences.scale, 0.75f, 1.35f);
        const int shortest = std::min(safeArea.Width, safeArea.Height);
        const int margin = std::max(8, static_cast<int>(std::lround(
            static_cast<float>(shortest) * 0.025f)));
        const int maxPad = std::max(48, safeArea.Height - margin * 2);
        const int padSize = std::clamp(
            static_cast<int>(std::lround(std::min(
                static_cast<float>(safeArea.Height) * 0.38f,
                static_cast<float>(safeArea.Width) * 0.25f) * scale)),
            std::min(96, maxPad), maxPad);
        const int gap = std::max(6, margin / 2);
        const int pedalWidth = std::max(48, static_cast<int>(std::lround(
            static_cast<float>(padSize) * 0.42f)));
        const int pedalHeight = std::max(72, static_cast<int>(std::lround(
            static_cast<float>(padSize) * 0.82f)));
        const int smallHeight = std::max(48, static_cast<int>(std::lround(
            static_cast<float>(padSize) * 0.27f)));
        const int topButton = std::clamp(
            static_cast<int>(std::lround(static_cast<float>(padSize) * 0.28f)),
            48, 128);

        MobileControlLayout result;
        result.safeArea = safeArea;
        result.steering = Rectangle(
            safeArea.X + margin,
            safeArea.Y + safeArea.Height - margin - padSize,
            padSize, padSize);
        result.throttle = Rectangle(
            safeArea.X + safeArea.Width - margin - pedalWidth,
            safeArea.Y + safeArea.Height - margin - pedalHeight,
            pedalWidth, pedalHeight);
        result.brake = Rectangle(
            result.throttle.X - gap - pedalWidth,
            result.throttle.Y, pedalWidth, pedalHeight);
        result.handbrake = Rectangle(
            result.brake.X,
            result.brake.Y - gap - smallHeight,
            pedalWidth * 2 + gap, smallHeight);
        const int topCentre = safeArea.X + safeArea.Width / 2;
        result.camera = Rectangle(
            topCentre - gap - topButton,
            safeArea.Y + margin, topButton, topButton);
        result.back = Rectangle(
            topCentre + gap,
            safeArea.Y + margin, topButton, topButton);

        if (preferences.leftHanded)
        {
            result.steering = Mirror(result.steering, safeArea);
            result.throttle = Mirror(result.throttle, safeArea);
            result.brake = Mirror(result.brake, safeArea);
            result.handbrake = Mirror(result.handbrake, safeArea);
            result.back = Mirror(result.back, safeArea);
            result.camera = Mirror(result.camera, safeArea);
        }
        return result;
    }

    MobileControlState MobileControlMapper::Map(
        const std::vector<MobileTouchPoint>& contacts,
        const int displayWidth, const int displayHeight, Rectangle safeArea,
        const MobileControlPreferences& preferences)
    {
        MobileControlState result;
        result.layout = MobileControlLayout::Create(
            displayWidth, displayHeight, safeArea, preferences);
        result.opacity = std::clamp(preferences.opacity, 0.15f, 0.85f);

        std::unordered_set<int> activeIds;
        for (const MobileTouchPoint& contact : contacts)
        {
            if (contact.phase != MobileTouchPoint::Phase::Released)
                activeIds.insert(contact.id);
        }
        for (auto iterator = capturedRoles.begin();
             iterator != capturedRoles.end();)
        {
            if (!activeIds.contains(iterator->first))
                iterator = capturedRoles.erase(iterator);
            else
                ++iterator;
        }

        const auto findRole = [&](const Vector2& position)
            -> std::optional<Role>
        {
            if (Contains(result.layout.back, position)) return Role::Back;
            if (Contains(result.layout.camera, position)) return Role::Camera;
            if (Contains(result.layout.steering, position)) return Role::Steering;
            if (Contains(result.layout.throttle, position)) return Role::Throttle;
            if (Contains(result.layout.brake, position)) return Role::Brake;
            if (Contains(result.layout.handbrake, position)) return Role::Handbrake;
            return std::nullopt;
        };

        for (const MobileTouchPoint& contact : contacts)
        {
            if (contact.phase == MobileTouchPoint::Phase::Released)
            {
                capturedRoles.erase(contact.id);
                continue;
            }

            auto captured = capturedRoles.find(contact.id);
            if (captured == capturedRoles.end())
            {
                const std::optional<Role> role = findRole(contact.position);
                if (!role) continue;
                captured = capturedRoles.emplace(contact.id, *role).first;
            }

            const bool justPressed =
                contact.phase == MobileTouchPoint::Phase::Pressed;
            switch (captured->second)
            {
            case Role::Steering:
            {
                const float centre = static_cast<float>(result.layout.steering.X) +
                    static_cast<float>(result.layout.steering.Width) * 0.5f;
                const float radius = static_cast<float>(
                    std::max(1, result.layout.steering.Width)) * 0.5f;
                const float raw = std::clamp(
                    (contact.position.X - centre) / radius, -1.0f, 1.0f);
                result.steering = ApplyDeadZone(
                    raw, std::clamp(preferences.steeringDeadZone, 0.0f, 0.45f),
                    std::clamp(preferences.steeringSensitivity, 0.25f, 2.0f));
                result.steeringPosition = contact.position;
                result.hasSteeringPosition = true;
                result.drivingTouchActive = true;
                break;
            }
            case Role::Throttle:
                result.throttle = std::max(
                    result.throttle,
                    VerticalPedalAmount(result.layout.throttle, contact.position));
                result.drivingTouchActive = true;
                break;
            case Role::Brake:
                result.brake = std::max(
                    result.brake,
                    VerticalPedalAmount(result.layout.brake, contact.position));
                result.drivingTouchActive = true;
                break;
            case Role::Handbrake:
                result.handbrakePressed = true;
                result.drivingTouchActive = true;
                break;
            case Role::Back:
                result.backPressed = true;
                result.backJustPressed = result.backJustPressed || justPressed;
                break;
            case Role::Camera:
                result.cameraPressed = true;
                result.cameraJustPressed = result.cameraJustPressed || justPressed;
                break;
            }
        }
        return result;
    }

    void MobileControlMapper::Reset()
    {
        capturedRoles.clear();
    }

    void TiltSteeringFilter::Reset()
    {
        calibrated = false;
        neutral = 0.0f;
        filtered = 0.0f;
    }

    void TiltSteeringFilter::Calibrate(const float value)
    {
        calibrated = true;
        neutral = value;
        filtered = 0.0f;
    }

    float TiltSteeringFilter::Update(
        const float value, const float sensitivity, const bool inverted)
    {
        if (!calibrated) Calibrate(value);
        const float signedValue = (value - neutral) * (inverted ? -1.0f : 1.0f);
        filtered += (signedValue - filtered) * 0.18f;
        return ApplyDeadZone(
            std::clamp(filtered / 0.45f, -1.0f, 1.0f), 0.06f,
            std::clamp(sensitivity, 0.25f, 2.0f));
    }

    bool TiltSteeringFilter::getIsCalibratedProperty() const
    {
        return calibrated;
    }
}
