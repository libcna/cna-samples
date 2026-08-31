// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PlanePrimitiveTextured.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Primitives/PlanePrimitiveTextured.hpp"

namespace SoccerPitch
{
    using Microsoft::Xna::Framework::Graphics::VertexPositionNormalTexture;

    PlanePrimitiveTextured::PlanePrimitiveTextured(GraphicsDevice& graphicsDevice)
        : PlanePrimitiveTextured(graphicsDevice, DefaultPlaneSize)
    {
    }

    PlanePrimitiveTextured::PlanePrimitiveTextured(
        GraphicsDevice& graphicsDevice, float size)
    {
        VertexPositionNormalTexture vertex;
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
        vertex.TextureCoordinate = Vector2(0.0f, 0.0f);
        AddVertex(vertex);
        vertex.Position = (side1 + side2) * halfSize;
        vertex.TextureCoordinate = Vector2(0.0f, 1.0f);
        AddVertex(vertex);
        vertex.Position = (side1 - side2) * halfSize;
        vertex.TextureCoordinate = Vector2(1.0f, 1.0f);
        AddVertex(vertex);
        vertex.Position = (-side1 - side2) * halfSize;
        vertex.TextureCoordinate = Vector2(1.0f, 0.0f);
        AddVertex(vertex);

        InitializePrimitive(
            graphicsDevice, VertexPositionNormalTexture::getVertexDeclarationStatic());
    }
}
