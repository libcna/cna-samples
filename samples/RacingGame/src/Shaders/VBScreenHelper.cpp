// SPDX-License-Identifier: MS-PL

#include "Shaders/VBScreenHelper.hpp"

#include <array>
#include <cstdint>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexElementSize.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionTexture.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace RacingGame::Shaders
{
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;
    using namespace Microsoft::Xna::Framework::Graphics;

    VBScreenHelper::VBScreenHelper(GraphicsDevice& setDevice)
        : device(setDevice),
          screenVertices(
              device, VertexPositionTexture::getVertexDeclarationStatic(),
              4, BufferUsage::WriteOnly),
          gridVertices(
              device, VertexPositionTexture::getVertexDeclarationStatic(),
              100, BufferUsage::WriteOnly),
          gridIndices(
              device, IndexElementSize::SixteenBits,
              9 * 9 * 6, BufferUsage::WriteOnly)
    {
        const std::array<VertexPositionTexture, 4> screen{{
            {Vector3(-1.0f, -1.0f, 0.5f), Vector2(0.0f, 1.0f)},
            {Vector3(-1.0f, 1.0f, 0.5f), Vector2(0.0f, 0.0f)},
            {Vector3(1.0f, -1.0f, 0.5f), Vector2(1.0f, 1.0f)},
            {Vector3(1.0f, 1.0f, 0.5f), Vector2(1.0f, 0.0f)},
        }};
        screenVertices.SetData(screen.data(), static_cast<int>(screen.size()));

        std::vector<VertexPositionTexture> vertices(100);
        for (int x = 0; x < 10; ++x)
        {
            for (int y = 0; y < 10; ++y)
            {
                vertices[static_cast<std::size_t>(x + y * 10)] =
                    VertexPositionTexture(
                        Vector3(-1.0f + 2.0f * static_cast<float>(x) / 9.0f,
                                -1.0f + 2.0f * static_cast<float>(y) / 9.0f,
                                0.5f),
                        Vector2(static_cast<float>(x) / 9.0f,
                                1.0f - static_cast<float>(y) / 9.0f));
            }
        }
        gridVertices.SetData(vertices.data(), static_cast<int>(vertices.size()));

        std::vector<std::uint16_t> indices(9 * 9 * 6);
        std::size_t output = 0;
        for (int x = 0; x < 9; ++x)
        {
            for (int y = 0; y < 9; ++y)
            {
                const std::uint16_t index1 =
                    static_cast<std::uint16_t>(x + y * 10);
                const std::uint16_t index2 =
                    static_cast<std::uint16_t>(x + 1 + y * 10);
                const std::uint16_t index3 =
                    static_cast<std::uint16_t>(x + 1 + (y + 1) * 10);
                const std::uint16_t index4 =
                    static_cast<std::uint16_t>(x + (y + 1) * 10);
                indices[output++] = index1;
                indices[output++] = index3;
                indices[output++] = index2;
                indices[output++] = index1;
                indices[output++] = index4;
                indices[output++] = index3;
            }
        }
        gridIndices.SetData(indices.data(), static_cast<int>(indices.size()));
    }

    void VBScreenHelper::Render()
    {
        device.SetVertexBuffer(&screenVertices);
        device.setIndicesProperty(nullptr);
        device.DrawPrimitives(PrimitiveType::TriangleStrip, 0, 2);
    }

    void VBScreenHelper::Render10x10Grid()
    {
        device.SetVertexBuffer(&gridVertices);
        device.setIndicesProperty(&gridIndices);
        device.DrawIndexedPrimitives(
            PrimitiveType::TriangleList, 0, 0, 100, 0, 9 * 9 * 2);
    }
}
