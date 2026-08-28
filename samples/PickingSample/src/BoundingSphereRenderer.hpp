// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// BoundingSphereRenderer.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"

namespace BoundingVolumeRendering
{
    using Microsoft::Xna::Framework::BoundingSphere;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;

    /**
     * @brief Renders the wireframe of a BoundingSphere.
     *
     * The original is a C# static class whose Draw is an extension method on BoundingSphere
     * (`this BoundingSphere sphere`); C++ has no extension methods, so it is a plain static
     * function, which is also how the sample's own call site spells it:
     * `BoundingSphereRenderer.Draw(sphere, viewMatrix, projectionMatrix)`.
     */
    class BoundingSphereRenderer
    {
    public:
        /**
         * @brief Creates the vertex buffer and effect used to render the spheres.
         *
         * @param graphicsDevice The device the buffer and effect are created on.
         * @param sphereResolution How many line segments each of the three rings is made of.
         */
        static void Initialize(GraphicsDevice& graphicsDevice, int sphereResolution);

        /**
         * @brief Renders a BoundingSphere.
         *
         * @param sphere The sphere to render.
         * @param view The camera's view matrix.
         * @param projection The camera's projection matrix.
         */
        static void Draw(const BoundingSphere& sphere, const Matrix& view, const Matrix& projection);
    };
}
