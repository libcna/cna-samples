// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SpherePrimitiveTextured.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Primitives/SpherePrimitiveTextured.hpp"

#include <cmath>

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "System/ArgumentOutOfRangeException.hpp"

namespace SoccerPitch
{
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Graphics::VertexPositionNormalTexture;

    SpherePrimitiveTextured::SpherePrimitiveTextured(GraphicsDevice& graphicsDevice)
        : SpherePrimitiveTextured(
              graphicsDevice, DefaultSphereSize, DefaultSphereTessellation)
    {
    }

    SpherePrimitiveTextured::SpherePrimitiveTextured(
        GraphicsDevice& graphicsDevice, float diameter, int tessellation)
    {
        if (tessellation < 3)
        {
            throw System::ArgumentOutOfRangeException(
                "tessellation must be greater than 3");
        }

        const int verticalSegments = tessellation;
        const int horizontalSegments = tessellation * 2;
        const float radius = diameter / 2.0f;

        VertexPositionNormalTexture vertex;
        vertex.Position = Vector3::Down * radius;
        vertex.Normal = Vector3::Down;
        vertex.TextureCoordinate.X = Vector3::Down.X;
        vertex.TextureCoordinate.Y = Vector3::Down.Y;
        AddVertex(vertex);

        for (int i = 0; i < verticalSegments - 1; i++)
        {
            const float latitude =
                ((i + 1) * MathHelper::Pi / verticalSegments) - MathHelper::PiOver2;
            const float dy = static_cast<float>(std::sin(latitude));
            const float dxz = static_cast<float>(std::cos(latitude));

            for (int j = 0; j < horizontalSegments; j++)
            {
                const float longitude = j * MathHelper::TwoPi / horizontalSegments;
                const float dx = static_cast<float>(std::cos(longitude)) * dxz;
                const float dz = static_cast<float>(std::sin(longitude)) * dxz;

                const Vector3 normal(dx, dy, dz);
                vertex.Position = normal * radius;
                vertex.TextureCoordinate.X = normal.X;
                vertex.TextureCoordinate.Y = normal.Y;
                vertex.Normal = normal;
                AddVertex(vertex);
            }
        }

        vertex.Position = Vector3::Up * radius;
        vertex.Normal = Vector3::Up;
        vertex.TextureCoordinate.X = Vector3::Up.X;
        vertex.TextureCoordinate.Y = Vector3::Up.Y;
        AddVertex(vertex);

        for (int i = 0; i < horizontalSegments; i++)
        {
            AddIndex(0);
            AddIndex(1 + (i + 1) % horizontalSegments);
            AddIndex(1 + i);
        }

        for (int i = 0; i < verticalSegments - 2; i++)
        {
            for (int j = 0; j < horizontalSegments; j++)
            {
                const int nextI = i + 1;
                const int nextJ = (j + 1) % horizontalSegments;

                AddIndex(1 + i * horizontalSegments + j);
                AddIndex(1 + i * horizontalSegments + nextJ);
                AddIndex(1 + nextI * horizontalSegments + j);
                AddIndex(1 + i * horizontalSegments + nextJ);
                AddIndex(1 + nextI * horizontalSegments + nextJ);
                AddIndex(1 + nextI * horizontalSegments + j);
            }
        }

        for (int i = 0; i < horizontalSegments; i++)
        {
            AddIndex(getCurrentVertexProperty() - 1);
            AddIndex(getCurrentVertexProperty() - 2 - (i + 1) % horizontalSegments);
            AddIndex(getCurrentVertexProperty() - 2 - i);
        }

        InitializePrimitive(
            graphicsDevice, VertexPositionNormalTexture::getVertexDeclarationStatic());
    }
}
