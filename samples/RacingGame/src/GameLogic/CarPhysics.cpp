// SPDX-License-Identifier: MS-PL

#include "GameLogic/CarPhysics.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "Helpers/Vector3Helper.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"

namespace RacingGame::GameLogic
{
    using Helpers::Vector3Helper;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;

    float CarPhysics::maxSpeed = DefaultMaxSpeed * 1.05f;
    float CarPhysics::carMass = DefaultCarMass * 1.015f;
    float CarPhysics::maxAccelerationPerSec =
        DefaultMaxAccelerationPerSec * 0.85f;
    Physics::SpringPhysicsObject CarPhysics::carPitchPhysics(
        DefaultCarMass, 1.5f, 120.0f, 0.0f);

    void CarPhysics::SetCarVariablesForCarType(
        const float setMaxCarSpeed, const float setCarMass,
        const float setMaxAccelerationPerSec)
    {
        maxSpeed = setMaxCarSpeed;
        carMass = setCarMass;
        maxAccelerationPerSec = setMaxAccelerationPerSec;
        carPitchPhysics = Physics::SpringPhysicsObject(
            carMass, 1.5f, 120.0f, 0.0f);
    }

    CarPhysics::CarPhysics(
        CarPhysicsEnvironment& setEnvironment, const Vector3 setCarPosition)
        : BasePlayer(setEnvironment), carEnvironment(setEnvironment)
    {
        SetCarPosition(setCarPosition, Vector3(0.0f, 1.0f, 0.0f),
                       Vector3(0.0f, 0.0f, 1.0f));
    }

    CarPhysics::CarPhysics(
        CarPhysicsEnvironment& setEnvironment, const Vector3 setCarPosition,
        const Vector3 setDirection, const Vector3 setUp)
        : BasePlayer(setEnvironment), carEnvironment(setEnvironment)
    {
        SetCarPosition(setCarPosition, setDirection, setUp);
    }

    void CarPhysics::SetCarPosition(
        const Vector3 position, const Vector3 direction, const Vector3 up)
    {
        carPos = position;
        carDir = direction;
        carUp = up;
    }

    void CarPhysics::Reset()
    {
        BasePlayer::Reset();
        speed = 0.0f;
        carForce = Vector3::Zero;
        trackSegmentNumber = 0;
        trackSegmentPercent = 0.0f;
    }

    void CarPhysics::ClearVariablesForGameOver()
    {
        BasePlayer::ClearVariablesForGameOver();
        speed = 0.0f;
        carForce = Vector3::Zero;
        trackSegmentNumber = 0;
        trackSegmentPercent = 0.0f;
    }

    CarPhysicsEnvironment& CarPhysics::GetCarEnvironment() const
    {
        return carEnvironment;
    }

    bool CarPhysics::IsFreeCameraActive() const
    {
        return carEnvironment.IsFreeCamera();
    }

    void CarPhysics::SetCameraPositionFromPhysics(const Vector3 position)
    {
        carEnvironment.SetCameraPosition(position);
    }

    void CarPhysics::InterpolateCameraPositionFromPhysics(
        const Vector3 position)
    {
        carEnvironment.InterpolateCameraPosition(position);
    }

    void CarPhysics::StartCameraWobble(const float factor)
    {
        carEnvironment.WobbelCamera(factor);
    }

    Vector3 CarPhysics::getCarPositionProperty() const
    {
        return carPos;
    }

    float CarPhysics::getSpeedProperty() const
    {
        return speed;
    }

    float CarPhysics::getAccelerationProperty()
    {
        lastAccelerationResult += Vector3::Dot(carForce, carDir) * 0.01f *
                                  carEnvironment.GetMoveFactorPerSecond();
        if (lastAccelerationResult < -0.25f)
            lastAccelerationResult = -0.25f;
        if (lastAccelerationResult > 1.0f)
            lastAccelerationResult = 1.0f;

        const int thisGear = 1 + static_cast<int>(
            5.0f * getSpeedProperty() / MaxPossibleSpeed);
        if (thisGear != lastGear)
        {
            lastAccelerationResult = 0.0f;
            lastGear = thisGear;
        }
        return lastAccelerationResult;
    }

