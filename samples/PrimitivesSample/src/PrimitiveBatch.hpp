// SPDX-License-Identifier: MS-PL

#pragma once

#include <vector>

#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/IDisposable.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"

namespace PrimitivesSample
{
    using SharpRuntime::intcs;
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Graphics::BasicEffect;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Graphics::PrimitiveType;
    using Microsoft::Xna::Framework::Graphics::VertexPositionColor;

    /** @brief Batches colored line-list and triangle-list primitives for efficient drawing. */
    class PrimitiveBatch : public System::IDisposable
    {
        static constexpr intcs DefaultBufferSize = 500;

        std::vector<VertexPositionColor> vertices;
        intcs positionInBuffer = 0;
        BasicEffect basicEffect;
        GraphicsDevice& device;
        PrimitiveType primitiveType = PrimitiveType::TriangleList;
        intcs numVertsPerPrimitive = 3;
        bool hasBegun = false;
        bool isDisposed = false;

        [[nodiscard]] static intcs NumVertsPerPrimitive(PrimitiveType primitive);
        void Flush();

    protected:
        /**
         * @brief Releases resources owned by this batch.
         *
         * @param disposing True when called from Dispose().
         */
        virtual void Dispose(bool disposing);

    public:
        /**
         * @brief Creates a primitive batch for a graphics device.
         *
         * @param graphicsDevice Device that receives the primitive draw calls.
         */
        explicit PrimitiveBatch(GraphicsDevice& graphicsDevice);

        /** @brief Destroys the primitive batch. */
        ~PrimitiveBatch() override;

        /** @brief Releases the batch's BasicEffect resource. */
        void Dispose() override;

        /**
         * @brief Starts a batch of the specified primitive type.
         *
         * @param primitive Type of primitives that subsequent vertices describe.
         */
        void Begin(PrimitiveType primitive);

        /**
         * @brief Adds one colored vertex to the active batch.
         *
         * @param vertex Two-dimensional vertex position.
         * @param color Vertex color.
         */
        void AddVertex(Vector2 vertex, Color color);

        /** @brief Flushes and ends the active primitive batch. */
        void End();
    };
}
