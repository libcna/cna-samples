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

namespace PerformanceMeasuring {

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

// Base class for simple geometric primitive models: a vertex buffer, an index
// buffer, plus methods for drawing. Port of Primitives/GeometricPrimitive.cs.
class GeometricPrimitive : public System::IDisposable {
    std::vector<VertexPositionNormal> vertices;
    std::vector<std::uint16_t> indices;

    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<IndexBuffer> indexBuffer;
    std::unique_ptr<BasicEffect> basicEffect;
    bool isDisposed = false;

protected:
    void AddVertex(Vector3 position, Vector3 normal) { vertices.emplace_back(position, normal); }

    void AddIndex(int index) {
        if (index > 65535)
            throw System::ArgumentOutOfRangeException("index");
        indices.push_back(static_cast<std::uint16_t>(index));
    }

    [[nodiscard]] int getCurrentVertexProperty() const { return static_cast<int>(vertices.size()); }

    void InitializePrimitive(GraphicsDevice& device) {
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
        basicEffect->setPreferPerPixelLightingProperty(false);
    }

    virtual void Dispose(bool disposing) {
        if (isDisposed)
            return;

        if (disposing) {
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
    ~GeometricPrimitive() override = 0;

    void Dispose() override { Dispose(true); }

    // Draws the primitive with a BasicEffect using default renderstates.
    void Draw(const Matrix& world, const Matrix& view, const Matrix& projection, Color color) {
        GraphicsDevice& device = basicEffect->getGraphicsDeviceInternal();

        basicEffect->setWorldProperty(world);
        basicEffect->setViewProperty(view);
        basicEffect->setProjectionProperty(projection);

        basicEffect->setDiffuseColorProperty(color.ToVector3());
        basicEffect->setAlphaProperty(color.getAProperty() / 255.0f);

        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setBlendStateProperty(color.getAProperty() < 255 ? BlendState::AlphaBlend : BlendState::Opaque);

        Draw(*basicEffect);
    }

    // Draws the primitive with a caller-supplied effect. Unlike the overload
    // above, this does not set any renderstates.
    void Draw(Effect& effect) {
        GraphicsDevice& device = effect.getGraphicsDeviceInternal();

        device.SetVertexBuffer(vertexBuffer.get());
        device.setIndicesProperty(indexBuffer.get());

        int primitiveCount = static_cast<int>(indices.size()) / 3;

        for (auto& pass : effect.getCurrentTechniqueProperty()->getPassesProperty()) {
            pass.Apply();
            device.DrawIndexedPrimitives(PrimitiveType::TriangleList, 0, 0, static_cast<int>(vertices.size()),
                                          0, primitiveCount);
        }
    }
};

inline GeometricPrimitive::~GeometricPrimitive() { Dispose(false); }

} // namespace PerformanceMeasuring
