// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ChaseCamera.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace ChaseCameraSample
{
    using SharpRuntime::Single;

    /** @brief Implements a damped spring camera that follows a moving object. */
    class ChaseCamera
    {
    public:
        /**
         * @brief Gets the position of the object being chased.
         * @return The chased object's world-space position.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getChasePositionProperty() const;

        /**
         * @brief Sets the position of the object being chased.
         * @param value The chased object's world-space position.
         */
        void setChasePositionProperty(Microsoft::Xna::Framework::Vector3 value);

        /**
         * @brief Gets the direction the chased object is facing.
         * @return The chased object's forward direction.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getChaseDirectionProperty() const;

        /**
         * @brief Sets the direction the chased object is facing.
         * @param value The chased object's forward direction.
         */
        void setChaseDirectionProperty(Microsoft::Xna::Framework::Vector3 value);

        /**
         * @brief Gets the chased object's up vector.
         * @return The chased object's up vector.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getUpProperty() const;

        /**
         * @brief Sets the chased object's up vector.
         * @param value The chased object's up vector.
         */
        void setUpProperty(Microsoft::Xna::Framework::Vector3 value);

        /**
         * @brief Gets the desired camera offset in chased-object coordinates.
         * @return The desired camera offset.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3
        getDesiredPositionOffsetProperty() const;

        /**
         * @brief Sets the desired camera offset in chased-object coordinates.
         * @param value The desired camera offset.
         */
        void setDesiredPositionOffsetProperty(Microsoft::Xna::Framework::Vector3 value);

        /**
         * @brief Gets the desired camera position in world space.
         * @return The desired camera position.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getDesiredPositionProperty();

        /**
         * @brief Gets the look-at offset in chased-object coordinates.
         * @return The look-at offset.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getLookAtOffsetProperty() const;

        /**
         * @brief Sets the look-at offset in chased-object coordinates.
         * @param value The look-at offset.
         */
        void setLookAtOffsetProperty(Microsoft::Xna::Framework::Vector3 value);

        /**
         * @brief Gets the look-at point in world space.
         * @return The look-at point.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getLookAtProperty();

        /**
         * @brief Gets the spring stiffness coefficient.
         * @return The stiffness coefficient.
         */
        [[nodiscard]] Single getStiffnessProperty() const;

        /**
         * @brief Sets the spring stiffness coefficient.
         * @param value The stiffness coefficient.
         */
        void setStiffnessProperty(Single value);

        /**
         * @brief Gets the spring damping coefficient.
         * @return The damping coefficient.
         */
        [[nodiscard]] Single getDampingProperty() const;

        /**
         * @brief Sets the spring damping coefficient.
         * @param value The damping coefficient.
         */
        void setDampingProperty(Single value);

        /**
         * @brief Gets the camera body's mass.
         * @return The camera mass.
         */
        [[nodiscard]] Single getMassProperty() const;

        /**
         * @brief Sets the camera body's mass.
         * @param value The camera mass.
         */
        void setMassProperty(Single value);

        /**
         * @brief Gets the current camera position in world space.
         * @return The camera position.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getPositionProperty() const;

        /**
         * @brief Gets the current camera velocity.
         * @return The camera velocity.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getVelocityProperty() const;

        /**
         * @brief Gets the perspective aspect ratio.
         * @return The perspective aspect ratio.
         */
        [[nodiscard]] Single getAspectRatioProperty() const;

        /**
         * @brief Sets the perspective aspect ratio.
         * @param value The perspective aspect ratio.
         */
        void setAspectRatioProperty(Single value);

        /**
         * @brief Gets the perspective field of view in radians.
         * @return The field of view.
         */
        [[nodiscard]] Single getFieldOfViewProperty() const;

        /**
         * @brief Sets the perspective field of view in radians.
         * @param value The field of view.
         */
        void setFieldOfViewProperty(Single value);

        /**
         * @brief Gets the near clipping-plane distance.
         * @return The near clipping-plane distance.
         */
        [[nodiscard]] Single getNearPlaneDistanceProperty() const;

        /**
         * @brief Sets the near clipping-plane distance.
         * @param value The near clipping-plane distance.
         */
        void setNearPlaneDistanceProperty(Single value);

        /**
         * @brief Gets the far clipping-plane distance.
         * @return The far clipping-plane distance.
         */
        [[nodiscard]] Single getFarPlaneDistanceProperty() const;

        /**
         * @brief Sets the far clipping-plane distance.
         * @param value The far clipping-plane distance.
         */
        void setFarPlaneDistanceProperty(Single value);

        /**
         * @brief Gets the view transform matrix.
         * @return The view transform matrix.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix getViewProperty() const;

        /**
         * @brief Gets the projection transform matrix.
         * @return The projection transform matrix.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix getProjectionProperty() const;

        /** @brief Moves the camera immediately to its desired position and stops it. */
        void Reset();

        /**
         * @brief Advances the damped spring simulation toward the desired position.
         * @param gameTime Timing information for the current frame.
         */
        void Update(const Microsoft::Xna::Framework::GameTime& gameTime);

    private:
        Microsoft::Xna::Framework::Vector3 chasePosition;
        Microsoft::Xna::Framework::Vector3 chaseDirection;
        Microsoft::Xna::Framework::Vector3 up = Microsoft::Xna::Framework::Vector3::Up;

        Microsoft::Xna::Framework::Vector3 desiredPositionOffset{0.0f, 2.0f, 2.0f};
        Microsoft::Xna::Framework::Vector3 desiredPosition;
        Microsoft::Xna::Framework::Vector3 lookAtOffset{0.0f, 2.8f, 0.0f};
        Microsoft::Xna::Framework::Vector3 lookAt;

        Single stiffness = 1800.0f;
        Single damping = 600.0f;
        Single mass = 50.0f;

        Microsoft::Xna::Framework::Vector3 position;
        Microsoft::Xna::Framework::Vector3 velocity;

        Single aspectRatio = 4.0f / 3.0f;
        Single fieldOfView = Microsoft::Xna::Framework::MathHelper::ToRadians(45.0f);
        Single nearPlaneDistance = 1.0f;
        Single farPlaneDistance = 100000.0f;

        Microsoft::Xna::Framework::Matrix view;
        Microsoft::Xna::Framework::Matrix projection;

        void UpdateWorldPositions();
        void UpdateMatrices();
    };
}
