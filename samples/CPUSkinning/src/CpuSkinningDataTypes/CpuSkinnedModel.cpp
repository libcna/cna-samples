// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CpuSkinnedModel.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CpuSkinnedModel.hpp"

#include <utility>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/IEffectLights.hpp"
#include "Microsoft/Xna/Framework/Graphics/IEffectMatrices.hpp"

namespace CpuSkinningDataTypes
{
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Graphics::IEffectLights;
    using Microsoft::Xna::Framework::Graphics::IEffectMatrices;

    CpuSkinnedModel::CpuSkinnedModel(
        std::vector<std::shared_ptr<CpuSkinnedModelPart>> modelParts,
        std::shared_ptr<SkinningData> skinningData)
        : modelParts(std::make_shared<
              std::vector<std::shared_ptr<CpuSkinnedModelPart>>>(std::move(modelParts)))
        , skinningData(std::move(skinningData))
        , parts(this->modelParts)
    {
    }

    const std::string& CpuSkinnedModel::GetTypeName() const
    {
        static const std::string name = "CpuSkinningDataTypes.CpuSkinnedModel";
        return name;
    }

    void CpuSkinnedModel::SetBones(const std::vector<Matrix>& bones)
    {
        for (const std::shared_ptr<CpuSkinnedModelPart>& part : *modelParts)
            part->SetBones(bones);
    }

    void CpuSkinnedModel::Draw(
        const Matrix& world, const Matrix& view, const Matrix& projection)
    {
        for (const std::shared_ptr<CpuSkinnedModelPart>& part : *modelParts)
        {
            auto* matrices = dynamic_cast<IEffectMatrices*>(part->getEffectProperty());
            if (matrices != nullptr)
            {
                matrices->setWorldProperty(world);
                matrices->setViewProperty(view);
                matrices->setProjectionProperty(projection);
            }

            auto* lights = dynamic_cast<IEffectLights*>(part->getEffectProperty());
            if (lights != nullptr)
                lights->EnableDefaultLighting();

            part->Draw();
        }
    }
}
