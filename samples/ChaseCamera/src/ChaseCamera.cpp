// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ChaseCamera.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ChaseCamera.hpp"

namespace ChaseCameraSample
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;

    Vector3 ChaseCamera::getChasePositionProperty() const
    {
        return chasePosition;
    }

    void ChaseCamera::setChasePositionProperty(Vector3 value)
    {
        chasePosition = value;
    }

    Vector3 ChaseCamera::getChaseDirectionProperty() const
    {
        return chaseDirection;
    }

    void ChaseCamera::setChaseDirectionProperty(Vector3 value)
    {
        chaseDirection = value;
    }

    Vector3 ChaseCamera::getUpProperty() const
    {
        return up;
    }

    void ChaseCamera::setUpProperty(Vector3 value)
    {
        up = value;
    }

    Vector3 ChaseCamera::getDesiredPositionOffsetProperty() const
    {
        return desiredPositionOffset;
    }

    void ChaseCamera::setDesiredPositionOffsetProperty(Vector3 value)
    {
        desiredPositionOffset = value;
    }

    Vector3 ChaseCamera::getDesiredPositionProperty()
    {
        UpdateWorldPositions();
        return desiredPosition;
    }

    Vector3 ChaseCamera::getLookAtOffsetProperty() const
    {
        return lookAtOffset;
    }

    void ChaseCamera::setLookAtOffsetProperty(Vector3 value)
    {
        lookAtOffset = value;
    }

    Vector3 ChaseCamera::getLookAtProperty()
    {
        UpdateWorldPositions();
        return lookAt;
    }

    Single ChaseCamera::getStiffnessProperty() const
    {
        return stiffness;
    }

    void ChaseCamera::setStiffnessProperty(Single value)
    {
        stiffness = value;
    }

    Single ChaseCamera::getDampingProperty() const
    {
        return damping;
    }

    void ChaseCamera::setDampingProperty(Single value)
    {
        damping = value;
    }

    Single ChaseCamera::getMassProperty() const
    {
        return mass;
    }

    void ChaseCamera::setMassProperty(Single value)
    {
        mass = value;
    }

    Vector3 ChaseCamera::getPositionProperty() const
    {
        return position;
    }

    Vector3 ChaseCamera::getVelocityProperty() const
    {
        return velocity;
    }

    Single ChaseCamera::getAspectRatioProperty() const
    {
        return aspectRatio;
    }

    void ChaseCamera::setAspectRatioProperty(Single value)
    {
        aspectRatio = value;
    }

    Single ChaseCamera::getFieldOfViewProperty() const
    {
        return fieldOfView;
    }

    void ChaseCamera::setFieldOfViewProperty(Single value)
    {
        fieldOfView = value;
    }

    Single ChaseCamera::getNearPlaneDistanceProperty() const
    {
        return nearPlaneDistance;
    }

    void ChaseCamera::setNearPlaneDistanceProperty(Single value)
    {
        nearPlaneDistance = value;
    }

    Single ChaseCamera::getFarPlaneDistanceProperty() const
    {
        return farPlaneDistance;
    }

    void ChaseCamera::setFarPlaneDistanceProperty(Single value)
    {
        farPlaneDistance = value;
    }

    Matrix ChaseCamera::getViewProperty() const
    {
        return view;
    }

    Matrix ChaseCamera::getProjectionProperty() const
    {
        return projection;
    }

    void ChaseCamera::UpdateWorldPositions()
    {
        Matrix transform = Matrix::getIdentityProperty();
        transform.setForwardProperty(getChaseDirectionProperty());
        transform.setUpProperty(getUpProperty());
        transform.setRightProperty(Vector3::Cross(getUpProperty(), getChaseDirectionProperty()));

        desiredPosition = getChasePositionProperty()
            + Vector3::TransformNormal(getDesiredPositionOffsetProperty(), transform);
        lookAt = getChasePositionProperty()
            + Vector3::TransformNormal(getLookAtOffsetProperty(), transform);
    }

    void ChaseCamera::UpdateMatrices()
    {
        view = Matrix::CreateLookAt(getPositionProperty(), getLookAtProperty(), getUpProperty());
        projection = Matrix::CreatePerspectiveFieldOfView(
            getFieldOfViewProperty(), getAspectRatioProperty(),
            getNearPlaneDistanceProperty(), getFarPlaneDistanceProperty());
    }

    void ChaseCamera::Reset()
    {
        UpdateWorldPositions();
        velocity = Vector3::Zero;
        position = desiredPosition;
        UpdateMatrices();
    }

    void ChaseCamera::Update(const GameTime& gameTime)
    {
        UpdateWorldPositions();

        const Single elapsed = static_cast<Single>(
            gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());

        const Vector3 stretch = position - desiredPosition;
        const Vector3 force = -stiffness * stretch - damping * velocity;

        const Vector3 acceleration = force / mass;
        velocity += acceleration * elapsed;

        position += velocity * elapsed;

        UpdateMatrices();
    }
}
