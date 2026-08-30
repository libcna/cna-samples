// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SpriteEntity.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "SpriteEntity.hpp"

#include <optional>

#include "QuadDrawer.hpp"

namespace Audio3D
{
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    Vector3 SpriteEntity::getPositionProperty() const
    {
        return position;
    }

    void SpriteEntity::setPositionProperty(const Vector3& value)
    {
        position = value;
    }

    Vector3 SpriteEntity::getForwardProperty() const
    {
        return forward;
    }

    void SpriteEntity::setForwardProperty(const Vector3& value)
    {
        forward = value;
    }

    Vector3 SpriteEntity::getUpProperty() const
    {
        return up;
    }

    void SpriteEntity::setUpProperty(const Vector3& value)
    {
        up = value;
    }

    Vector3 SpriteEntity::getVelocityProperty() const
    {
        return velocity;
    }

    void SpriteEntity::setVelocityProperty(const Vector3& value)
    {
        velocity = value;
    }

    Texture2D* SpriteEntity::getTextureProperty() const
    {
        return texture;
    }

    void SpriteEntity::setTextureProperty(Texture2D* value)
    {
        texture = value;
    }

    void SpriteEntity::Draw(
        QuadDrawer& quadDrawer,
        const Vector3& cameraPosition,
        const Matrix& view,
        const Matrix& projection)
    {
        const Matrix world = Matrix::CreateTranslation(0.0f, 1.0f, 0.0f)
            * Matrix::CreateScale(800.0f)
            * Matrix::CreateConstrainedBillboard(
                getPositionProperty(),
                cameraPosition,
                getUpProperty(),
                std::nullopt,
                std::nullopt);

        quadDrawer.DrawQuad(*getTextureProperty(), 1.0f, world, view, projection);
    }
}
