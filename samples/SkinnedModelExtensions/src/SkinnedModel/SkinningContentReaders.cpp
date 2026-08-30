// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CNAEXT -- AOT counterpart of the reflection XNA performs over the sample-owned types.
//-----------------------------------------------------------------------------

#include "SkinningContentReaders.hpp"

#include <memory>
#include <string>

#include "AnimationClip.hpp"
#include "Keyframe.hpp"
#include "SkinnedSphere.hpp"
#include "SkinningData.hpp"

#include "CNA/Internal/Xnb/CollectionContentTypeReaders.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"
#include "Microsoft/Xna/Framework/Content/ReflectiveTypeReader.hpp"
#include "System/Object.hpp"

namespace SkinnedModel
{
    using CNA::Internal::Xnb::ArrayReader;
    using CNA::Internal::Xnb::DictionaryReader;
    using CNA::Internal::Xnb::ListReader;
    using Microsoft::Xna::Framework::Content::ContentTypeReaderManager;
    using Microsoft::Xna::Framework::Content::ReflectiveTypeReader;
    using Microsoft::Xna::Framework::Content::ReflectiveTypeReaderBuilder;

    namespace
    {
        const std::string KeyframeTypeName = "SkinnedModel.Keyframe";
        const std::string ClipTypeName = "SkinnedModel.AnimationClip";
        const std::string DataTypeName = "SkinnedModel.SkinningData";
        const std::string SphereTypeName = "SkinnedModel.SkinnedSphere";
    }

    void SkinningContentReaderRegistrationEXT::RegisterEXT()
    {
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
            .Field(&SkinningData::boneIndices)
            .RegisterShared<System::Object>();

        ReflectiveTypeReaderBuilder<SkinnedSphere>(SphereTypeName)
            .Field(&SkinnedSphere::BoneName)
            .Field(&SkinnedSphere::Radius)
            .Field(&SkinnedSphere::Offset)
            .RegisterShared();

        ContentTypeReaderManager::AddTypeCreator(
            "Microsoft.Xna.Framework.Content.ArrayReader`1[[" + SphereTypeName + "]]",
            [] {
                return std::make_unique<ArrayReader<std::shared_ptr<SkinnedSphere>>>(
                    SphereTypeName + "[]",
                    ReflectiveTypeReader<SkinnedSphere>::CanonicalReaderName(SphereTypeName));
            });
    }
}
