// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PlanePrimitive.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "CustomVertexFormats.hpp"
#include "Primitives/ProceduralPrimitive.hpp"

namespace SoccerPitch
{
    /** @brief Untextured plane primitive containing positions and normals. */
    class PlanePrimitive : public ProceduralPrimitive<VertexPositionNormal>
    {
    public:
        /**
         * @brief Constructs a plane using the default size.
         *
         * @param graphicsDevice Graphics device that owns the primitive buffers.
         */
        explicit PlanePrimitive(GraphicsDevice& graphicsDevice);

        /**
         * @brief Constructs an untextured plane of the specified size.
         *
         * @param graphicsDevice Graphics device that owns the primitive buffers.
         * @param size Width and depth of the plane.
         */
        PlanePrimitive(GraphicsDevice& graphicsDevice, float size);

    private:
        static constexpr float DefaultPlaneSize = 1.0f;
    };
}
