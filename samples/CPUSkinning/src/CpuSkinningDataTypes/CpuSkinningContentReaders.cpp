// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CpuSkinnedModelReader.cs and CNAEXT reader registration
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CpuSkinningContentReaders.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Animation/AnimationClip.hpp"
#include "Animation/Keyframe.hpp"
#include "Animation/SkinningData.hpp"
#include "CpuSkinnedModel.hpp"
#include "CpuSkinnedModelPart.hpp"
#include "CpuVertex.hpp"

#include "CNA/Internal/Xnb/CollectionContentTypeReaders.hpp"
#include "Microsoft/Xna/Framework/Content/ContentLoadException.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"
#include "Microsoft/Xna/Framework/Content/ReflectiveTypeReader.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "System/Object.hpp"

namespace CpuSkinningDataTypes
{
    using CNA::Internal::Xnb::ArrayReader;
    using CNA::Internal::Xnb::DictionaryReader;
    using CNA::Internal::Xnb::ListReader;
    using Microsoft::Xna::Framework::Content::ContentLoadException;
    using Microsoft::Xna::Framework::Content::ContentReader;
    using Microsoft::Xna::Framework::Content::ContentTypeReader;
    using Microsoft::Xna::Framework::Content::ContentTypeReaderManager;
    using Microsoft::Xna::Framework::Content::ReflectiveTypeReader;
    using Microsoft::Xna::Framework::Content::ReflectiveTypeReaderBuilder;
    using Microsoft::Xna::Framework::Graphics::BasicEffect;
    using Microsoft::Xna::Framework::Graphics::Effect;
    using Microsoft::Xna::Framework::Graphics::IndexBuffer;

    namespace
    {
        const std::string ModelTypeName = "CpuSkinningDataTypes.CpuSkinnedModel";
        const std::string ModelPartTypeName = "CpuSkinningDataTypes.CpuSkinnedModelPart";
        const std::string VertexTypeName = "CpuSkinningDataTypes.CpuVertex";
        const std::string KeyframeTypeName = "CpuSkinningDataTypes.Keyframe";
        const std::string ClipTypeName = "CpuSkinningDataTypes.AnimationClip";
        const std::string DataTypeName = "CpuSkinningDataTypes.SkinningData";
    }

    CpuSkinnedModelReader::CpuSkinnedModelReader()
        : ContentTypeReader<std::shared_ptr<CpuSkinnedModel>>(ModelTypeName)
    {
    }

    std::shared_ptr<CpuSkinnedModel> CpuSkinnedModelReader::Read(
        ContentReader& input,
        std::optional<std::shared_ptr<CpuSkinnedModel>> /*existingInstance*/)
    {
        auto modelParts =
            input.ReadObject<std::vector<std::shared_ptr<CpuSkinnedModelPart>>>();

        std::shared_ptr<System::Object> object =
            input.ReadObject<std::shared_ptr<System::Object>>();
        std::shared_ptr<SkinningData> skinningData =
            std::dynamic_pointer_cast<SkinningData>(std::move(object));
        if (!skinningData)
        {
            throw ContentLoadException(
                "'" + input.getAssetNameProperty()
                + "': CpuSkinnedModelReader expected CpuSkinningDataTypes.SkinningData.");
        }

        return std::shared_ptr<CpuSkinnedModel>(
            new CpuSkinnedModel(std::move(modelParts), std::move(skinningData)));
    }

    CpuSkinnedModelPartReader::CpuSkinnedModelPartReader()
        : ContentTypeReader<std::shared_ptr<CpuSkinnedModelPart>>(ModelPartTypeName)
    {
    }