    Vector3 CarPhysics::getLookAtPosProperty() const
    {
        return carPos + carUp * CarHeight;
    }

    Vector3 CarPhysics::getCarDirectionProperty() const
    {
        return carDir;
    }

    float CarPhysics::getCarWheelPosProperty() const
    {
        return wheelPos;
    }

    Vector3 CarPhysics::getCarRightProperty() const
    {
        return Vector3::Cross(carDir, carUp);
    }

    Vector3 CarPhysics::getCarUpVectorProperty() const
    {
        return carUp;
    }

    Matrix CarPhysics::getCarRenderMatrixProperty() const
    {
        return carRenderMatrix;
    }

    bool CarPhysics::getIsCarOnGroundProperty() const
    {
        return isCarOnGround;
    }

    int CarPhysics::getTrackSegmentNumberProperty() const
    {
        return trackSegmentNumber;
    }

    float CarPhysics::getTrackSegmentPercentProperty() const
    {
        return trackSegmentPercent;
    }

    BrakeSoundType CarPhysics::GetBrakeSoundType(
        const float speedChange) const
    {
        const float moveFactor = carEnvironment.GetMoveFactorPerSecond();
        const bool inRotation = rotationChange >
            0.25f * MaxRotationPerSec * moveFactor;
        BrakeSoundType result = inRotation
            ? BrakeSoundType::BrakeCurveMinor
            : BrakeSoundType::BrakeMinor;
        if (speed > 1.5f && std::abs(speedChange) > 5.0f * moveFactor)
        {
            result = inRotation
                ? BrakeSoundType::BrakeCurveMajor
                : BrakeSoundType::BrakeMajor;
        }
        return result;
    }

