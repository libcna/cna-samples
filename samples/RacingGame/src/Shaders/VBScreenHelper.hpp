// SPDX-License-Identifier: MS-PL

#pragma once

#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class GraphicsDevice;
}

namespace RacingGame::Shaders
{
    /** @brief Owns the original fullscreen quad and 10x10 radial-blur grid. */
    class VBScreenHelper
    {
    public:
        /**
         * @brief Builds reusable screen geometry.
         * @param device Graphics device that owns the buffers.
         */
        explicit VBScreenHelper(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);

        /** @brief Renders the original four-vertex fullscreen triangle strip. */
        void Render();
        /** @brief Renders the original indexed 10x10 screen grid. */
        void Render10x10Grid();

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device;
        Microsoft::Xna::Framework::Graphics::VertexBuffer screenVertices;
        Microsoft::Xna::Framework::Graphics::VertexBuffer gridVertices;
        Microsoft::Xna::Framework::Graphics::IndexBuffer gridIndices;
    };
}
