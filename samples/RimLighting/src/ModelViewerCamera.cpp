// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ModelViewerCamera.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "ModelViewerCamera.hpp"

namespace RimLighting
{
    ModelViewerCamera::ModelViewerCamera(Vector3 CameraPosition, Vector3 CameraUpDir,
                                         int x, int y, int width, int height)
        : worldArcball(x, y, width, height)
        , viewArcball(x, y, width, height)
    {
        cameraPosition = CameraPosition;
        cameraUpDir = CameraUpDir;
    }

    bool ModelViewerCamera::getIsRotatingWorldProperty() const { return isRotatingWorldInt; }

    void ModelViewerCamera::setIsRotatingWorldProperty(bool value)
    {
        if (isRotatingWorldInt == false && value == true)
        {
            // Absorb the difference from last view rotation to current view rotation into world
            // rotation
            worldArcball.SetCurrentRotation(
                Matrix::Invert(viewArcball.GetCurrentRotationMatrix()) * lastViewRotation *
                worldArcball.GetCurrentRotationMatrix() *
                Matrix::Invert(lastViewRotation) * viewArcball.GetCurrentRotationMatrix());

            // So that when lastViewRotation is updated here, GetWorldMatrix() still returns the
            // same world rotation
            // This is necessary since we don't want the object to jump when switching between
            // world/camera rotation modes
            lastViewRotation = viewArcball.GetCurrentRotationMatrix();
        }

        isRotatingWorldInt = value;
    }

    void ModelViewerCamera::HandleTouch(const Input::Touch::TouchLocation& loc)
    {
        if (getIsRotatingWorldProperty())
            worldArcball.HandleTouch(loc);
        else
            viewArcball.HandleTouch(loc);
    }

    Matrix ModelViewerCamera::GetWorldMatrix() const
    {
        // V * W * V^-1 is needed here becase we want the object to rotate natually
        // no matter whether the rotation of view matrix is identity or not
        return (lastViewRotation) * worldArcball.GetCurrentRotationMatrix() *
               Matrix::Invert(lastViewRotation);
    }

    Matrix ModelViewerCamera::GetViewMatrix() const
    {
        // Rotate the camera
        Matrix rot = Matrix::Invert(viewArcball.GetCurrentRotationMatrix());
        return Matrix::CreateLookAt(Vector3::Transform(cameraPosition, rot),
                                    Vector3::Zero,
                                    Vector3::Transform(cameraUpDir, rot));
    }
}