    void CarPhysics::Update(const CarControlState& input)
    {
        BasePlayer::Update();
        if (IsFreeCameraActive())
            return;

        if (getZoomInTimeProperty() > 0.0f)
            isCarOnGround = false;

        wheelPos += carEnvironment.GetMoveFactorPerSecond() * speed /
                    WheelMovementSpeed;

        float moveFactor = carEnvironment.GetMoveFactorPerSecond();
        if (moveFactor < 0.001f)
            moveFactor = 0.001f;
        if (moveFactor > 0.5f)
            moveFactor = 0.5f;

        const float effectiveSensitivity =
            MinSensitivity + carEnvironment.GetControllerSensitivity();
        rotationChange *= 0.95f;
        if (input.keyboardLeftPressed || input.keyA)
        {
            rotationChange += effectiveSensitivity * MaxRotationPerSec *
                              moveFactor / 2.5f;
        }
        else if (input.keyboardRightPressed || input.keyD || input.keyE)
        {
            rotationChange -= effectiveSensitivity * MaxRotationPerSec *
                              moveFactor / 2.5f;
        }
        else
        {
            rotationChange = 0.0f;
        }

        if (input.mouseXMovement != 0.0f)
        {
            rotationChange -= effectiveSensitivity *
                              (input.mouseXMovement / 15.0f) *
                              MaxRotationPerSec * moveFactor;
        }
        if (input.gamePadConnected)
        {
            rotationChange -= effectiveSensitivity * input.gamePadLeftStickX *
                              MaxRotationPerSec * moveFactor / 1.12345f;
            if (input.gamePadDPadLeft)
            {
                rotationChange += effectiveSensitivity * MaxRotationPerSec *
                                  moveFactor / 1.5f;
            }
            else if (input.gamePadDPadRight)
            {
                rotationChange -= effectiveSensitivity * MaxRotationPerSec *
                                  moveFactor / 1.5f;
            }
        }

        const float maxRot = MaxRotationPerSec * moveFactor * 1.25f;
        if (rotateCarAfterCollision != 0.0f)
        {
            if (rotateCarAfterCollision > maxRot)
            {
                rotationChange += maxRot;
                rotateCarAfterCollision -= maxRot;
            }
            else if (rotateCarAfterCollision < -maxRot)
            {
                rotationChange -= maxRot;
                rotateCarAfterCollision += maxRot;
            }
            else
            {
                rotationChange += rotateCarAfterCollision;
                rotateCarAfterCollision = 0.0f;
            }
        }
        else if (speed < 10.0f)
        {
            rotationChange *= 0.67f + 0.33f * speed / 10.0f;
        }
        else
        {
            rotationChange *= 1.0f + (speed - 10.0f) / 100.0f;
        }

        if (rotationChange > maxRot)
            rotationChange = maxRot;
        if (rotationChange < -maxRot)
            rotationChange = -maxRot;

        virtualRotationAmount += rotationChange;
        const float interpolatedRotationChange =
            (rotationChange + virtualRotationAmount) * moveFactor / 0.225f;
        virtualRotationAmount -= interpolatedRotationChange;
        if (isCarOnGround)
        {
            carDir = Vector3::TransformNormal(
                carDir, Matrix::CreateFromAxisAngle(
                    carUp, interpolatedRotationChange));
        }

        if (input.keyPageUp || input.gamePadX)
            viewDistance -= moveFactor * 2.0f;
        if (input.keyPageDown || input.gamePadY)
            viewDistance += moveFactor * 2.0f;
        if (input.mouseWheelDelta != 0)
            viewDistance -= static_cast<float>(input.mouseWheelDelta) / 500.0f;
        if (getZoomInTimeProperty() <= 0.0f)
        {
            viewDistance = MathHelper::Clamp(
                viewDistance, MinViewDistance, MaxViewDistance);
        }
        else
        {
            viewDistance = std::max(viewDistance, MinViewDistance);
        }

        float newAccelerationForce = 0.0f;
        if (input.keyboardUpPressed || input.keyW ||
            input.mouseLeftButtonPressed || input.gamePadA)
        {
            newAccelerationForce += maxAccelerationPerSec;
        }
        else if (input.keyboardDownPressed || input.keyS || input.keyO ||
                 input.mouseRightButtonPressed)
        {
            newAccelerationForce -= maxAccelerationPerSec;
        }
        else if (input.gamePadConnected)
        {
            newAccelerationForce += input.gamePadRightTrigger *
                                    maxAccelerationPerSec;
            if (input.gamePadDPadUp)
                newAccelerationForce += maxAccelerationPerSec;
            else if (input.gamePadDPadDown)
                newAccelerationForce -= maxAccelerationPerSec;
        }

        if (speed > 0.0f && newAccelerationForce > MaxAcceleration)
            newAccelerationForce = MaxAcceleration;
        if (newAccelerationForce < MinAcceleration)
            newAccelerationForce = MinAcceleration;
        if (isCarOnGround)
        {
            carForce += carDir * newAccelerationForce * (moveFactor * 85.0f);
        }

        const float oldSpeed = speed;
        const Vector3 speedChangeVector = carForce / carMass;
        if (isCarOnGround && speedChangeVector.Length() > 0.0f)
        {
            float speedApplyFactor = Vector3::Dot(
                Vector3::Normalize(speedChangeVector), carDir);
            if (speedApplyFactor > 1.0f)
                speedApplyFactor = 1.0f;
            speed += speedChangeVector.Length() * speedApplyFactor;
        }

        float airFriction = AirFrictionPerSpeed * std::abs(speed);
        if (airFriction > MaxAirFriction)
            airFriction = MaxAirFriction;
        float groundFriction = CarFrictionOnRoad;
        if (!isCarOnGround)
            groundFriction = 0.0f;

        carForce *= 1.0f - (0.275f * 0.02125f * 0.2f *
                            (groundFriction + airFriction));
        const float noFrictionSpeed = speed;
        speed *= 1.0f - (0.01f * 0.1f * 0.02125f *
                         (groundFriction + airFriction));
        if (speed < noFrictionSpeed - 1.0f)
            speed = noFrictionSpeed - 1.0f;

        if (isCarOnGround)
        {
            bool downPressed = input.mouseRightButtonPressed ||
                               input.keyboardDownPressed ||
                               input.gamePadDPadDown;
            if (input.keySpace || input.mouseMiddleButtonPressed ||
                input.gamePadLeftTrigger > 0.5f || input.gamePadB ||
                downPressed)
            {
                const float slowdown = 1.0f - moveFactor *
                    (downPressed ? BrakeSlowdown / 2.0f : BrakeSlowdown) *
                    (speed < 0.0f ? 0.33f : 1.0f);
                speed *= std::max(0.0f, slowdown);
                if (speed > oldSpeed + 100.0f * moveFactor)
                    speed = oldSpeed + 100.0f * moveFactor;
                if (speed < oldSpeed - 100.0f * moveFactor)
                    speed = oldSpeed - 100.0f * moveFactor;
                downPressed = true;
            }

            float speedChange = speed - oldSpeed;
            if ((speed > 0.5f && speed < 7.5f &&
                 speedChange > 5.5f * moveFactor) ||
                (speed > 0.75f && speedChange < 10.0f * moveFactor &&
                 downPressed))
            {
                const BrakeSoundType brakeType = GetBrakeSoundType(speedChange);
                if (brakeType == BrakeSoundType::BrakeCurveMajor ||
                    brakeType == BrakeSoundType::BrakeMajor)
                {
                    carEnvironment.AddBrakeTrack(*this);
                }
                carEnvironment.PlayBrakeSound(brakeType);
            }

            if (speedChange < -8.0f * moveFactor)
                speedChange = -8.0f * moveFactor;
            if (speedChange > 8.0f * moveFactor)
                speedChange = 8.0f * moveFactor;
            carPitchPhysics.ChangePos(speedChange);
        }

        if (speed > maxSpeed)
            speed = maxSpeed;
        if (speed < -maxSpeed)
            speed = -maxSpeed;
        carPos += speed * carDir * moveFactor * 1.75f;
        carPitchPhysics.Simulate(moveFactor);

        const int oldTrackSegmentNumber = trackSegmentNumber;
        carEnvironment.UpdateCarTrackPosition(
            carPos, trackSegmentNumber, trackSegmentPercent);
        if (trackSegmentNumber != oldTrackSegmentNumber &&
            carEnvironment.IsInGame() && !getGameOverProperty())
        {
            if (trackSegmentNumber == 0 &&
                carEnvironment.GetCheckpointTimeCount() >=
                    carEnvironment.GetCheckpointSegmentCount() - 1)
            {
                carEnvironment.AddTimeFadeupEffect(
                    static_cast<int>(getGameTimeMillisecondsProperty()),
                    TimeFadeupMode::Normal);
                StartNewLap();
            }
            else
            {
                const int checkpoint =
                    carEnvironment.GetCheckpointTimeCount();
                if (getZoomInTimeProperty() <= 0.0f &&
                    checkpoint < carEnvironment.GetCheckpointSegmentCount() &&
                    carEnvironment.GetCheckpointSegment(checkpoint) >
                        oldTrackSegmentNumber &&
                    carEnvironment.GetCheckpointSegment(checkpoint) <=
                        trackSegmentNumber)
                {
                    const int differenceMs =
                        carEnvironment.CompareCheckpointTime(checkpoint);
                    carEnvironment.PlayCheckpointSound(
                        differenceMs < 0
                            ? CheckpointSoundType::Better
                            : CheckpointSoundType::Worse);
                    carEnvironment.AddTimeFadeupEffect(
                        std::abs(differenceMs), differenceMs < 0
                            ? TimeFadeupMode::Minus
                            : TimeFadeupMode::Plus);
                    carEnvironment.AddCheckpointTime(
                        getGameTimeMillisecondsProperty() / 1000.0f);
                }
            }
        }

        float roadWidth = 0.0f;
        float nextRoadWidth = 0.0f;
        const Matrix trackMatrix = carEnvironment.GetTrackPositionMatrix(
            trackSegmentNumber, trackSegmentPercent,
            roadWidth, nextRoadWidth);
        const Vector3 rememberedRight = getCarRightProperty();
        carUp = trackMatrix.getUpProperty();
        carDir = Vector3::Cross(carUp, rememberedRight);
        const Vector3 trackPos = trackMatrix.getTranslationProperty();
        SetGroundPlaneAndGuardRails(
            trackPos, trackMatrix.getUpProperty(),
            trackPos - trackMatrix.getRightProperty() *
                (roadWidth / 2.0f - GuardRailInsideRoadDistance / 2.0f),
            trackPos - trackMatrix.getRightProperty() *
                (roadWidth / 2.0f - GuardRailInsideRoadDistance / 2.0f) +
                trackMatrix.getForwardProperty(),
            trackPos + trackMatrix.getRightProperty() *
                (nextRoadWidth / 2.0f - GuardRailInsideRoadDistance / 2.0f),
            trackPos + trackMatrix.getRightProperty() *
                (nextRoadWidth / 2.0f - GuardRailInsideRoadDistance / 2.0f) +
                trackMatrix.getForwardProperty());
        carRenderMatrix = UpdateCarMatrixAndCamera();
        ApplyGravityAndCheckForCollisions();
    }

