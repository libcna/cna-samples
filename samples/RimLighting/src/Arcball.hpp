// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Arcball.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Quaternion.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocation.hpp"

namespace RimLighting
{
    using namespace Microsoft::Xna::Framework;

    /**
     * @brief Simple Arcball class for handling rotations from user's drag operations on screen.
     */
    class Arcball
    {
    public:
        /**
         * @brief Constructor, the bounding box of the arcball is needed.
         *
         * @param x The bounding box's left edge, in screen pixels.
         * @param y The bounding box's top edge, in screen pixels.
         * @param width The bounding box's width, in screen pixels.
         * @param height The bounding box's height, in screen pixels.
         */
        Arcball(int x, int y, int width, int height);

        /**
         * @brief Returns current rotation as quaternion.
         * @return The current rotation.
         */
        [[nodiscard]] Quaternion GetCurrentRotationQuaternion() const;

        /**
         * @brief Returns current rotation as matrix.
         * @return The current rotation.
         */
        [[nodiscard]] Matrix GetCurrentRotationMatrix() const;

        /**
         * @brief Set current rotation using quaternion.
         * @param rotation The rotation to adopt.
         */
        void SetCurrentRotation(Quaternion rotation);

        /**
         * @brief Set current rotation using matrix.
         * @param rotation The rotation to adopt.
         */
        void SetCurrentRotation(const Matrix& rotation);

        /**
         * @brief Is the user currently dragging on this arcball?
         * @return True while a drag is in progress.
         */
        [[nodiscard]] bool getIsDraggingProperty() const;

        /**
         * @brief Process touch input. This should be called within Update() of the game.
         * @param loc The touch location to process.
         */
        void HandleTouch(const Input::Touch::TouchLocation& loc);

    protected:
        /** Converts screen coordinates to coordinates on the sphere. */
        [[nodiscard]] Vector3 screenToVector(float screenX, float screenY) const;

        /** Compute rotation quaternion from two points on the sphere. */
        [[nodiscard]] static Quaternion quatFromBallPoints(Vector3 from, Vector3 to);

        bool isDraggingInt = false;

    private:
        // Current rotation quaternion
        Quaternion qNow = Quaternion::Identity;

        // The rotation quaternion when the user click down on the screen and begins dragging
        Quaternion qDown = Quaternion::Identity;

        // The bounding box in which drag operations is processed by this arcball
        Vector2 vec2Offset;
        Vector2 vec2Size;

        // Coordinates on the sphere when the user begins dragging
        Vector3 vec3DownPt;

        // Coordinates on the sphere during dragging
        Vector3 vec3CurrentPt;

        // Sphere radius (proportional to the bounding box)
        float fRadius = 0.9f;
    };
}
