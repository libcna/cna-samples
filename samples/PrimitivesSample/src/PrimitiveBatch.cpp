// SPDX-License-Identifier: MS-PL

#include "PrimitiveBatch.hpp"

#include "System/InvalidOperationException.hpp"
#include "System/NotSupportedException.hpp"

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace PrimitivesSample
{
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;

    PrimitiveBatch::PrimitiveBatch(GraphicsDevice& graphicsDevice)
        : vertices(DefaultBufferSize)
        , basicEffect(graphicsDevice)
        , device(graphicsDevice)
    {
        basicEffect.VertexColorEnabled = true;

        const auto& viewport = graphicsDevice.getViewportProperty();
        basicEffect.Projection = Matrix::CreateOrthographicOffCenter(
            0.0f,
            static_cast<float>(viewport.getWidthProperty()),
            static_cast<float>(viewport.getHeightProperty()),
            0.0f,
            0.0f,
            1.0f);
    }

    PrimitiveBatch::~PrimitiveBatch() = default;

    void PrimitiveBatch::Dispose()
    {
        Dispose(true);
    }

    void PrimitiveBatch::Dispose(bool disposing)
    {
        if (disposing && !isDisposed)
        {
            basicEffect.Dispose();
            isDisposed = true;
        }
    }

    void PrimitiveBatch::Begin(PrimitiveType primitive)
    {
        if (hasBegun)
        {
            throw System::InvalidOperationException(
                "End must be called before Begin can be called again.");
        }

        if (primitive == PrimitiveType::LineStrip ||
            primitive == PrimitiveType::TriangleStrip)
        {
            throw System::NotSupportedException(
                "The specified primitiveType is not supported by PrimitiveBatch.");
        }

        primitiveType = primitive;
        numVertsPerPrimitive = NumVertsPerPrimitive(primitive);
        basicEffect.getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();
        hasBegun = true;
    }

    void PrimitiveBatch::AddVertex(Vector2 vertex, Color color)
    {
        if (!hasBegun)
        {
            throw System::InvalidOperationException(
                "Begin must be called before AddVertex can be called.");
        }

        const bool newPrimitive = (positionInBuffer % numVertsPerPrimitive) == 0;
        if (newPrimitive &&
            positionInBuffer + numVertsPerPrimitive >= static_cast<intcs>(vertices.size()))
        {
            Flush();
        }

        vertices[static_cast<std::size_t>(positionInBuffer)].Position = Vector3(vertex, 0.0f);
        vertices[static_cast<std::size_t>(positionInBuffer)].Color = color;
        ++positionInBuffer;
    }

    void PrimitiveBatch::End()
    {
        if (!hasBegun)
        {
            throw System::InvalidOperationException(
                "Begin must be called before End can be called.");
        }

        Flush();
        hasBegun = false;
    }

    void PrimitiveBatch::Flush()
    {
        if (!hasBegun)
        {
            throw System::InvalidOperationException(
                "Begin must be called before Flush can be called.");
        }

        if (positionInBuffer == 0)
        {
            return;
        }

        const intcs primitiveCount = positionInBuffer / numVertsPerPrimitive;
        device.DrawUserPrimitives(
            primitiveType, vertices.data(), 0, primitiveCount);
        positionInBuffer = 0;
    }

    intcs PrimitiveBatch::NumVertsPerPrimitive(PrimitiveType primitive)
    {
        switch (primitive)
        {
            case PrimitiveType::LineList:
                return 2;
            case PrimitiveType::TriangleList:
                return 3;
            default:
                throw System::InvalidOperationException("primitive is not valid");
        }
    }
}
