// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SampleCamera.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Quaternion.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace PerPixelLightingSample
{
    using namespace Microsoft::Xna::Framework;

    /** @brief The two orbit behaviors SampleArcBallCamera offers. */
    enum class SampleArcBallCameraMode
    {
        /** @brief A totally free-look arcball that orbits relative to its orientation. */
        Free = 0,

        /** @brief A camera constrained by roll so that orbits only occur on latitude and longitude. */
        RollConstrained = 1
    };

    /**
     * @brief An example arc ball camera.
     */
    class SampleArcBallCamera
    {
    public:
        /**
         * @brief Uses a pair of keys to simulate a positive or negative axis input.
         *
         * @param keyState The keyboard state to read.
         * @param downKey The key that contributes -1.
         * @param upKey The key that contributes +1.
         * @return The resulting axis value in [-1, 1].
         */
        static float ReadKeyboardAxis(const Input::KeyboardState& keyState, Input::Keys downKey,
                                      Input::Keys upKey);

        /**
         * @brief Create an arcball camera that allows free orbit around a target point.
         *
         * @param controlMode The orbit behavior the camera follows.
         */
        explicit SampleArcBallCamera(SampleArcBallCameraMode controlMode);

        /**
         * @brief Get the forward direction vector of the camera.
         * @return The normalized forward direction.
         */
        [[nodiscard]] Vector3 getDirectionProperty() const;

        /**
         * @brief Get the right direction vector of the camera.
         * @return The right direction.
         */
        [[nodiscard]] Vector3 getRightProperty() const;

        /**
         * @brief Get the up direction vector of the camera.
         * @return The up direction.
         */
        [[nodiscard]] Vector3 getUpProperty() const;

        /**
         * @brief Get the View (look at) Matrix defined by the camera.
         * @return The view matrix.
         */
        [[nodiscard]] Matrix getViewMatrixProperty() const;

        /**
         * @brief Get the camera's orbit behavior.
         * @return The active control mode.
         */
        [[nodiscard]] SampleArcBallCameraMode getControlModeProperty() const;

        /**
         * @brief Set the camera's orbit behavior, re-deriving the gimbals when it changes.
         * @param value The control mode to adopt.
         */
        void setControlModeProperty(SampleArcBallCameraMode value);

        /**
         * @brief Get the current target of the camera.
         * @return The look-at target.
         */
        [[nodiscard]] Vector3 getTargetProperty() const;

        /**
         * @brief Set the current target of the camera.
         * @param value The look-at target.
         */
        void setTargetProperty(Vector3 value);

        /**
         * @brief Get the camera's distance to the target.
         * @return The distance.
         */
        [[nodiscard]] float getDistanceProperty() const;

        /**
         * @brief Set the camera's distance to the target.
         * @param value The distance.
         */
        void setDistanceProperty(float value);

        /**
         * @brief Get the rate of distance change when automatically handling input.
         * @return The distance rate.
         */
        [[nodiscard]] float getInputDistanceRateProperty() const;

        /**
         * @brief Set the rate of distance change when automatically handling input.
         * @param value The distance rate.
         */
        void setInputDistanceRateProperty(float value);

        /**
         * @brief Get the camera's current position.
         * @return The position in world space.
         */
        [[nodiscard]] Vector3 getPositionProperty() const;

        /**
         * @brief Set the camera's current position, keeping the existing target.
         * @param value The position in world space.
         */
        void setPositionProperty(Vector3 value);

        /**
         * @brief Orbit directly upwards in Free camera, or on the longitude line when roll constrained.
         * @param angle The orbit angle in radians.
         */
        void OrbitUp(float angle);

        /**
         * @brief Orbit towards the Right vector in Free camera, or on the latitude line when roll constrained.
         * @param angle The orbit angle in radians.
         */
        void OrbitRight(float angle);

        /**
         * @brief Rotate around the Forward vector, in free-look camera only.
         * @param angle The roll angle in radians.
         */
        void RotateClockwise(float angle);

        /**
         * @brief Sets up a quaternion and position from vector camera components, oriented up.
         *
         * @param position The camera position.
         * @param target The camera's look-at point.
         * @param up The up direction the look-at matrix is built with.
         */
        void SetCamera(Vector3 position, Vector3 target, Vector3 up);

        /**
         * @brief Handle default keyboard input for a camera.
         *
         * @param kbState The keyboard state to read.
         * @param gameTime Provides a snapshot of timing values.
         */
        void HandleDefaultKeyboardControls(const Input::KeyboardState& kbState,
                                           const GameTime& gameTime);

        /**
         * @brief Handle default gamepad input for a camera.
         *
         * @param gpState The gamepad state to read.
         * @param gameTime Provides a snapshot of timing values.
         */
        void HandleDefaultGamepadControls(const Input::GamePadState& gpState,
                                          const GameTime& gameTime);

        /** @brief Reset the camera to the defaults set in the constructor. */
        void Reset();

    private:
        // The location of the look-at target.
        Vector3 targetValue;

        // The distance between the camera and the target.
        float distanceValue = 0.0f;

        // The orientation of the camera relative to the target.
        Quaternion orientation;

        float inputDistanceRateValue = 0.0f;
        static constexpr float InputTurnRate = 3.0f;
        SampleArcBallCameraMode mode = SampleArcBallCameraMode::Free;
        float yaw = 0.0f, pitch = 0.0f;
    };
}
