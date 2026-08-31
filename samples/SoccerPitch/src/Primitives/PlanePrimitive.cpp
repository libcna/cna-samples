// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PlanePrimitive.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Primitives/PlanePrimitive.hpp"

namespace SoccerPitch
{
    PlanePrimitive::PlanePrimitive(GraphicsDevice& graphicsDevice)
        : PlanePrimitive(graphicsDevice, DefaultPlaneSize)
    {
    }

    PlanePrimitive::PlanePrimitive(GraphicsDevice& graphicsDevice, float size)
    {
        VertexPositionNormal vertex;
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
        AddVertex(vertex);
        vertex.Position = (side1 + side2) * halfSize;
        AddVertex(vertex);
        vertex.Position = (side1 - side2) * halfSize;
        AddVertex(vertex);
        vertex.Position = (-side1 - side2) * halfSize;
        AddVertex(vertex);

        InitializePrimitive(graphicsDevice, VertexPositionNormal::VertexDeclaration);
    }
}
