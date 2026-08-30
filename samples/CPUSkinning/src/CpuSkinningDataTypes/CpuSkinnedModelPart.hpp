// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CpuSkinnedModelPart.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "CpuVertex.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionNormalTexture.hpp"
#include "System/Object.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class BasicEffect;
    class DynamicVertexBuffer;
    class IndexBuffer;
}

namespace CpuSkinningDataTypes
{
    class CpuSkinnedModelPartReader;

    /** @brief One indexed, single-material section of a CPU-skinned model. */
    class CpuSkinnedModelPart : public System::Object
    {
    public:
        /** @brief Releases the dynamic vertex buffer and retained shared resources. */
        ~CpuSkinnedModelPart() override;

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `CpuSkinningDataTypes.CpuSkinnedModelPart`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Gets the BasicEffect assigned by the content reader.
         * @return The model part effect.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::BasicEffect*
            getEffectProperty() const { return effect.get(); }

        /**
         * @brief Skins every source vertex and replaces this part's dynamic vertex-buffer data.
         * @param bones Current skin transform for every skeleton bone.
         */
        void SetBones(const std::vector<Microsoft::Xna::Framework::Matrix>& bones);

        /** @brief Draws the indexed triangles once for every pass of the assigned effect. */
        void Draw();

    private:
        friend class CpuSkinnedModelPartReader;

        CpuSkinnedModelPart(
            int triangleCount,
            std::vector<CpuVertex> vertices,
            std::shared_ptr<Microsoft::Xna::Framework::Graphics::IndexBuffer> indexBuffer);

        int triangleCount;
        int vertexCount;
        std::vector<CpuVertex> cpuVertices;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionNormalTexture> gpuVertices;

        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertexBuffer;
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::IndexBuffer> indexBuffer;
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect;
    };
}
