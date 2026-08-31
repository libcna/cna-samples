// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SpherePrimitiveTextured.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Graphics/VertexPositionNormalTexture.hpp"
#include "Primitives/ProceduralPrimitive.hpp"

namespace SoccerPitch
{
    /** @brief Textured sphere primitive generated from latitude and longitude rings. */
    class SpherePrimitiveTextured
        : public ProceduralPrimitive<Microsoft::Xna::Framework::Graphics::VertexPositionNormalTexture>
    {
    public:
        /**
         * @brief Constructs a sphere using the default size and tessellation.
         *
         * @param graphicsDevice Graphics device that owns the primitive buffers.
         */
        explicit SpherePrimitiveTextured(GraphicsDevice& graphicsDevice);

        /**
         * @brief Constructs a sphere using the specified diameter and tessellation.
         *
         * @param graphicsDevice Graphics device that owns the primitive buffers.
         * @param diameter Diameter of the sphere.
         * @param tessellation Number of vertical tessellation segments.
         */
        SpherePrimitiveTextured(
            GraphicsDevice& graphicsDevice, float diameter, int tessellation);

    private:
        static constexpr int DefaultSphereTessellation = 6;
        static constexpr float DefaultSphereSize = 1.0f;
    };
}
