// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace Spacewar
{
    class Camera
    {
    public:
        Camera(float fov, float aspectRatio, float nearPlane, float farPlane);

        [[nodiscard]] const Microsoft::Xna::Framework::Matrix& getProjectionProperty() const;
        [[nodiscard]] const Microsoft::Xna::Framework::Matrix& getViewProperty() const;
        [[nodiscard]] const Microsoft::Xna::Framework::Vector3& getViewPositionProperty() const;
        void setViewPositionProperty(const Microsoft::Xna::Framework::Vector3& value);

    private:
        Microsoft::Xna::Framework::Matrix projection_;
        Microsoft::Xna::Framework::Matrix view_;
        Microsoft::Xna::Framework::Vector3 viewPosition_;
    };
}
