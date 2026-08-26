// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Arcball.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Arcball.hpp"

#include <cmath>

#include "Microsoft/Xna/Framework/Input/Touch/TouchLocationState.hpp"

namespace RimLighting
{
    using Microsoft::Xna::Framework::Input::Touch::TouchLocationState;

    Arcball::Arcball(int x, int y, int width, int height)
    {
        vec2Offset.X = (float)x;
        vec2Offset.Y = (float)y;
        vec2Size.X = (float)width;
        vec2Size.Y = (float)height;
    }

    Quaternion Arcball::GetCurrentRotationQuaternion() const { return qNow; }

    Matrix Arcball::GetCurrentRotationMatrix() const
    {
        return Matrix::CreateFromQuaternion(qNow);
    }

    void Arcball::SetCurrentRotation(Quaternion rotation) { qNow = rotation; }

    void Arcball::SetCurrentRotation(const Matrix& rotation)
    {
        qNow = Quaternion::CreateFromRotationMatrix(rotation);
    }

    bool Arcball::getIsDraggingProperty() const { return isDraggingInt; }

    void Arcball::HandleTouch(const Input::Touch::TouchLocation& loc)
    {
        if (loc.getStateProperty() == TouchLocationState::Pressed && !getIsDraggingProperty())
        {
            if (loc.getPositionProperty().X >= vec2Offset.X &&
                loc.getPositionProperty().X < (vec2Offset.X + vec2Size.X) &&
                loc.getPositionProperty().Y >= vec2Offset.Y &&
                loc.getPositionProperty().Y < (vec2Offset.Y + vec2Size.Y))
            {
                isDraggingInt = true;
                qDown = qNow;
                vec3DownPt = screenToVector(loc.getPositionProperty().X,
                                            loc.getPositionProperty().Y);
            }
        }
        else
        {
            if (loc.getStateProperty() == TouchLocationState::Released)
            {
                isDraggingInt = false;
            }
        }

        if (getIsDraggingProperty())
        {
            vec3CurrentPt = screenToVector(loc.getPositionProperty().X,
                                           loc.getPositionProperty().Y);
            qNow = quatFromBallPoints(vec3DownPt, vec3CurrentPt) * qDown;
        }
    }

    Vector3 Arcball::screenToVector(float screenX, float screenY) const
    {
        // Scale to screen
        float x = (screenX - vec2Offset.X - vec2Size.X / 2.0f) / (fRadius * vec2Size.X / 2.0f);
        float y = (screenY - vec2Offset.Y - vec2Size.Y / 2.0f) / (fRadius * vec2Size.Y / 2.0f);

        float z = 0.0f;
        float mag = x * x + y * y;

        if (mag > 1.0f)
        {
            float scale = (float)(1.0f / std::sqrt(mag));
            x *= scale;
            y *= scale;
        }
        else
            z = (float)std::sqrt(1.0f - mag);

        return Vector3(x, y, z);
    }

    Quaternion Arcball::quatFromBallPoints(Vector3 from, Vector3 to)
    {
        float dot = Vector3::Dot(from, to);
        Vector3 qPart = Vector3::Cross(from, to);
        return Quaternion(qPart.X, qPart.Y, qPart.Z, dot);
    }
}
