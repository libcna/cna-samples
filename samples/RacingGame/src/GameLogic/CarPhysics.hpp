// SPDX-License-Identifier: MS-PL

#pragma once

#include <cstddef>

#include "GameLogic/BasePlayer.hpp"
#include "GameLogic/CarControlState.hpp"
#include "GameLogic/Physics/SpringPhysicsObject.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace RacingGame::GameLogic
{
    class CarPhysics;

    /** @brief Brake-sound choices made by the original car controller. */
    enum class BrakeSoundType
    {
        BrakeCurveMajor,
        BrakeCurveMinor,
        BrakeMajor,
        BrakeMinor,
    };

    /** @brief Time-overlay modes emitted by checkpoint and lap logic. */
    enum class TimeFadeupMode
    {
        Normal,
        Minus,
        Plus,
    };

    /** @brief Checkpoint comparison sounds selected by race timing. */
    enum class CheckpointSoundType
    {
        /** @brief The player reached the checkpoint sooner than the best replay. */
        Better,
        /** @brief The player reached the checkpoint no sooner than the best replay. */
        Worse,
    };

    /** @brief Supplies game-owned track, camera, replay, sound and UI services. */
    class CarPhysicsEnvironment : public BasePlayerEnvironment
    {
    public:
        /** @brief Gets the fixed-step movement factor in seconds. */
        [[nodiscard]] virtual float GetMoveFactorPerSecond() const = 0;
        /** @brief Gets total game time in milliseconds. */
        [[nodiscard]] virtual float GetTotalTimeMilliseconds() const = 0;
        /** @brief Gets the configured controller sensitivity addition. */
        [[nodiscard]] virtual float GetControllerSensitivity() const = 0;
        /** @brief Gets whether the chase camera is in free-camera mode. */
        [[nodiscard]] virtual bool IsFreeCamera() const = 0;
        /** @brief Gets whether active race gameplay is running. */
        [[nodiscard]] virtual bool IsInGame() const = 0;

        /** @brief Gets the current game view matrix. */
        [[nodiscard]] virtual Microsoft::Xna::Framework::Matrix
        GetViewMatrix() const = 0;
        /** @brief Replaces the current game view matrix. */
        virtual void SetViewMatrix(
            Microsoft::Xna::Framework::Matrix matrix) = 0;
        /** @brief Gets a random vector whose components are in the requested range. */
        [[nodiscard]] virtual Microsoft::Xna::Framework::Vector3
        GetRandomVector3(float minimum, float maximum) = 0;

        /** @brief Locates a car inside the active track. */
        virtual void UpdateCarTrackPosition(
            Microsoft::Xna::Framework::Vector3 carPosition,
            int& segment, float& segmentPercent) = 0;
        /** @brief Gets the active track coordinate matrix. */
        [[nodiscard]] virtual Microsoft::Xna::Framework::Matrix
        GetTrackPositionMatrix(int segment, float segmentPercent,
                               float& roadWidth, float& nextRoadWidth) = 0;

        /** @brief Gets the number of checkpoint times already recorded. */
        [[nodiscard]] virtual int GetCheckpointTimeCount() const = 0;
        /** @brief Gets the number of checkpoints on the active track. */
        [[nodiscard]] virtual int GetCheckpointSegmentCount() const = 0;
        /** @brief Gets a checkpoint's track segment. */
        [[nodiscard]] virtual int GetCheckpointSegment(int index) const = 0;
        /** @brief Compares the current time at a checkpoint with the best replay. */
        [[nodiscard]] virtual int CompareCheckpointTime(int index) = 0;
        /** @brief Records a checkpoint time in seconds. */
        virtual void AddCheckpointTime(float seconds) = 0;
        /** @brief Emits a checkpoint or lap time overlay. */
        virtual void AddTimeFadeupEffect(int milliseconds,
                                         TimeFadeupMode mode) = 0;
        /** @brief Plays the checkpoint comparison sound selected by timing. */
        virtual void PlayCheckpointSound(CheckpointSoundType type) = 0;

        /** @brief Adds wheel marks for a major brake event. */
        virtual void AddBrakeTrack(const CarPhysics& car) = 0;
        /** @brief Plays the selected brake sound. */
        virtual void PlayBrakeSound(BrakeSoundType type) = 0;
        /** @brief Plays a minor or total crash sound. */
        virtual void PlayCrashSound(bool totalCrash) = 0;
        /** @brief Starts camera wobble with the requested factor. */
        virtual void WobbelCamera(float factor) = 0;

        /** @brief Replaces the camera position without interpolation. */
        virtual void SetCameraPosition(
            Microsoft::Xna::Framework::Vector3 position) = 0;
        /** @brief Interpolates the camera toward a target position. */
        virtual void InterpolateCameraPosition(
            Microsoft::Xna::Framework::Vector3 position) = 0;
        /** @brief Gets the number of matrices already stored in the replay. */
        [[nodiscard]] virtual int GetReplayMatrixCount() const = 0;
        /** @brief Appends a car transform to the current replay. */
        virtual void AddReplayCarMatrix(
            Microsoft::Xna::Framework::Matrix matrix) = 0;
    };

    /** @brief Implements the original Racing car controller and collision model. */
    class CarPhysics : public BasePlayer
    {
    private:
#if defined(RACING_GAME_TURBO)
        static constexpr float PerformanceMultiplier = 2.0f;
#else
        static constexpr float PerformanceMultiplier = 1.0f;
#endif

    public:
        /** @brief Default car mass in kilograms. */
        static constexpr float DefaultCarMass = 1000.0f;
        /** @brief Miles-per-hour conversion for meters per second. */
        static constexpr float MeterPerSecToMph =
            1.609344f * ((60.0f * 60.0f) / 1000.0f);
        /** @brief Meters-per-second conversion for miles per hour. */
        static constexpr float MphToMeterPerSec = 1.0f / MeterPerSecToMph;
        /** @brief Default maximum vehicle speed. */
        static constexpr float DefaultMaxSpeed =
            275.0f * MphToMeterPerSec * PerformanceMultiplier;
        /** @brief Absolute speed used by the gear display. */
        static constexpr float MaxPossibleSpeed =
            290.0f * MphToMeterPerSec * PerformanceMultiplier;
        /** @brief Default rate at which acceleration force changes. */
        static constexpr float DefaultMaxAccelerationPerSec =
            2.5f * PerformanceMultiplier;
        /** @brief Maximum forward acceleration force. */
        static constexpr float MaxAcceleration =
            5.75f * PerformanceMultiplier;
        /** @brief Maximum reverse acceleration force. */
        static constexpr float MinAcceleration = -3.25f;
        /** @brief Maximum steering rotation per second. */
        static constexpr float MaxRotationPerSec = 1.3f;
        /** @brief Minimum controller sensitivity. */
        static constexpr float MinSensitivity = 0.5f;

        /** @brief Applies the selected car model's physical constants. */
        static void SetCarVariablesForCarType(
            float setMaxCarSpeed, float setCarMass,
            float setMaxAccelerationPerSec);

        /** @brief Creates a controller at a position with default orientation. */
        CarPhysics(CarPhysicsEnvironment& setEnvironment,
                   Microsoft::Xna::Framework::Vector3 setCarPosition);
        /** @brief Creates a controller with an explicit position and orientation. */
        CarPhysics(CarPhysicsEnvironment& setEnvironment,
                   Microsoft::Xna::Framework::Vector3 setCarPosition,
                   Microsoft::Xna::Framework::Vector3 setDirection,
                   Microsoft::Xna::Framework::Vector3 setUp);

        /** @brief Repositions and reorients the car. */
        void SetCarPosition(Microsoft::Xna::Framework::Vector3 position,
                            Microsoft::Xna::Framework::Vector3 direction,
                            Microsoft::Xna::Framework::Vector3 up);
        /** @brief Resets the controller for a new race. */
        void Reset() override;
        /** @brief Stops and clears the car after game over. */
        void ClearVariablesForGameOver() override;
        /** @brief Advances the complete car controller by one logical input snapshot. */
        virtual void Update(const CarControlState& input);
        /** @brief Applies current ground gravity and guard-rail collisions. */
        void ApplyGravityAndCheckForCollisions();
        /** @brief Sets current ground and guard-rail geometry. */
        void SetGroundPlaneAndGuardRails(
            Microsoft::Xna::Framework::Vector3 setGroundPlanePos,
            Microsoft::Xna::Framework::Vector3 setGroundPlaneNormal,
            Microsoft::Xna::Framework::Vector3 setGuardrailLeft,
            Microsoft::Xna::Framework::Vector3 setNextGuardrailLeft,
            Microsoft::Xna::Framework::Vector3 setGuardrailRight,
            Microsoft::Xna::Framework::Vector3 setNextGuardrailRight);
        /** @brief Builds the car transform and updates the chase-camera target. */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix
        UpdateCarMatrixAndCamera();

        /** @brief Gets the car position. */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getCarPositionProperty() const;
        /** @brief Gets the scalar forward speed. */
        [[nodiscard]] float getSpeedProperty() const;
        /** @brief Gets the smoothed engine acceleration value. */
        [[nodiscard]] float getAccelerationProperty();
        /** @brief Gets the elevated camera look-at point. */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getLookAtPosProperty() const;
        /** @brief Gets the current car direction. */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getCarDirectionProperty() const;
        /** @brief Gets the accumulated wheel animation position. */
        [[nodiscard]] float getCarWheelPosProperty() const;
        /** @brief Gets the current car right vector. */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getCarRightProperty() const;
        /** @brief Gets the current car up vector. */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getCarUpVectorProperty() const;
        /** @brief Gets the final model rendering matrix. */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix
        getCarRenderMatrixProperty() const;
        /** @brief Gets whether the car touched the ground during the last update. */
        [[nodiscard]] bool getIsCarOnGroundProperty() const;
        /** @brief Gets the current track segment. */
        [[nodiscard]] int getTrackSegmentNumberProperty() const;
        /** @brief Gets the fraction traveled inside the current track segment. */
        [[nodiscard]] float getTrackSegmentPercentProperty() const;

    protected:
        static constexpr float CarHeight = 2.0f;
        static float maxSpeed;
        static float carMass;
        static float maxAccelerationPerSec;
        bool isCarOnGround = false;
        Microsoft::Xna::Framework::Vector3 groundPlanePos;
        Microsoft::Xna::Framework::Vector3 groundPlaneNormal;
        Microsoft::Xna::Framework::Vector3 guardrailLeft;
        Microsoft::Xna::Framework::Vector3 nextGuardrailLeft;
        Microsoft::Xna::Framework::Vector3 guardrailRight;
        Microsoft::Xna::Framework::Vector3 nextGuardrailRight;

        /** @brief Gets the environment shared by the controller and chase camera. */
        [[nodiscard]] CarPhysicsEnvironment& GetCarEnvironment() const;
        /** @brief Gets the free-camera state used to suspend car control. */
        [[nodiscard]] virtual bool IsFreeCameraActive() const;
        /** @brief Handles an immediate camera-position request from car physics. */
        virtual void SetCameraPositionFromPhysics(
            Microsoft::Xna::Framework::Vector3 position);
        /** @brief Handles an interpolated camera-position request from car physics. */
        virtual void InterpolateCameraPositionFromPhysics(
            Microsoft::Xna::Framework::Vector3 position);
        /** @brief Starts the collision camera wobble. */
        virtual void StartCameraWobble(float factor);

    private:
        static constexpr float Gravity = 9.81f;
        static constexpr float CarFrictionOnRoad = 17.523456789f;
        static constexpr float AirFrictionPerSpeed = 0.66f;
        static constexpr float MaxAirFriction = AirFrictionPerSpeed * 200.0f;
        static constexpr float BrakeSlowdown = 1.0f;
        static constexpr float MinViewDistance = 0.4f;
        static constexpr float MaxViewDistance = 1.8f;
        static constexpr float WheelMovementSpeed = 1.0f;
        static constexpr float GuardRailInsideRoadDistance = 0.5f;
        static constexpr float ReplayTrackMatrixIntervals = 0.2f;

        void ApplyGravity();
        [[nodiscard]] BrakeSoundType GetBrakeSoundType(
            float speedChange) const;

        CarPhysicsEnvironment& carEnvironment;
        static Physics::SpringPhysicsObject carPitchPhysics;

        Microsoft::Xna::Framework::Vector3 carPos;
        Microsoft::Xna::Framework::Vector3 carDir;
        float speed = 0.0f;
        Microsoft::Xna::Framework::Vector3 carUp;
        Microsoft::Xna::Framework::Vector3 carForce;
        float viewDistance = 1.0f;
        float wheelPos = 0.0f;
        float rotateCarAfterCollision = 0.0f;
        int trackSegmentNumber = 0;
        float trackSegmentPercent = 0.0f;
        Microsoft::Xna::Framework::Matrix carRenderMatrix =
            Microsoft::Xna::Framework::Matrix::getIdentityProperty();
        float lastAccelerationResult = 0.0f;
        int lastGear = 0;
        float virtualRotationAmount = 0.0f;
        float rotationChange = 0.0f;
        float gravitySpeed = 0.0f;
    };
}
