// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// QuadDrawer.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Graphics/AlphaTestEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionTexture.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace Audio3D
{
    /** @brief Draws billboard sprites and the checkered ground as textured quadrilaterals. */
    class QuadDrawer
    {
    public:
        /**
         * @brief Constructs a quadrilateral drawing worker.
         * @param device Graphics device used for drawing.
         */
        explicit QuadDrawer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);

        /**
         * @brief Draws a quadrilateral as part of the 3D world.
         * @param texture Texture applied to the quadrilateral.
         * @param textureRepeats Number of times the texture repeats across the surface.
         * @param world World transform.
         * @param view View transform.
         * @param projection Projection transform.
         */
        void DrawQuad(
            Microsoft::Xna::Framework::Graphics::Texture2D& texture,
            SharpRuntime::Single textureRepeats,
            const Microsoft::Xna::Framework::Matrix& world,
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection);

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& graphicsDevice;
        Microsoft::Xna::Framework::Graphics::AlphaTestEffect effect;
        Microsoft::Xna::Framework::Graphics::VertexPositionTexture vertices[4];
    };
}
