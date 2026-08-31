// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PlanePrimitiveDualTextured.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Primitives/PlanePrimitiveDualTextured.hpp"

namespace SoccerPitch
{
    PlanePrimitiveDualTextured::PlanePrimitiveDualTextured(
        GraphicsDevice& graphicsDevice,
        float size,
        Vector2 tiling1,
        Vector2 tiling2)
    {
        VertexPositionNormalDualTexture vertex;
        vertex.Normal = Vector3(0.0f, 1.0f, 0.0f);

        const Vector3 side1(1.0f, 0.0f, 0.0f);
        const Vector3 side2(0.0f, 0.0f, 1.0f);

        AddIndex(getCurrentVertexProperty() + 0);
        AddIndex(getCurrentVertexProperty() + 2);
        AddIndex(getCurrentVertexProperty() + 1);
        AddIndex(getCurrentVertexProperty() + 0);
        AddIndex(getCurrentVertexProperty() + 3);
        AddIndex(getCurrentVertexProperty() + 2);

        const float halfSize = size / 2.0f;
        vertex.Position = (-side1 + side2) * halfSize;
        vertex.TextureCoordinate0 = Vector2(0.0f, 0.0f);
        vertex.TextureCoordinate1 = Vector2(0.0f, 0.0f);
        AddVertex(vertex);

        vertex.Position = (side1 + side2) * halfSize;
        vertex.TextureCoordinate0 = Vector2(0.0f, tiling1.Y);
        vertex.TextureCoordinate1 = Vector2(tiling2.X, 0.0f);
        AddVertex(vertex);

        vertex.Position = (side1 - side2) * halfSize;
        vertex.TextureCoordinate0 = Vector2(tiling1.X, tiling1.Y);
        vertex.TextureCoordinate1 = Vector2(tiling2.X, tiling2.Y);
        AddVertex(vertex);

        vertex.Position = (-side1 - side2) * halfSize;
        vertex.TextureCoordinate0 = Vector2(tiling1.X, 0.0f);
        vertex.TextureCoordinate1 = Vector2(0.0f, tiling2.Y);
        AddVertex(vertex);

        InitializePrimitive(
            graphicsDevice, VertexPositionNormalDualTexture::VertexDeclaration);
    }
}
