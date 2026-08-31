// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PlanePrimitiveDualTextured.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "CustomVertexFormats.hpp"
#include "Primitives/ProceduralPrimitive.hpp"

namespace SoccerPitch
{
    /** @brief Plane primitive with independently tiled texture-coordinate channels. */
    class PlanePrimitiveDualTextured
        : public ProceduralPrimitive<VertexPositionNormalDualTexture>
    {
    public:
        /**
         * @brief Constructs a plane supporting independent tiling for both textures.
         *
         * @param graphicsDevice Graphics device that owns the primitive buffers.
         * @param size Width and depth of the plane.
         * @param tiling1 Tiling applied to the first texture coordinate.
         * @param tiling2 Tiling applied to the second texture coordinate.
         */
        PlanePrimitiveDualTextured(
            GraphicsDevice& graphicsDevice,
            float size,
            Vector2 tiling1,
            Vector2 tiling2);
    };
}
