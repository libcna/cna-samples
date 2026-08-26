// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ModelViewerCamera.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Arcball.hpp"

namespace RimLighting
{
    /**
     * @brief Simple ModelViewerCamera which uses two arcballs, one for rotating objects in
     *        world space, another one for rotating camera.
     */
    class ModelViewerCamera
    {
    public:
        /**
         * @brief Constructor, camera's position and its up direction, as well as the bounding
         *        box of the arcball are needed.
         *
         * @param CameraPosition The initial camera position.
         * @param CameraUpDir The initial camera up direction.
         * @param x The arcball bounding box's left edge, in screen pixels.
         * @param y The arcball bounding box's top edge, in screen pixels.
         * @param width The arcball bounding box's width, in screen pixels.
         * @param height The arcball bounding box's height, in screen pixels.
         */
        ModelViewerCamera(Vector3 CameraPosition, Vector3 CameraUpDir,
                          int x, int y, int width, int height);

        /**
         * @brief Gets the mode selector: is the camera currently rotating in world space or
         *        rotating the camera?
         * @return True when rotating world, false when rotating camera.
         */
        [[nodiscard]] bool getIsRotatingWorldProperty() const;

        /**
         * @brief Sets the mode selector, absorbing the view rotation into the world rotation so
         *        the object does not jump when the mode changes.
         * @param value True to rotate world, false to rotate camera.
         */
        void setIsRotatingWorldProperty(bool value);

        /**
         * @brief Process the touch input, rotates the world or camera according to current mode
         *        selector.
         * @param loc The touch location to process.
         */
        void HandleTouch(const Input::Touch::TouchLocation& loc);

        /**
         * @brief Get current world matrix.
         * @return The world matrix.
         */
        [[nodiscard]] Matrix GetWorldMatrix() const;

        /**
         * @brief Get current view matrix.
         * @return The view matrix.
         */
        [[nodiscard]] Matrix GetViewMatrix() const;

    protected:
        bool isRotatingWorldInt = true;

    private:
        // Acrballs for rotating world and camera
        Arcball worldArcball;
        Arcball viewArcball;

        // The initial camera position and up direction
        Vector3 cameraPosition;
        Vector3 cameraUpDir;

        Matrix lastViewRotation = Matrix::getIdentityProperty();
    };
}
