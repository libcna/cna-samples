// SPDX-License-Identifier: MS-PL
#pragma once

#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

#include "VertexPositionNormal.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexElementSize.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "System/ArgumentOutOfRangeException.hpp"
#include "System/IDisposable.hpp"

namespace Bounce
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    class GeometricPrimitive : public System::IDisposable
    {
        std::vector<VertexPositionNormal> vertices;
        std::vector<std::uint16_t> indices;

        std::unique_ptr<VertexBuffer> vertexBuffer;
        std::unique_ptr<IndexBuffer> indexBuffer;
        std::unique_ptr<BasicEffect> basicEffect;
        std::unique_ptr<BasicEffect> basicEffectShadow;
        bool isDisposed = false;

    protected:
        void AddVertex(Vector3 position, Vector3 normal)
        {
            vertices.emplace_back(position, normal);
        }

        void AddIndex(int index)
        {
            if (index > std::numeric_limits<std::uint16_t>::max())
                throw System::ArgumentOutOfRangeException("index");

            indices.push_back(static_cast<std::uint16_t>(index));
        }

        [[nodiscard]] int CurrentVertex() const
        {
            return static_cast<int>(vertices.size());
        }

        void InitializePrimitive(GraphicsDevice& graphicsDevice)
        {
            vertexBuffer = std::make_unique<VertexBuffer>(
                graphicsDevice,
                VertexPositionNormal::VertexDeclaration,
                static_cast<int>(vertices.size()),
                BufferUsage::None);
            vertexBuffer->SetData(
                vertices.data(), static_cast<int>(vertices.size()));

            indexBuffer = std::make_unique<IndexBuffer>(
                graphicsDevice,
                IndexElementSize::SixteenBits,
                static_cast<int>(indices.size()),
                BufferUsage::None);
            indexBuffer->SetData(
                indices.data(), static_cast<int>(indices.size()));

            basicEffect = std::make_unique<BasicEffect>(graphicsDevice);
            basicEffect->setLightingEnabledProperty(true);
            basicEffect->getDirectionalLight0Property().setEnabledProperty(true);

            Vector3 lightDirection(0.25f, -1.0f, -1.0f);
            lightDirection.Normalize();
            basicEffect->getDirectionalLight0Property()
                .setDirectionProperty(lightDirection);
            basicEffect->setSpecularColorProperty(Vector3::One);
            basicEffect->getDirectionalLight0Property()
                .setSpecularColorProperty(Vector3::One);
            basicEffect->setSpecularPowerProperty(32.0f);
            basicEffect->setPreferPerPixelLightingProperty(false);

            basicEffectShadow = std::make_unique<BasicEffect>(graphicsDevice);
            basicEffectShadow->setLightingEnabledProperty(false);
            basicEffectShadow->setPreferPerPixelLightingProperty(false);
        }

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
                if (basicEffectShadow)
                    basicEffectShadow->Dispose();
            }

            isDisposed = true;
        }

    public:
        ~GeometricPrimitive() override
        {
            Dispose(false);
        }

        void Dispose() override
        {
            Dispose(true);
        }

        void Draw(BasicEffect& effect)
        {
            GraphicsDevice& graphicsDevice = *effect.getGraphicsDeviceProperty();
            graphicsDevice.SetVertexBuffer(vertexBuffer.get());
            graphicsDevice.setIndicesProperty(indexBuffer.get());

            for (auto& effectPass :
                 effect.getCurrentTechniqueProperty()->getPassesProperty())
            {
                effectPass.Apply();
                const int primitiveCount = static_cast<int>(indices.size()) / 3;
                graphicsDevice.DrawIndexedPrimitives(
                    PrimitiveType::TriangleList,
                    0,
                    0,
                    static_cast<int>(vertices.size()),
                    0,
                    primitiveCount);
            }
        }

        void Draw(
            Matrix world,
            Matrix view,
            Matrix projection,
            Color color,
            bool drawShadow)
        {
            BasicEffect& drawBasicEffect =
                drawShadow ? *basicEffectShadow : *basicEffect;
            drawBasicEffect.setWorldProperty(world);
            drawBasicEffect.setViewProperty(view);
            drawBasicEffect.setProjectionProperty(projection);
            drawBasicEffect.setDiffuseColorProperty(color.ToVector3());
            drawBasicEffect.setAlphaProperty(
                color.getAProperty() / 255.0f);
            Draw(drawBasicEffect);
        }
    };
}
