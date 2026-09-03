// SPDX-License-Identifier: MS-PL

#pragma once

#include <unordered_map>
#include <vector>

#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace RacingGame::GameLogic
{
    /** @brief User-adjustable behavior of the Racing touch-control layout. */
    struct MobileControlPreferences
    {
        /** @brief Mirrors steering and pedals horizontally for left-handed play. */
        bool leftHanded = false;
        /** @brief Uniform control-size multiplier. */
        float scale = 1.0f;
        /** @brief Overlay opacity in the inclusive zero-to-one range. */
        float opacity = 0.45f;
        /** @brief Steering response multiplier applied after the dead zone. */
        float steeringSensitivity = 1.0f;
        /** @brief Steering dead-zone radius as a fraction of the pad radius. */
        float steeringDeadZone = 0.08f;
        /** @brief Hides the touch overlay while a physical gamepad is connected. */
        bool hideWithGamePad = false;
        /** @brief Enables accelerometer steering when the sensor is available. */
        bool tiltEnabled = false;
        /** @brief Reverses the sign of accelerometer steering. */
        bool tiltInverted = false;
        /** @brief Accelerometer steering response multiplier. */
        float tiltSensitivity = 1.0f;
    };

    /** @brief One touch point supplied to the platform-independent control mapper. */
    struct MobileTouchPoint
    {
        /** @brief Lifecycle phase of one touch point. */
        enum class Phase
        {
            Pressed,
            Moved,
            Released,
        };

        /** @brief Stable touch identifier. */
        int id = -1;
        /** @brief Touch position in backbuffer pixels. */
        Microsoft::Xna::Framework::Vector2 position;
        /** @brief Current lifecycle phase. */
        Phase phase = Phase::Released;
    };

    /** @brief Safe-area-relative rectangles used by the Racing touch overlay. */
    struct MobileControlLayout
    {
        /** @brief Safe interactive area from which every control is derived. */
        Microsoft::Xna::Framework::Rectangle safeArea;
        /** @brief Analog steering-pad hit rectangle. */
        Microsoft::Xna::Framework::Rectangle steering;
        /** @brief Analog throttle-pedal hit rectangle. */
        Microsoft::Xna::Framework::Rectangle throttle;
        /** @brief Analog brake/reverse-pedal hit rectangle. */
        Microsoft::Xna::Framework::Rectangle brake;
        /** @brief Handbrake-button hit rectangle. */
        Microsoft::Xna::Framework::Rectangle handbrake;
        /** @brief Back/pause-button hit rectangle. */
        Microsoft::Xna::Framework::Rectangle back;
        /** @brief Camera-distance-button hit rectangle. */
        Microsoft::Xna::Framework::Rectangle camera;

        /**
         * @brief Maps a safe rectangle from window-client coordinates to backbuffer coordinates.
         * @param clientBounds Full platform window client rectangle.
         * @param clientSafeArea Safe interactive rectangle in the same client coordinates.
         * @param displayWidth Current logical backbuffer width in pixels.
         * @param displayHeight Current logical backbuffer height in pixels.
         * @return The safe rectangle in backbuffer coordinates, or an empty rectangle when either
         * input rectangle has no usable extent.
         */
        [[nodiscard]] static Microsoft::Xna::Framework::Rectangle
            MapClientSafeAreaToDisplay(
                Microsoft::Xna::Framework::Rectangle clientBounds,
                Microsoft::Xna::Framework::Rectangle clientSafeArea,
                int displayWidth, int displayHeight);

        /**
         * @brief Creates a landscape touch layout contained by the supplied safe area.
         * @param displayWidth Current backbuffer width in pixels.
         * @param displayHeight Current backbuffer height in pixels.
         * @param safeArea Platform safe area, or an empty rectangle to use the full backbuffer.
         * @param preferences User-selected size and handedness preferences.
         * @return Fully resolved control rectangles in backbuffer pixels.
         */
        [[nodiscard]] static MobileControlLayout Create(
            int displayWidth, int displayHeight,
            Microsoft::Xna::Framework::Rectangle safeArea,
            const MobileControlPreferences& preferences);
    };

    /** @brief One mapped mobile-control snapshot and its overlay state. */
    struct MobileControlState
    {
        /** @brief Resolved control geometry for this frame. */
        MobileControlLayout layout;
        /** @brief Analog steering in the inclusive range -1 to +1. */
        float steering = 0.0f;
        /** @brief Analog throttle in the inclusive range zero to one. */
        float throttle = 0.0f;
        /** @brief Analog brake/reverse in the inclusive range zero to one. */
        float brake = 0.0f;
        /** @brief True while the handbrake button is held. */
        bool handbrakePressed = false;
        /** @brief True while the back/pause button is held. */
        bool backPressed = false;
        /** @brief True only when the back/pause button is newly touched. */
        bool backJustPressed = false;
        /** @brief True while the camera-distance button is held. */
        bool cameraPressed = false;
        /** @brief True only when the camera-distance button is newly touched. */
        bool cameraJustPressed = false;
        /** @brief True while at least one driving control owns a finger. */
        bool drivingTouchActive = false;
        /** @brief True when the in-race touch overlay should be rendered. */
        bool overlayVisible = false;
        /** @brief Latest steering finger position, used to draw its thumb indicator. */
        Microsoft::Xna::Framework::Vector2 steeringPosition;
        /** @brief True when steeringPosition contains an active finger position. */
        bool hasSteeringPosition = false;
        /** @brief Clamped user-selected overlay opacity. */
        float opacity = 0.45f;
    };

    /** @brief Maps stable multi-touch contacts to Racing's mobile action model. */
    class MobileControlMapper
    {
    public:
        /**
         * @brief Maps one frame of contacts while retaining each finger's initial control role.
         * @param contacts Current touch snapshot, including one-frame release records when present.
         * @param displayWidth Current backbuffer width in pixels.
         * @param displayHeight Current backbuffer height in pixels.
         * @param safeArea Platform safe area, or an empty rectangle for the full backbuffer.
         * @param preferences User-selected touch preferences.
         * @return Mobile action and overlay state for this frame.
         */
        [[nodiscard]] MobileControlState Map(
            const std::vector<MobileTouchPoint>& contacts,
            int displayWidth, int displayHeight,
            Microsoft::Xna::Framework::Rectangle safeArea,
            const MobileControlPreferences& preferences);

        /** @brief Releases every captured finger, for focus loss or lifecycle suspension. */
        void Reset();

    private:
        enum class Role
        {
            Steering,
            Throttle,
            Brake,
            Handbrake,
            Back,
            Camera,
        };

        std::unordered_map<int, Role> capturedRoles;
    };

    /** @brief Calibrates and filters landscape accelerometer steering. */
    class TiltSteeringFilter
    {
    public:
        /** @brief Clears calibration and filtered history. */
        void Reset();

        /**
         * @brief Establishes the current landscape horizontal acceleration as neutral.
         * @param value Current horizontal acceleration in standard-gravity units.
         */
        void Calibrate(float value);

        /**
         * @brief Filters one landscape horizontal acceleration sample into steering.
         * @param value Current horizontal acceleration in standard-gravity units.
         * @param sensitivity User-selected steering response multiplier.
         * @param inverted True to reverse steering direction.
         * @return Filtered steering in the inclusive range -1 to +1.
         */
        [[nodiscard]] float Update(float value, float sensitivity, bool inverted);

        /** @brief Gets whether a neutral value has been established. */
        [[nodiscard]] bool getIsCalibratedProperty() const;

    private:
        bool calibrated = false;
        float neutral = 0.0f;
        float filtered = 0.0f;
    };
}
