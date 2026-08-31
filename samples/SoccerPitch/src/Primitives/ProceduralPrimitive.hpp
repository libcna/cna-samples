// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ProceduralPrimitive.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Graphics/AlphaTestEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DualTextureEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexElementSize.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"
#include "System/ArgumentOutOfRangeException.hpp"
#include "System/IDisposable.hpp"

namespace SoccerPitch
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /** @brief Generic base for geometry assembled procedurally and uploaded to GPU buffers. */
    template<typename T>
    class ProceduralPrimitive : public System::IDisposable
    {
    protected:
        /**
         * @brief Adds an index while the primitive is being constructed.
         *
         * @param index Vertex index to append.
         */
        void AddIndex(int index)
        {
            if (index > UINT16_MAX)
            {
                throw System::ArgumentOutOfRangeException("index");
            }
            indices_.push_back(static_cast<std::uint16_t>(index));
        }

        /**
         * @brief Adds a vertex while the primitive is being constructed.
         *
         * @param vertex Vertex to append.
         */
        void AddVertex(T vertex)
        {
            vertices_.push_back(vertex);
        }

        /**
         * @brief Uploads all accumulated vertices and indices to GPU buffers.
         *
         * @param graphicsDevice Graphics device that owns the buffers.
         * @param vertexDeclaration Declaration describing the raw vertex layout.
         */
        void InitializePrimitive(
            GraphicsDevice& graphicsDevice, const VertexDeclaration& vertexDeclaration)
        {
            vertexBuffer_ = std::make_unique<VertexBuffer>(
                graphicsDevice,
                vertexDeclaration,
                static_cast<int>(vertices_.size()),
                BufferUsage::None);
            vertexBuffer_->SetData(vertices_.data(), static_cast<int>(vertices_.size()));

            indexBuffer_ = std::make_unique<IndexBuffer>(
                graphicsDevice,
                IndexElementSize::SixteenBits,
                static_cast<int>(indices_.size()),
                BufferUsage::None);
            indexBuffer_->SetData(indices_.data(), static_cast<int>(indices_.size()));
        }

        /**
         * @brief Returns the index that will be assigned to the next vertex.
         *
         * @return Current number of accumulated vertices.
         */
        [[nodiscard]] int getCurrentVertexProperty() const
        {
            return static_cast<int>(vertices_.size());
        }

    public:
        /** @brief Creates an empty procedural primitive. */
        ProceduralPrimitive() = default;

        /** @brief Releases the primitive's GPU buffers. */
        ~ProceduralPrimitive() override = default;

        /** @brief Copying is not supported for GPU-owned primitive data. */
        ProceduralPrimitive(const ProceduralPrimitive&) = delete;

        /** @brief Copy assignment is not supported for GPU-owned primitive data. */
        ProceduralPrimitive& operator=(const ProceduralPrimitive&) = delete;

        /** @brief Move-constructs a primitive and transfers its GPU resources. */
        ProceduralPrimitive(ProceduralPrimitive&&) noexcept = default;

        /** @brief Move-assigns a primitive and transfers its GPU resources. */
        ProceduralPrimitive& operator=(ProceduralPrimitive&&) noexcept = default;

        /** @brief Frees resources used by this primitive. */
        void Dispose() override
        {
            if (disposed_)
            {
                return;
            }
            disposed_ = true;
            if (vertexBuffer_)
            {
                vertexBuffer_->Dispose();
                vertexBuffer_.reset();
            }
            if (indexBuffer_)
            {
                indexBuffer_->Dispose();
                indexBuffer_.reset();
            }
        }

        /**
         * @brief Draws the primitive with a caller-configured effect without changing render state.
         *
         * @param effect Effect whose current technique is applied for the draw.
         */
        void Draw(Effect& effect)
        {
            ThrowIfDisposed();
            GraphicsDevice& graphicsDevice = *effect.getGraphicsDeviceProperty();
            graphicsDevice.SetVertexBuffer(vertexBuffer_.get());
            graphicsDevice.setIndicesProperty(indexBuffer_.get());

            for (EffectPass& effectPass :
                 effect.getCurrentTechniqueProperty()->getPassesProperty())
            {
                effectPass.Apply();
                const int primitiveCount = static_cast<int>(indices_.size()) / 3;
                graphicsDevice.DrawIndexedPrimitives(
                    PrimitiveType::TriangleList,
                    0,
                    0,
                    static_cast<int>(vertices_.size()),
                    0,
                    primitiveCount);
            }
        }

        /**
         * @brief Draws the primitive using a BasicEffect and XNA's standard model render states.
         *
         * @param basicEffect Effect used to draw the primitive.
         * @param world World matrix.
         * @param view View matrix.
         * @param projection Projection matrix.
         * @param color Diffuse color and alpha.
         */
        void Draw(
            BasicEffect& basicEffect,
            const Matrix& world,
            const Matrix& view,
            const Matrix& projection,
            Color color)
        {
            basicEffect.setWorldProperty(world);
            basicEffect.setViewProperty(view);
            basicEffect.setProjectionProperty(projection);
            basicEffect.setDiffuseColorProperty(color.ToVector3());
            basicEffect.setAlphaProperty(color.getAProperty() / 255.0f);

            SetRenderStates(*basicEffect.getGraphicsDeviceProperty(), color);
            Draw(basicEffect);
        }

        /**
         * @brief Draws the primitive using an AlphaTestEffect.
         *
         * @param atEffect Effect used to draw the primitive.
         * @param world World matrix.
         * @param view View matrix.
         * @param projection Projection matrix.
         * @param color Diffuse color and alpha.
         */
        void DrawAlphaTest(
            AlphaTestEffect& atEffect,
            const Matrix& world,
            const Matrix& view,
            const Matrix& projection,
            Color color)
        {
            atEffect.setWorldProperty(world);
            atEffect.setViewProperty(view);
            atEffect.setProjectionProperty(projection);
            atEffect.setDiffuseColorProperty(color.ToVector3());
            atEffect.setAlphaProperty(color.getAProperty() / 255.0f);

            SetRenderStates(*atEffect.getGraphicsDeviceProperty(), color);
            Draw(atEffect);
        }

        /**
         * @brief Draws the primitive using a DualTextureEffect.
         *
         * @param dtEffect Effect used to draw the primitive.
         * @param world World matrix.
         * @param view View matrix.
         * @param projection Projection matrix.
         * @param color Diffuse color and alpha.
         */
        void DrawDualTextured(
            DualTextureEffect& dtEffect,
            const Matrix& world,
            const Matrix& view,
            const Matrix& projection,
            Color color)
        {
            dtEffect.setWorldProperty(world);
            dtEffect.setViewProperty(view);
            dtEffect.setProjectionProperty(projection);
            dtEffect.setDiffuseColorProperty(color.ToVector3());
            dtEffect.setAlphaProperty(color.getAProperty() / 255.0f);

            SetRenderStates(*dtEffect.getGraphicsDeviceProperty(), color);
            Draw(dtEffect);
        }

    private:
        void ThrowIfDisposed() const
        {
            if (disposed_)
            {
                throw std::runtime_error("ProceduralPrimitive has been disposed.");
            }
        }

        static void SetRenderStates(GraphicsDevice& graphicsDevice, Color color)
        {
            if (color.getAProperty() < 255)
            {
                graphicsDevice.setDepthStencilStateProperty(DepthStencilState::None);
                graphicsDevice.setBlendStateProperty(BlendState::Additive);
            }
            else
            {
                graphicsDevice.setDepthStencilStateProperty(DepthStencilState::Default);
                graphicsDevice.setBlendStateProperty(BlendState::Opaque);
            }
        }

        std::unique_ptr<VertexBuffer> vertexBuffer_;
        std::unique_ptr<IndexBuffer> indexBuffer_;
        std::vector<std::uint16_t> indices_;
        std::vector<T> vertices_;
        bool disposed_ = false;
    };
}
