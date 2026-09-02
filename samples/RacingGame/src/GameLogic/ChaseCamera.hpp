// SPDX-License-Identifier: MS-PL

#pragma once

#include "GameLogic/CarPhysics.hpp"

namespace RacingGame::GameLogic
{
    /** @brief Smoothly follows the player car and provides the original free-camera mode. */
    class ChaseCamera : public CarPhysics
    {
    public:
        /** @brief Camera behaviors supported by the original game. */
        enum class CameraMode
        {
            /** @brief Automatically follows the controlled car. */
            Default,
            /** @brief Allows input-driven orbiting and zooming around the car. */
            FreeCamera,
        };

        /** @brief Maximum collision-wobble duration in milliseconds. */
        static constexpr int MaxCameraWobbelTimeoutMs = 700;

        /**
         * @brief Starts a collision camera wobble that fades over time.
         * @param wobbelFactor Strength of the wobble.
         */
        static void WobbelCamera(float wobbelFactor);

        /**
         * @brief Creates a chase camera with explicit car and camera orientation.
         * @param setEnvironment Game-owned services used by physics and the camera.
         * @param setCarPosition Initial car position.
         * @param setDirection Initial car direction.
         * @param setUp Initial car up vector.
         * @param setCameraPos Initial camera position.
         */
        ChaseCamera(CarPhysicsEnvironment& setEnvironment,
                    Microsoft::Xna::Framework::Vector3 setCarPosition,
                    Microsoft::Xna::Framework::Vector3 setDirection,
                    Microsoft::Xna::Framework::Vector3 setUp,
                    Microsoft::Xna::Framework::Vector3 setCameraPos);
        /**
         * @brief Creates a chase camera with an explicit initial camera position.
         * @param setEnvironment Game-owned services used by physics and the camera.
         * @param setCarPosition Initial car position.
         * @param setCameraPos Initial camera position.
         */
        ChaseCamera(CarPhysicsEnvironment& setEnvironment,
                    Microsoft::Xna::Framework::Vector3 setCarPosition,
                    Microsoft::Xna::Framework::Vector3 setCameraPos);
        /**
         * @brief Creates a chase camera at the original default offset.
         * @param setEnvironment Game-owned services used by physics and the camera.
         * @param setCarPosition Initial car position.
         */
        ChaseCamera(CarPhysicsEnvironment& setEnvironment,
                    Microsoft::Xna::Framework::Vector3 setCarPosition);

        /** @brief Gets the current camera position. */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getCameraPositionProperty() const;
        /** @brief Gets the current view-space x axis. */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getXAxisProperty() const;
        /** @brief Gets the current view-space y axis. */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getYAxisProperty() const;
        /** @brief Gets the current view-space z axis. */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getZAxisProperty() const;
        /** @brief Gets the current camera rotation matrix. */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix
        getRotationMatrixProperty() const;
        /** @brief Gets whether input-driven free-camera mode is active. */
        [[nodiscard]] bool getFreeCameraProperty() const;
        /**
         * @brief Enables or disables input-driven free-camera mode.
         * @param value True to use free-camera mode; false for automatic chase mode.
         */
        void setFreeCameraProperty(bool value);

        /**
         * @brief Replaces the camera position and its chase interpolation targets.
         * @param setCameraPos New camera position.
         */
        void SetCameraPosition(
            Microsoft::Xna::Framework::Vector3 setCameraPos);
        /**
         * @brief Changes the desired camera position used by chase interpolation.
         * @param setInterpolatedCameraPos New desired camera position.
         */
        void InterpolateCameraPosition(
            Microsoft::Xna::Framework::Vector3 setInterpolatedCameraPos);
        /** @brief Resets player state and collision-wobble strength. */
        void Reset() override;
        /** @brief Clears game-over state and collision-wobble strength. */
        void ClearVariablesForGameOver() override;
        /**
         * @brief Advances car physics, free-camera input and the view matrix.
         * @param input Logical controls captured for this frame.
         */
        void Update(const CarControlState& input) override;

    protected:
        Microsoft::Xna::Framework::Vector3 cameraPos;

        /** @brief Gets this camera's free-camera state for car physics. */
        [[nodiscard]] bool IsFreeCameraActive() const override;
        /** @brief Applies car physics' immediate camera request to this camera. */
        void SetCameraPositionFromPhysics(
            Microsoft::Xna::Framework::Vector3 position) override;
        /** @brief Applies car physics' interpolated camera request to this camera. */
        void InterpolateCameraPositionFromPhysics(
            Microsoft::Xna::Framework::Vector3 position) override;
        /** @brief Applies collision wobble directly to the chase camera. */
        void StartCameraWobble(float factor) override;

    private:
        void HandleFreeCamera(const CarControlState& input);
        void UpdateViewMatrix();

        float cameraDistance = 0.0f;
        Microsoft::Xna::Framework::Vector3 cameraLookVector;
        CameraMode cameraMode = CameraMode::Default;
        Microsoft::Xna::Framework::Matrix rotMatrix =
            Microsoft::Xna::Framework::Matrix::getIdentityProperty();

        static float cameraWobbelTimeoutMs;
        static float cameraWobbelFactor;

        Microsoft::Xna::Framework::Vector3 wannaCameraLookVector =
            Microsoft::Xna::Framework::Vector3::Zero;
        float wannaCameraDistance = 0.0f;
        Microsoft::Xna::Framework::Vector3 freeCameraRot;
        Microsoft::Xna::Framework::Vector3 wannaHaveCameraRotation =
            Microsoft::Xna::Framework::Vector3::Zero;
        Microsoft::Xna::Framework::Vector3 lastCameraWobble =
            Microsoft::Xna::Framework::Vector3::Zero;
    };
}
