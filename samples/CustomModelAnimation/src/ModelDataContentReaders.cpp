// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CNAEXT -- the C++ counterpart of the reflection XNA does for the game here. Not upstream.
//-----------------------------------------------------------------------------

#include "ModelDataContentReaders.hpp"

#include <memory>
#include <string>

#include "ModelAnimationClip.hpp"
#include "ModelData.hpp"
#include "ModelKeyframe.hpp"

#include "CNA/Internal/Xnb/CollectionContentTypeReaders.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"
#include "Microsoft/Xna/Framework/Content/ReflectiveTypeReader.hpp"
#include "System/Object.hpp"

namespace CustomModelAnimation
{
    using CNA::Internal::Xnb::DictionaryReader;
    using CNA::Internal::Xnb::ListReader;
    using Microsoft::Xna::Framework::Content::ContentTypeReaderManager;
    using Microsoft::Xna::Framework::Content::ReflectiveTypeReader;
    using Microsoft::Xna::Framework::Content::ReflectiveTypeReaderBuilder;

    namespace
    {
        const std::string KeyframeTypeName = "CustomModelAnimation.ModelKeyframe";
        const std::string ClipTypeName = "CustomModelAnimation.ModelAnimationClip";
        const std::string DataTypeName = "CustomModelAnimation.ModelData";
    }

    void ModelContentReaderRegistrationEXT::RegisterEXT()
    {
        // Each field list is in the order IntermediateSerializer wrote it -- the serialized
        // properties in declaration order -- and each of the three types is a C# class, so all
        // three register the reference shape: the .xnb writes the 1-based reader index in front of
        // a reference type, and only the shared_ptr-shaped reader consumes it.
        ReflectiveTypeReaderBuilder<ModelKeyframe>(KeyframeTypeName)
            .Field(&ModelKeyframe::bone)
            .Field(&ModelKeyframe::time)
            .Field(&ModelKeyframe::transform)
            .RegisterShared();

        ContentTypeReaderManager::AddTypeCreator(
            "Microsoft.Xna.Framework.Content.ListReader`1[[" + KeyframeTypeName + "]]",
            [] {
                return std::make_unique<ListReader<std::shared_ptr<ModelKeyframe>>>(
                    "System.Collections.Generic.List`1[[" + KeyframeTypeName + "]]",
                    ReflectiveTypeReader<ModelKeyframe>::CanonicalReaderName(KeyframeTypeName));
            });

        ReflectiveTypeReaderBuilder<ModelAnimationClip>(ClipTypeName)
            .Field(&ModelAnimationClip::duration)
            .Field(&ModelAnimationClip::keyframes)
            .RegisterShared();

        ContentTypeReaderManager::AddTypeCreator(
            "Microsoft.Xna.Framework.Content.DictionaryReader`2[[System.String],["
                + ClipTypeName + "]]",
            [] {
                return std::make_unique<
                    DictionaryReader<std::string, std::shared_ptr<ModelAnimationClip>>>(
                    "System.Collections.Generic.Dictionary`2[[System.String],["
                        + ClipTypeName + "]]",
                    "Microsoft.Xna.Framework.Content.StringReader",
                    ReflectiveTypeReader<ModelAnimationClip>::CanonicalReaderName(ClipTypeName));
            });

        // ModelData rides on Model.Tag, which ModelReader stores as a System::Object*, so this one
        // hands back a pointer to that base rather than to itself.
        ReflectiveTypeReaderBuilder<ModelData>(DataTypeName)
            .Field(&ModelData::rootAnimationClips)
            .Field(&ModelData::modelAnimationClips)
            .Field(&ModelData::bindPose)
            .Field(&ModelData::inverseBindPose)
            .Field(&ModelData::skeletonHierarchy)
            .RegisterShared<System::Object>();
    }
}
