// SPDX-License-Identifier: MS-PL

#include "GameLogic/ChaseCamera.hpp"

#include <cmath>

#include "Microsoft/Xna/Framework/MathHelper.hpp"

namespace RacingGame::GameLogic
{
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;

    float ChaseCamera::cameraWobbelTimeoutMs = 0.0f;
    float ChaseCamera::cameraWobbelFactor = 1.0f;

    void ChaseCamera::WobbelCamera(const float wobbelFactor)
    {
        cameraWobbelTimeoutMs =
            static_cast<float>(MaxCameraWobbelTimeoutMs);
        cameraWobbelFactor = wobbelFactor;
    }

    ChaseCamera::ChaseCamera(
        CarPhysicsEnvironment& setEnvironment, const Vector3 setCarPosition,
        const Vector3 setDirection, const Vector3 setUp,
        const Vector3 setCameraPos)
        : CarPhysics(setEnvironment, setCarPosition, setDirection, setUp),
          freeCameraRot(MathHelper::Pi, 0.0f, -MathHelper::Pi / 2.0f)
    {
        SetCameraPosition(setCameraPos);
    }

    ChaseCamera::ChaseCamera(
        CarPhysicsEnvironment& setEnvironment, const Vector3 setCarPosition,
        const Vector3 setCameraPos)
        : CarPhysics(setEnvironment, setCarPosition),
          freeCameraRot(MathHelper::Pi, 0.0f, -MathHelper::Pi / 2.0f)
    {
        SetCameraPosition(setCameraPos);
    }

    ChaseCamera::ChaseCamera(
        CarPhysicsEnvironment& setEnvironment, const Vector3 setCarPosition)
        : CarPhysics(setEnvironment, setCarPosition),
          freeCameraRot(MathHelper::Pi, 0.0f, -MathHelper::Pi / 2.0f)
    {
        SetCameraPosition(
            setCarPosition + Vector3(0.0f, 10.0f, 25.0f));
    }

    Vector3 ChaseCamera::getCameraPositionProperty() const
    {
        return cameraPos;
    }

    Vector3 ChaseCamera::getXAxisProperty() const
    {
        const Matrix view = GetCarEnvironment().GetViewMatrix();
        return {view.M11, view.M21, view.M31};
    }

    Vector3 ChaseCamera::getYAxisProperty() const
    {
        const Matrix view = GetCarEnvironment().GetViewMatrix();
        return {view.M12, view.M22, view.M32};
    }

    Vector3 ChaseCamera::getZAxisProperty() const
    {
        const Matrix view = GetCarEnvironment().GetViewMatrix();
        return {view.M13, view.M23, view.M33};
    }

    Matrix ChaseCamera::getRotationMatrixProperty() const
    {
        return rotMatrix;
    }

    bool ChaseCamera::getFreeCameraProperty() const
    {
        return cameraMode == CameraMode::FreeCamera;
    }

    void ChaseCamera::setFreeCameraProperty(const bool value)
    {
        cameraMode = value ? CameraMode::FreeCamera : CameraMode::Default;
    }

    void ChaseCamera::SetCameraPosition(const Vector3 setCameraPos)
    {
        cameraPos = setCameraPos;
        cameraDistance = Vector3::Distance(
            getLookAtPosProperty(), cameraPos);
        cameraLookVector = getLookAtPosProperty() - cameraPos;
        wannaCameraDistance = cameraDistance;
        wannaCameraLookVector = cameraLookVector;
        rotMatrix = Matrix::CreateLookAt(
            cameraPos, getLookAtPosProperty(), getCarUpVectorProperty());
    }

    void ChaseCamera::InterpolateCameraPosition(
        const Vector3 setInterpolatedCameraPos)
    {
        if (getFreeCameraProperty())
            return;

        if (wannaCameraDistance == 0.0f)
            SetCameraPosition(setInterpolatedCameraPos);

        wannaCameraDistance = Vector3::Distance(
            getLookAtPosProperty(), setInterpolatedCameraPos);
        wannaCameraLookVector =
            getLookAtPosProperty() - setInterpolatedCameraPos;
    }