    std::shared_ptr<CpuSkinnedModelPart> CpuSkinnedModelPartReader::Read(
        ContentReader& input,
        std::optional<std::shared_ptr<CpuSkinnedModelPart>> /*existingInstance*/)
    {
        const int triangleCount = input.ReadInt32();
        std::vector<CpuVertex> cpuVertices = input.ReadObject<std::vector<CpuVertex>>();
        std::shared_ptr<IndexBuffer> indexBuffer =
            input.ReadObject<std::shared_ptr<IndexBuffer>>();

        auto modelPart = std::shared_ptr<CpuSkinnedModelPart>(
            new CpuSkinnedModelPart(
                triangleCount, std::move(cpuVertices), std::move(indexBuffer)));

        input.ReadSharedResource<std::shared_ptr<Effect>>(
            [modelPart, assetName = input.getAssetNameProperty()](
                std::shared_ptr<Effect> sharedEffect) {
                modelPart->effect = std::dynamic_pointer_cast<BasicEffect>(
                    std::move(sharedEffect));
                if (!modelPart->effect)
                {
                    throw ContentLoadException(
                        "'" + assetName
                        + "': CpuSkinnedModelPartReader expected a shared BasicEffect.");
                }
            });

        return modelPart;
    }

    void CpuSkinningContentReaderRegistrationEXT::RegisterEXT()
    {
        ContentTypeReaderManager::AddTypeCreator(
            "CpuSkinningDataTypes.CpuSkinnedModelReader",
            [] { return std::make_unique<CpuSkinnedModelReader>(); });
        ContentTypeReaderManager::AddTypeCreator(
            "CpuSkinningDataTypes.CpuSkinnedModelPartReader",
            [] { return std::make_unique<CpuSkinnedModelPartReader>(); });

        ContentTypeReaderManager::AddTypeCreator(
            "Microsoft.Xna.Framework.Content.ListReader`1[[" + ModelPartTypeName + "]]",
            [] {
                return std::make_unique<ListReader<std::shared_ptr<CpuSkinnedModelPart>>>(
                    "System.Collections.Generic.List`1[[" + ModelPartTypeName + "]]",
                    "CpuSkinningDataTypes.CpuSkinnedModelPartReader");
            });

        ReflectiveTypeReaderBuilder<CpuVertex>(VertexTypeName)
            .Field(&CpuVertex::Position)
            .Field(&CpuVertex::Normal)
            .Field(&CpuVertex::TextureCoordinate)
            .Field(&CpuVertex::BlendWeights)
            .Field(&CpuVertex::BlendIndices)
            .Register();

        ContentTypeReaderManager::AddTypeCreator(
            "Microsoft.Xna.Framework.Content.ArrayReader`1[[" + VertexTypeName + "]]",
            [] {
                return std::make_unique<ArrayReader<CpuVertex>>(
                    VertexTypeName + "[]",
                    ReflectiveTypeReader<CpuVertex>::CanonicalReaderName(VertexTypeName));
            });

        ReflectiveTypeReaderBuilder<Keyframe>(KeyframeTypeName)
            .Field(&Keyframe::bone)
            .Field(&Keyframe::time)
            .Field(&Keyframe::transform)
            .RegisterShared();

        ContentTypeReaderManager::AddTypeCreator(
            "Microsoft.Xna.Framework.Content.ListReader`1[[" + KeyframeTypeName + "]]",
            [] {
                return std::make_unique<ListReader<std::shared_ptr<Keyframe>>>(
                    "System.Collections.Generic.List`1[[" + KeyframeTypeName + "]]",
                    ReflectiveTypeReader<Keyframe>::CanonicalReaderName(KeyframeTypeName));
            });

        ReflectiveTypeReaderBuilder<AnimationClip>(ClipTypeName)
            .Field(&AnimationClip::duration)
            .Field(&AnimationClip::keyframes)
            .RegisterShared();

        ContentTypeReaderManager::AddTypeCreator(
            "Microsoft.Xna.Framework.Content.DictionaryReader`2[[System.String],["
                + ClipTypeName + "]]",
            [] {
                return std::make_unique<
                    DictionaryReader<std::string, std::shared_ptr<AnimationClip>>>(
                    "System.Collections.Generic.Dictionary`2[[System.String],["
                        + ClipTypeName + "]]",
                    "Microsoft.Xna.Framework.Content.StringReader",
                    ReflectiveTypeReader<AnimationClip>::CanonicalReaderName(ClipTypeName));
            });

        ReflectiveTypeReaderBuilder<SkinningData>(DataTypeName)
            .Field(&SkinningData::animationClips)
            .Field(&SkinningData::bindPose)
            .Field(&SkinningData::inverseBindPose)
            .Field(&SkinningData::skeletonHierarchy)
            .RegisterShared<System::Object>();
    }
}