    void CarPhysics::SetGroundPlaneAndGuardRails(
        const Vector3 setGroundPlanePos,
        const Vector3 setGroundPlaneNormal,
        const Vector3 setGuardrailLeft,
        const Vector3 setNextGuardrailLeft,
        const Vector3 setGuardrailRight,
        const Vector3 setNextGuardrailRight)
    {
        groundPlanePos = setGroundPlanePos;
        groundPlaneNormal = setGroundPlaneNormal;
        guardrailLeft = setGuardrailLeft;
        nextGuardrailLeft = setNextGuardrailLeft;
        guardrailRight = setGuardrailRight;
        nextGuardrailRight = setNextGuardrailRight;
    }

    void CarPhysics::ApplyGravityAndCheckForCollisions()
    {
        if (carEnvironment.IsInMenu())
            return;

        const Vector3 guardrailLeftVec = Vector3::Normalize(
            nextGuardrailLeft - guardrailLeft);
        const Vector3 guardrailRightVec = Vector3::Normalize(
            nextGuardrailRight - guardrailRight);
        const Vector3 guardrailLeftNormal = Vector3::Cross(
            guardrailLeftVec, groundPlaneNormal);
        const Vector3 guardrailRightNormal = Vector3::Cross(
            groundPlaneNormal, guardrailRightVec);
        const float roadWidth = (guardrailLeft - guardrailRight).Length();
        const float moveFactor = carEnvironment.GetMoveFactorPerSecond();
        const Vector3 pos = carPos;
        const Vector3 carRight = Vector3::Cross(carDir, carUp);
        const Vector3 carLeft = -carRight;
        const std::array carCorners{
            pos + carDir * (5.6f / 2.0f) - carRight * (2.6f / 2.0f),
            pos + carDir * (5.6f / 2.0f) + carRight * (2.6f / 2.0f),
            pos - carDir * (5.6f / 2.0f) + carRight * (2.6f / 2.0f),
            pos - carDir * (5.6f / 2.0f) - carRight * (2.6f / 2.0f),
        };

        [[maybe_unused]] float applyGravity = 0.0f;
        for (std::size_t index = 0; index < carCorners.size(); ++index)
        {
            if (carCorners[index].Z > groundPlanePos.Z)
                applyGravity += Gravity / 4.0f;

            const float leftDist = Vector3Helper::DistanceToLine(
                carCorners[index], guardrailLeft, nextGuardrailLeft);
            const float rightDist = Vector3Helper::DistanceToLine(
                carCorners[index], guardrailRight, nextGuardrailRight);

            if (leftDist < 0.1f || rightDist > roadWidth)
            {
                float collisionAngle = Vector3Helper::GetAngleBetweenVectors(
                    carRight, guardrailLeftNormal);
                if (collisionAngle > MathHelper::Pi / 2.0f)
                    collisionAngle -= MathHelper::Pi;
                if (std::abs(collisionAngle) < MathHelper::Pi / 4.0f)
                {
                    carEnvironment.PlayCrashSound(false);
                    if (index < 2)
                    {
                        rotateCarAfterCollision = -collisionAngle / 1.5f;
                        speed *= 0.93f;
                        if (viewDistance > 0.75f)
                            viewDistance -= 0.1f;
                    }
                    else
                    {
                        rotateCarAfterCollision = -collisionAngle / 2.5f;
                        speed *= 0.96f;
                        if (viewDistance > 0.75f)
                            viewDistance -= 0.05f;
                    }
                    StartCameraWobble(0.00075f * speed);
                }
                else if (std::abs(collisionAngle) <
                         MathHelper::Pi * 3.0f / 4.0f)
                {
                    if (std::abs(collisionAngle) < MathHelper::Pi / 3.0f)
                        rotateCarAfterCollision = collisionAngle / 3.0f;
                    carEnvironment.PlayCrashSound(true);
                    StartCameraWobble(0.005f * speed);
                    speed = 0.0f;
                }
                carForce = Vector3::Zero;
                const float speedDistanceToGuardrails = speed * std::abs(
                    Vector3::Dot(carDir, guardrailLeftNormal));
                if (leftDist > 0.0f)
                {
                    const float correction = leftDist + 0.01f +
                        0.1f * speedDistanceToGuardrails * moveFactor;
                    carPos += correction * guardrailLeftNormal;
                }
            }

            if (rightDist < 0.1f || leftDist > roadWidth)
            {
                float collisionAngle = Vector3Helper::GetAngleBetweenVectors(
                    carLeft, guardrailRightNormal);
                if (collisionAngle > MathHelper::Pi / 2.0f)
                    collisionAngle -= MathHelper::Pi;
                if (std::abs(collisionAngle) < MathHelper::Pi / 4.0f)
                {
                    carEnvironment.PlayCrashSound(false);
                    if (index < 2)
                    {
                        rotateCarAfterCollision = collisionAngle / 1.5f;
                        speed *= 0.935f;
                        if (viewDistance > 0.75f)
                            viewDistance -= 0.1f;
                    }
                    else
                    {
                        rotateCarAfterCollision = collisionAngle / 2.5f;
                        speed *= 0.96f;
                        if (viewDistance > 0.75f)
                            viewDistance -= 0.05f;
                    }
                    StartCameraWobble(0.00075f * speed);
                }
                else if (std::abs(collisionAngle) <
                         MathHelper::Pi * 3.0f / 4.0f)
                {
                    if (std::abs(collisionAngle) < MathHelper::Pi / 3.0f)
                        rotateCarAfterCollision = collisionAngle / 3.0f;
                    carEnvironment.PlayCrashSound(true);
                    StartCameraWobble(0.005f * speed);
                    speed = 0.0f;
                }
                carForce = Vector3::Zero;
                const float speedDistanceToGuardrails = speed * std::abs(
                    Vector3::Dot(carDir, guardrailLeftNormal));
                if (rightDist > 0.0f)
                {
                    const float correction = rightDist + 0.01f +
                        0.1f * speedDistanceToGuardrails * moveFactor;
                    carPos += correction * guardrailRightNormal;
                }
            }
        }
        ApplyGravity();
    }

