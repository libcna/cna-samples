// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SampleCamera.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "SampleCamera.hpp"

#include <cmath>

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "System/Math.hpp"

namespace VertexLightingSample
{
    using namespace Microsoft::Xna::Framework::Input;

    float SampleArcBallCamera::ReadKeyboardAxis(const KeyboardState& keyState, Keys downKey,
                                                Keys upKey)
    {
        float value = 0;

        if (keyState.IsKeyDown(downKey))
            value -= 1.0f;

        if (keyState.IsKeyDown(upKey))
            value += 1.0f;

        return value;
    }

    SampleArcBallCamera::SampleArcBallCamera(SampleArcBallCameraMode controlMode)
    {
        // orientation quaternion assumes a Pi rotation so you're facing the "front"
        // of the model (looking down the +Z axis)
        orientation = Quaternion::CreateFromAxisAngle(Vector3::Up, MathHelper::Pi);

        mode = controlMode;
        inputDistanceRateValue = 4.0f;
        yaw = MathHelper::Pi;
        pitch = 0;
    }

    Vector3 SampleArcBallCamera::getDirectionProperty() const
    {
        // R v R' where v = (0,0,-1,0)
        // The equation can be reduced because we know the following things:
        //   1.  We're using unit quaternions
        //   2.  The initial aspect does not change
        // The reduced form of the same equation follows
        Vector3 dir = Vector3::Zero;
        dir.X = -2.0f *
            ((orientation.X * orientation.Z) + (orientation.W * orientation.Y));
        dir.Y = 2.0f *
            ((orientation.W * orientation.X) - (orientation.Y * orientation.Z));
        dir.Z =
            ((orientation.X * orientation.X) + (orientation.Y * orientation.Y)) -
            ((orientation.Z * orientation.Z) + (orientation.W * orientation.W));
        Vector3::Normalize(dir, dir);
        return dir;
    }

    Vector3 SampleArcBallCamera::getRightProperty() const
    {
        // R v R' where v = (1,0,0,0), reduced the same way as Direction.
        Vector3 right = Vector3::Zero;
        right.X =
            ((orientation.X * orientation.X) + (orientation.W * orientation.W)) -
            ((orientation.Z * orientation.Z) + (orientation.Y * orientation.Y));
        right.Y = 2.0f *
            ((orientation.X * orientation.Y) + (orientation.Z * orientation.W));
        right.Z = 2.0f *
            ((orientation.X * orientation.Z) - (orientation.Y * orientation.W));

        return right;
    }

    Vector3 SampleArcBallCamera::getUpProperty() const
    {
        // R v R' where v = (0,1,0,0), reduced the same way as Direction.
        Vector3 up = Vector3::Zero;
        up.X = 2.0f *
            ((orientation.X * orientation.Y) - (orientation.Z * orientation.W));
        up.Y =
            ((orientation.Y * orientation.Y) + (orientation.W * orientation.W)) -
            ((orientation.Z * orientation.Z) + (orientation.X * orientation.X));
        up.Z = 2.0f *
            ((orientation.Y * orientation.Z) + (orientation.X * orientation.W));
        return up;
    }

    Matrix SampleArcBallCamera::getViewMatrixProperty() const
    {
        return Matrix::CreateLookAt(targetValue -
            (getDirectionProperty() * distanceValue), targetValue, getUpProperty());
    }

    SampleArcBallCameraMode SampleArcBallCamera::getControlModeProperty() const
    {
        return mode;
    }

    void SampleArcBallCamera::setControlModeProperty(SampleArcBallCameraMode value)
    {
        if (value != mode)
        {
            mode = value;
            SetCamera(targetValue - (getDirectionProperty() * distanceValue),
                      targetValue, Vector3::Up);
        }
    }

    Vector3 SampleArcBallCamera::getTargetProperty() const { return targetValue; }

    void SampleArcBallCamera::setTargetProperty(Vector3 value) { targetValue = value; }

    float SampleArcBallCamera::getDistanceProperty() const { return distanceValue; }

    void SampleArcBallCamera::setDistanceProperty(float value) { distanceValue = value; }

    float SampleArcBallCamera::getInputDistanceRateProperty() const
    {
        return inputDistanceRateValue;
    }

    void SampleArcBallCamera::setInputDistanceRateProperty(float value)
    {
        inputDistanceRateValue = value;
    }

    Vector3 SampleArcBallCamera::getPositionProperty() const
    {
        return targetValue - (getDirectionProperty() * getDistanceProperty());
    }

    void SampleArcBallCamera::setPositionProperty(Vector3 value)
    {
        SetCamera(value, targetValue, Vector3::Up);
    }

    void SampleArcBallCamera::OrbitUp(float angle)
    {
        switch (mode)
        {
            case SampleArcBallCameraMode::Free:
                // rotate the aspect by the angle
                orientation = orientation *
                    Quaternion::CreateFromAxisAngle(Vector3::Right, -angle);

                // normalize to reduce errors
                Quaternion::Normalize(orientation, orientation);
                break;
            case SampleArcBallCameraMode::RollConstrained:
                // update the yaw
                pitch -= angle;

                // constrain pitch to vertical to avoid confusion
                pitch = MathHelper::Clamp(pitch, -(MathHelper::PiOver2) + .0001f,
                    (MathHelper::PiOver2) - .0001f);

                // create a new aspect based on pitch and yaw
                orientation = Quaternion::CreateFromAxisAngle(Vector3::Up, -yaw) *
                    Quaternion::CreateFromAxisAngle(Vector3::Right, pitch);
                break;
        }
    }

