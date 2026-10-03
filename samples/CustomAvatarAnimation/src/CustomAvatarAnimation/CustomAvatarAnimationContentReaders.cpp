// SPDX-License-Identifier: MS-PL
// AOT counterpart of the original XNA reflective type graph; no alternate asset format.
#include "CustomAvatarAnimationContentReaders.hpp"
#include "CustomAvatarAnimationData.hpp"
#include "CNA/Internal/Xnb/CollectionContentTypeReaders.hpp"
#include "Microsoft/Xna/Framework/Content/ReflectiveTypeReader.hpp"
#include <any>
namespace CustomAvatarAnimation {
using namespace Microsoft::Xna::Framework::Content;
using namespace Microsoft::Xna::Framework::GamerServices;
using CNA::Internal::Xnb::ListReader;
namespace {
const std::string BoneType = "CustomAvatarAnimation.AvatarKeyFrame";
const std::string ExpressionType = "CustomAvatarAnimation.AvatarExpressionKeyFrame";
const std::string FaceType = "Microsoft.Xna.Framework.GamerServices.AvatarExpression";
template<class T> void RegisterList(const std::string& name) {
    ContentTypeReaderManager::AddTypeCreator("Microsoft.Xna.Framework.Content.ListReader`1[["+name+"]]",[name]{
        return std::make_unique<ListReader<T>>("System.Collections.Generic.List`1[["+name+"]]",CanonicalReflectiveReaderNameEXT(name));
    });
}
template<class T> std::shared_ptr<System::Collections::Generic::List<T>> ReadNullableList(ContentReader& input) {
    auto value = input.ReadObject();
    if (!value.has_value()) return nullptr;
    auto result = std::make_shared<System::Collections::Generic::List<T>>();
    for (const auto& item : std::any_cast<std::vector<T>>(std::move(value))) result->Add(item);
    return result;
}
template<class T> void RegisterEnum(const std::string& name) {
    ContentTypeReaderManager::AddTypeCreator(EnumTypeReader<T>::CanonicalReaderName(name),[name]{return std::make_unique<EnumTypeReader<T>>(name);});
}
}
void CustomAvatarAnimationContentReaderRegistrationEXT::RegisterEXT() {
    RegisterEnum<AvatarMouth>("Microsoft.Xna.Framework.GamerServices.AvatarMouth");
    RegisterEnum<AvatarEye>("Microsoft.Xna.Framework.GamerServices.AvatarEye");
    RegisterEnum<AvatarEyebrow>("Microsoft.Xna.Framework.GamerServices.AvatarEyebrow");
    ReflectiveTypeReaderBuilder<AvatarExpression>(FaceType)
        .Custom([](AvatarExpression& e,ContentReader& r){e.setMouthProperty(static_cast<AvatarMouth>(r.ReadInt32()));})
        .Custom([](AvatarExpression& e,ContentReader& r){e.setLeftEyeProperty(static_cast<AvatarEye>(r.ReadInt32()));})
        .Custom([](AvatarExpression& e,ContentReader& r){e.setRightEyeProperty(static_cast<AvatarEye>(r.ReadInt32()));})
        .Custom([](AvatarExpression& e,ContentReader& r){e.setLeftEyebrowProperty(static_cast<AvatarEyebrow>(r.ReadInt32()));})
        .Custom([](AvatarExpression& e,ContentReader& r){e.setRightEyebrowProperty(static_cast<AvatarEyebrow>(r.ReadInt32()));})
        .Register();
    ReflectiveTypeReaderBuilder<AvatarKeyFrame>(BoneType)
        .Field(&AvatarKeyFrame::Bone).Field(&AvatarKeyFrame::Time).Field(&AvatarKeyFrame::Transform).Register();
    ReflectiveTypeReaderBuilder<AvatarExpressionKeyFrame>(ExpressionType)
        .Field(&AvatarExpressionKeyFrame::Time)
        .Custom([](AvatarExpressionKeyFrame& key,ContentReader& input){
            auto reader=ContentTypeReaderManager::CreateReader(CanonicalReflectiveReaderNameEXT(FaceType));
            key.Expression=input.ReadRawObject<AvatarExpression>(*reader);
        }).Register();
    RegisterList<AvatarKeyFrame>(BoneType);RegisterList<AvatarExpressionKeyFrame>(ExpressionType);
    ReflectiveTypeReaderBuilder<CustomAvatarAnimationData>("CustomAvatarAnimation.CustomAvatarAnimationData")
        .Field(&CustomAvatarAnimationData::name).Field(&CustomAvatarAnimationData::length)
        .Custom([](CustomAvatarAnimationData& data,ContentReader& input){data.keyframes=ReadNullableList<AvatarKeyFrame>(input);})
        .Custom([](CustomAvatarAnimationData& data,ContentReader& input){data.expressionKeyframes=ReadNullableList<AvatarExpressionKeyFrame>(input);})
        .RegisterShared();
}
}