    void CarPhysics::ApplyGravity()
    {
        const float moveFactor = carEnvironment.GetMoveFactorPerSecond();
        float distFromGround = Vector3Helper::SignedDistanceToPlane(
            carPos, groundPlanePos - Vector3(0.0f, 0.0f, 0.15f),
            groundPlaneNormal);
        isCarOnGround = distFromGround > -0.5f;
        const float maxGravity = Gravity * moveFactor;
        const float minGravity = -Gravity * moveFactor;
        if (distFromGround > maxGravity)
        {
            distFromGround = maxGravity;
            gravitySpeed = 0.0f;
        }
        if (distFromGround < minGravity)
        {
            distFromGround = minGravity;
            gravitySpeed -= distFromGround;
        }
        carPos.Z += distFromGround;

        const bool upsideDown = carUp.Z < 0.05f;
        const bool movingUp = carDir.Z > 0.65f;
        const bool movingDown = carDir.Z < -0.65f;
        if (upsideDown || movingUp || movingDown)
            carPos.Z = groundPlanePos.Z;
    }

    Matrix CarPhysics::UpdateCarMatrixAndCamera()
    {
        Matrix carMatrix = Matrix::getIdentityProperty();
        carMatrix.setRightProperty(getCarRightProperty());
        carMatrix.setUpProperty(carUp);
        carMatrix.setForwardProperty(carDir);
        carMatrix.setTranslationProperty(carPos);

        const float chaseCamDistance =
            (4.25f + 9.75f * speed / maxSpeed) * viewDistance;
        if (!carEnvironment.IsInMenu() && getZoomInTimeProperty() > 1500.0f)
        {
            const float zoomFraction = std::max(
                getZoomInTimeProperty() -
                    static_cast<float>(StartGameZoomedInTime),
                0.0f) / static_cast<float>(StartGameZoomTimeMilliseconds);
            const Vector3 cameraPosition = carPos + carUp * CarHeight +
                carMatrix.getForwardProperty() *
                    (chaseCamDistance + zoomFraction * 250.0f) -
                carMatrix.getUpProperty() *
                    (0.6f + zoomFraction * 200.0f);
            if (getZoomInTimeProperty() -
                    carEnvironment.GetElapsedMilliseconds() >= 3000.0f)
            {
                SetCameraPositionFromPhysics(cameraPosition);
            }
            else
            {
                InterpolateCameraPositionFromPhysics(cameraPosition);
            }
        }
        else if (IsFreeCameraActive())
        {
            InterpolateCameraPositionFromPhysics(
                carPos + carUp * CarHeight +
                carMatrix.getForwardProperty() * chaseCamDistance -
                carMatrix.getUpProperty() *
                    (chaseCamDistance / (viewDistance + 6.0f)) -
                carMatrix.getUpProperty());
        }
        else if (carEnvironment.IsInMenu() &&
                 carEnvironment.GetTotalTimeMilliseconds() < 100.0f)
        {
            SetCameraPositionFromPhysics(
                carPos + carUp * CarHeight +
                carMatrix.getForwardProperty() * chaseCamDistance -
                carMatrix.getUpProperty() * 0.6f);
        }
        else
        {
            InterpolateCameraPositionFromPhysics(
                carPos + carMatrix.getUpProperty() * CarHeight +
                carMatrix.getForwardProperty() *
                    (chaseCamDistance / 1.125f) -
                carMatrix.getUpProperty() * 0.8f);
        }

        if (getGameTimeMillisecondsProperty() >
            carEnvironment.GetReplayMatrixCount() *
                ReplayTrackMatrixIntervals * 1000.0f)
        {
            carEnvironment.AddReplayCarMatrix(carMatrix);
        }

        carMatrix = Matrix::CreateRotationX(
                        MathHelper::Pi / 2.0f - carPitchPhysics.pos / 60.0f) *
                    Matrix::CreateRotationZ(MathHelper::Pi) * carMatrix;
        return carMatrix;
    }
}
