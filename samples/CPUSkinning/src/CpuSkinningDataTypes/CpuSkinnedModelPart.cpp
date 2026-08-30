// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CpuSkinnedModelPart.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CpuSkinnedModelPart.hpp"

#include <cstddef>
#include <utility>

#include "CpuSkinningHelpers.hpp"

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"

namespace CpuSkinningDataTypes
{
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Graphics::BufferUsage;
    using Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer;
    using Microsoft::Xna::Framework::Graphics::EffectPass;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Graphics::IndexBuffer;
    using Microsoft::Xna::Framework::Graphics::PrimitiveType;
    using Microsoft::Xna::Framework::Graphics::SetDataOptions;
    using Microsoft::Xna::Framework::Graphics::VertexPositionNormalTexture;

    CpuSkinnedModelPart::CpuSkinnedModelPart(
        int triangleCount,
        std::vector<CpuVertex> vertices,
        std::shared_ptr<IndexBuffer> indexBuffer)
        : triangleCount(triangleCount)
        , vertexCount(static_cast<int>(vertices.size()))
        , cpuVertices(std::move(vertices))
        , gpuVertices(cpuVertices.size())
        , indexBuffer(std::move(indexBuffer))
    {
        vertexBuffer = std::make_unique<DynamicVertexBuffer>(
            *this->indexBuffer->getGraphicsDeviceProperty(),
            VertexPositionNormalTexture::getVertexDeclarationStatic(),
            vertexCount,
            BufferUsage::WriteOnly);

        for (std::size_t i = 0; i < cpuVertices.size(); ++i)
            gpuVertices[i].TextureCoordinate = cpuVertices[i].TextureCoordinate;
    }

    CpuSkinnedModelPart::~CpuSkinnedModelPart() = default;

    const std::string& CpuSkinnedModelPart::GetTypeName() const
    {
        static const std::string name = "CpuSkinningDataTypes.CpuSkinnedModelPart";
        return name;
    }

    void CpuSkinnedModelPart::SetBones(const std::vector<Matrix>& bones)
    {
        for (int i = 0; i < vertexCount; ++i)
        {
            const std::size_t index = static_cast<std::size_t>(i);
            CpuSkinningHelpers::SkinVertex(
                bones,
                cpuVertices[index].Position,
                cpuVertices[index].Normal,
                cpuVertices[index].BlendIndices,
                cpuVertices[index].BlendWeights,
                gpuVertices[index].Position,
                gpuVertices[index].Normal);
        }

        vertexBuffer->SetData(
            gpuVertices.data(), 0, vertexCount, SetDataOptions::Discard);
    }

    void CpuSkinnedModelPart::Draw()
    {
        GraphicsDevice* graphics = effect->getGraphicsDeviceProperty();

        graphics->setIndicesProperty(indexBuffer.get());
        graphics->SetVertexBuffer(vertexBuffer.get());

        for (EffectPass& pass : effect->getCurrentTechniqueProperty()->getPassesProperty())
        {
            pass.Apply();
            graphics->DrawIndexedPrimitives(
                PrimitiveType::TriangleList, 0, 0, vertexCount, 0, triangleCount);
        }

        graphics->setIndicesProperty(nullptr);
        graphics->SetVertexBuffer(nullptr);
    }
}
