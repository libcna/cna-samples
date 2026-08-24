// SPDX-License-Identifier: MS-PL

#include "Camera.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;

    Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
        : projection_(Matrix::CreatePerspectiveFieldOfView(fov, aspectRatio, nearPlane, farPlane))
    {
    }

    const Matrix& Camera::getProjectionProperty() const { return projection_; }
    const Matrix& Camera::getViewProperty() const { return view_; }
    const Vector3& Camera::getViewPositionProperty() const { return viewPosition_; }

    void Camera::setViewPositionProperty(const Vector3& value)
    {
        viewPosition_ = value;
        view_ = Matrix::CreateLookAt(viewPosition_, Vector3::Zero, Vector3::Up);
    }
}