    void SampleArcBallCamera::OrbitRight(float angle)
    {
        switch (mode)
        {
            case SampleArcBallCameraMode::Free:
                // rotate the aspect by the angle
                orientation = orientation *
                    Quaternion::CreateFromAxisAngle(Vector3::Up, angle);

                // normalize to reduce errors
                Quaternion::Normalize(orientation, orientation);
                break;
            case SampleArcBallCameraMode::RollConstrained:
                // update the yaw
                yaw -= angle;

                // float mod yaw to avoid eventual precision errors
                // as we move away from 0
                yaw = std::fmod(yaw, MathHelper::TwoPi);

                // create a new aspect based on pitch and yaw
                orientation = Quaternion::CreateFromAxisAngle(Vector3::Up, -yaw) *
                    Quaternion::CreateFromAxisAngle(Vector3::Right, pitch);

                // normalize to reduce errors
                Quaternion::Normalize(orientation, orientation);
                break;
        }
    }

    void SampleArcBallCamera::RotateClockwise(float angle)
    {
        switch (mode)
        {
            case SampleArcBallCameraMode::Free:
                // rotate the orientation around the direction vector
                orientation = orientation *
                    Quaternion::CreateFromAxisAngle(Vector3::Forward, angle);
                Quaternion::Normalize(orientation, orientation);
                break;
            case SampleArcBallCameraMode::RollConstrained:
                // Do nothing, we don't want to roll at all to stay consistent
                break;
        }
    }

    void SampleArcBallCamera::SetCamera(Vector3 position, Vector3 target, Vector3 up)
    {
        // Create a look at matrix, to simplify matters a bit
        Matrix temp = Matrix::CreateLookAt(position, target, up);

        // invert the matrix, since we're determining the
        // orientation from the rotation matrix in RH coords
        temp = Matrix::Invert(temp);

        // set the postion
        targetValue = target;

        // create the new aspect from the look-at matrix
        orientation = Quaternion::CreateFromRotationMatrix(temp);

        // When setting a new eye-view direction in one of the gimble-locked modes, the yaw and
        // pitch gimble must be calculated.
        if (mode != SampleArcBallCameraMode::Free)
        {
            // first, get the direction projected on the x/z plne
            Vector3 dir = getDirectionProperty();
            dir.Y = 0;
            if (dir.Length() == 0.0f)
            {
                dir = Vector3::Forward;
            }
            dir.Normalize();

            // find the yaw of the direction on the x/z plane
            // and use the sign of the x-component since we have 360 degrees
            // of freedom
            yaw = (float)(std::acos(-dir.Z) * (double)System::Math::Sign(dir.X));

            // Get the pitch from the angle formed by the Up vector and the
            // the forward direction, then subtracting Pi / 2, since
            // we pitch is zero at Forward, not Up.
            pitch = (float)-(std::acos(Vector3::Dot(Vector3::Up, getDirectionProperty()))
                - MathHelper::PiOver2);
        }
    }

    void SampleArcBallCamera::HandleDefaultKeyboardControls(const KeyboardState& kbState,
                                                            const GameTime& gameTime)
    {
        // The original throws ArgumentNullException when gameTime is null; a C++ reference
        // cannot be null, so the guard has nothing to check.
        float elapsedTime = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        float dX = elapsedTime * ReadKeyboardAxis(kbState, Keys::A, Keys::D) * InputTurnRate;
        float dY = elapsedTime * ReadKeyboardAxis(kbState, Keys::S, Keys::W) * InputTurnRate;

        if (dY != 0) OrbitUp(dY);
        if (dX != 0) OrbitRight(dX);

        distanceValue += ReadKeyboardAxis(kbState, Keys::Z, Keys::X)
            * inputDistanceRateValue * elapsedTime;
        if (distanceValue < .001f) distanceValue = .001f;

        if (mode != SampleArcBallCameraMode::Free)
        {
            float dR = elapsedTime * ReadKeyboardAxis(kbState, Keys::Q, Keys::E) * InputTurnRate;
            if (dR != 0) RotateClockwise(dR);
        }
    }

    void SampleArcBallCamera::HandleDefaultGamepadControls(const GamePadState& gpState,
                                                           const GameTime& gameTime)
    {
        // The original's null check on gameTime cannot apply to a C++ reference.
        if (gpState.getIsConnectedProperty())
        {
            float elapsedTime =
                (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

            float dX = gpState.getThumbSticksProperty().getRightProperty().X * elapsedTime *
                       InputTurnRate;
            float dY = gpState.getThumbSticksProperty().getRightProperty().Y * elapsedTime *
                       InputTurnRate;
            float dR = gpState.getTriggersProperty().getRightProperty() * elapsedTime *
                       InputTurnRate;
            dR -= gpState.getTriggersProperty().getLeftProperty() * elapsedTime * InputTurnRate;

            // change orientation if necessary
            if (dY != 0) OrbitUp(dY);
            if (dX != 0) OrbitRight(dX);
            if (dR != 0) RotateClockwise(dR);

            // decrease distance to target (move forward)
            if (gpState.getButtonsProperty().getAProperty() == ButtonState::Pressed)
            {
                distanceValue -= elapsedTime * inputDistanceRateValue;
            }
            // increase distance to target (move back)
            if (gpState.getButtonsProperty().getBProperty() == ButtonState::Pressed)
            {
                distanceValue += elapsedTime * inputDistanceRateValue;
            }
            if (distanceValue < .001f) distanceValue = .001f;
        }
    }

    void SampleArcBallCamera::Reset()
    {
        // orientation quaternion assumes a Pi rotation so you're facing the "front"
        // of the model (looking down the +Z axis)
        orientation = Quaternion::CreateFromAxisAngle(Vector3::Up, MathHelper::Pi);
        distanceValue = 3.0f;
        targetValue = Vector3::Zero;
        yaw = MathHelper::Pi;
        pitch = 0;
    }
}
