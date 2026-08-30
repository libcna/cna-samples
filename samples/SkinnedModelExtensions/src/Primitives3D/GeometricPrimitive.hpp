// SPDX-License-Identifier: MS-PL
#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexElementSize.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "System/ArgumentOutOfRangeException.hpp"
#include "System/IDisposable.hpp"

#include "VertexPositionNormal.hpp"

namespace Primitives3D
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /** @brief Common GPU storage and drawing implementation for the geometric primitives. */
    class GeometricPrimitive : public System::IDisposable
    {
        std::vector<VertexPositionNormal> vertices;
        std::vector<std::uint16_t> indices;

        std::unique_ptr<VertexBuffer> vertexBuffer;
        std::unique_ptr<IndexBuffer> indexBuffer;
        std::unique_ptr<BasicEffect> basicEffect;
        bool isDisposed = false;

    protected:
        /** @brief Adds a position and normal while constructing a primitive. */
        void AddVertex(Vector3 position, Vector3 normal)
        {
            vertices.emplace_back(position, normal);
        }

        /** @brief Adds a 16-bit vertex index while constructing a primitive. */
        void AddIndex(int index)
        {
            if (index > UINT16_MAX)
                throw System::ArgumentOutOfRangeException("index");

            indices.push_back(static_cast<std::uint16_t>(index));
        }

        /** @brief Returns the index that will be assigned to the next vertex. */
        [[nodiscard]] int CurrentVertex() const
        {
            return static_cast<int>(vertices.size());
        }

        /** @brief Uploads the completed geometry and creates the default-lit BasicEffect. */
        void InitializePrimitive(GraphicsDevice& device)
        {
            vertexBuffer = std::make_unique<VertexBuffer>(
                device, VertexPositionNormal::VertexDeclaration,
                static_cast<int>(vertices.size()), BufferUsage::None);
            vertexBuffer->SetData(vertices.data(), static_cast<int>(vertices.size()));

            indexBuffer = std::make_unique<IndexBuffer>(
                device, IndexElementSize::SixteenBits,
                static_cast<int>(indices.size()), BufferUsage::None);
            indexBuffer->SetData(indices.data(), static_cast<int>(indices.size()));

            basicEffect = std::make_unique<BasicEffect>(device);
            basicEffect->EnableDefaultLighting();
        }

        /** @brief Releases owned graphics resources when disposal is explicit. */
        virtual void Dispose(bool disposing)
        {
            if (isDisposed)
                return;

            if (disposing)
            {
                if (vertexBuffer)
                    vertexBuffer->Dispose();
                if (indexBuffer)
                    indexBuffer->Dispose();
                if (basicEffect)
                    basicEffect->Dispose();
            }
            isDisposed = true;
        }

    public:
        /** @brief Finalizes the primitive and releases its C++ resource owners. */
        ~GeometricPrimitive() override
        {
            Dispose(false);
        }

        /** @brief Frees the primitive's owned graphics resources. */
        void Dispose() override
        {
            Dispose(true);
        }

        /** @brief Draws the primitive with an explicitly supplied effect. */
        void Draw(Effect& effect)
        {
            GraphicsDevice& device = *effect.getGraphicsDeviceProperty();
            device.SetVertexBuffer(vertexBuffer.get());
            device.setIndicesProperty(indexBuffer.get());

            for (auto& pass : effect.getCurrentTechniqueProperty()->getPassesProperty())
            {
                pass.Apply();
                const int primitiveCount = static_cast<int>(indices.size()) / 3;
                device.DrawIndexedPrimitives(
                    PrimitiveType::TriangleList, 0, 0,
                    static_cast<int>(vertices.size()), 0, primitiveCount);
            }
        }

        /** @brief Draws the primitive with the sample's default-lit BasicEffect. */
        void Draw(const Matrix& world, const Matrix& view, const Matrix& projection, Color color)
        {
            basicEffect->setWorldProperty(world);
            basicEffect->setViewProperty(view);
            basicEffect->setProjectionProperty(projection);
            basicEffect->setDiffuseColorProperty(color.ToVector3());
            basicEffect->setAlphaProperty(color.getAProperty() / 255.0f);

            GraphicsDevice& device = *basicEffect->getGraphicsDeviceProperty();
            device.setDepthStencilStateProperty(DepthStencilState::Default);
            if (color.getAProperty() < 255)
                device.setBlendStateProperty(BlendState::AlphaBlend);
            else
                device.setBlendStateProperty(BlendState::Opaque);

            Draw(*basicEffect);
        }
    };
}