    void ChaseCamera::HandleFreeCamera(const CarControlState& input)
    {
        if (cameraMode != CameraMode::FreeCamera)
            return;

        constexpr float rotationFactor = 0.0075f;
        const float gamePadRotFactor =
            5.0f * GetCarEnvironment().GetMoveFactorPerSecond();
        cameraDistance = cameraLookVector.Length();

        if (wannaHaveCameraRotation.Equals(Vector3::Zero))
            wannaHaveCameraRotation = freeCameraRot;
        const Vector3 rot = wannaHaveCameraRotation;

        float addRotX = -input.mouseXMovement * rotationFactor +
                        input.gamePadLeftStickX * gamePadRotFactor;
        if (addRotX == 0.0f)
        {
            if (input.gamePadDPadLeft || input.keyboardLeftPressed)
                addRotX = -gamePadRotFactor;
            if (input.gamePadDPadRight || input.keyboardRightPressed)
                addRotX = gamePadRotFactor;
        }

        float addRotY = -input.mouseYMovement * rotationFactor +
                        input.gamePadLeftStickY * gamePadRotFactor;
        if (addRotY == 0.0f)
        {
            if (input.gamePadDPadUp || input.keyboardUpPressed)
                addRotY = -gamePadRotFactor;
            if (input.gamePadDPadDown || input.keyboardDownPressed)
                addRotY = gamePadRotFactor;
        }

        wannaHaveCameraRotation = Vector3(
            rot.X, rot.Y + addRotY, rot.Z + addRotX);
        freeCameraRot = Vector3::Lerp(
            freeCameraRot, wannaHaveCameraRotation, 0.5f);

        constexpr float epsilon = 0.000001f;
        const float minRotationRange = epsilon;
        const float maxRotationRange = MathHelper::Pi - epsilon;
        if (freeCameraRot.X < minRotationRange)
            freeCameraRot.X = minRotationRange;
        else if (freeCameraRot.X > maxRotationRange)
            freeCameraRot.X = maxRotationRange;

        cameraLookVector = Vector3(0.0f, 0.0f, cameraDistance);
        cameraLookVector = Vector3::TransformNormal(
            cameraLookVector,
            Matrix::CreateRotationX(freeCameraRot.X) *
                Matrix::CreateRotationY(freeCameraRot.Y) *
                Matrix::CreateRotationZ(freeCameraRot.Z));

        const float moveFactor =
            (input.keyLeftShift ? 20.0f : 40.0f) *
            GetCarEnvironment().GetMoveFactorPerSecond();
        const float smallMoveFactor = moveFactor / 4.0f;
        float lookDistanceChange = 0.0f;
        if (input.keyPageUp)
            lookDistanceChange += moveFactor * 0.05f;
        if (input.keyPageDown)
            lookDistanceChange -= moveFactor * 0.05f;
        if (input.keyHome)
            lookDistanceChange += smallMoveFactor * 0.05f;
        if (input.keyEnd)
            lookDistanceChange -= smallMoveFactor * 0.05f;
        if (input.mouseWheelDelta != 0)
        {
            lookDistanceChange = static_cast<float>(input.mouseWheelDelta) *
                GetCarEnvironment().GetMoveFactorPerSecond() / 16.0f;
        }
        if (input.gamePadRightStickY != 0.0f)
        {
            lookDistanceChange = input.gamePadRightStickY *
                GetCarEnvironment().GetMoveFactorPerSecond();
        }

        if (lookDistanceChange != 0.0f)
        {
            if (input.keyLeftShift)
                lookDistanceChange /= 2.0f;
            cameraDistance *= 1.0f - lookDistanceChange;
            if (cameraDistance < 1.0f)
                cameraDistance = 1.0f;
            cameraLookVector = Vector3::TransformNormal(
                Vector3(0.0f, 0.0f, cameraDistance),
                Matrix::CreateRotationX(freeCameraRot.X) *
                    Matrix::CreateRotationY(freeCameraRot.Y) *
                    Matrix::CreateRotationZ(freeCameraRot.Z));
        }

        wannaCameraDistance = cameraDistance;
        wannaCameraLookVector = cameraLookVector;
    }

    void ChaseCamera::UpdateViewMatrix()
    {
        cameraDistance = cameraDistance * 0.9f +
                         wannaCameraDistance * 0.1f;
        cameraLookVector = cameraLookVector * 0.9f +
                           wannaCameraLookVector * 0.1f;
        cameraPos = getLookAtPosProperty() + cameraLookVector;
        rotMatrix = Matrix::CreateLookAt(
            cameraPos, getLookAtPosProperty(), getCarUpVectorProperty());

        if (cameraWobbelTimeoutMs > 0.0f)
        {
            cameraWobbelTimeoutMs -= GetCarEnvironment().GetElapsedMilliseconds();
            if (cameraWobbelTimeoutMs < 0.0f)
                cameraWobbelTimeoutMs = 0.0f;
        }

        if (cameraWobbelTimeoutMs > 0.0f &&
            getZoomInTimeProperty() <= StartGameZoomTimeMilliseconds)
        {
            const float effectStrength = 1.5f * cameraWobbelFactor *
                (cameraWobbelTimeoutMs /
                 static_cast<float>(MaxCameraWobbelTimeoutMs));
            lastCameraWobble = lastCameraWobble * 0.9f +
                GetCarEnvironment().GetRandomVector3(
                    -effectStrength, effectStrength) * 0.1f;
            rotMatrix *= Matrix::CreateTranslation(lastCameraWobble);
        }

        GetCarEnvironment().SetViewMatrix(rotMatrix);
    }

    void ChaseCamera::Reset()
    {
        CarPhysics::Reset();
        cameraWobbelFactor = 0.0f;
    }

    void ChaseCamera::ClearVariablesForGameOver()
    {
        CarPhysics::ClearVariablesForGameOver();
        cameraWobbelFactor = 0.0f;
    }

    void ChaseCamera::Update(const CarControlState& input)
    {
        CarPhysics::Update(input);
        HandleFreeCamera(input);
        UpdateViewMatrix();
    }

    bool ChaseCamera::IsFreeCameraActive() const
    {
        return getFreeCameraProperty();
    }

    void ChaseCamera::SetCameraPositionFromPhysics(const Vector3 position)
    {
        SetCameraPosition(position);
    }

    void ChaseCamera::InterpolateCameraPositionFromPhysics(
        const Vector3 position)
    {
        InterpolateCameraPosition(position);
    }

    void ChaseCamera::StartCameraWobble(const float factor)
    {
        WobbelCamera(factor);
    }
}
