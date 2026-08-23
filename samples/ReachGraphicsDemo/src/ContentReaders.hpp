#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "CNA/Internal/Xnb/CollectionContentTypeReaders.hpp"
#include "Microsoft/Xna/Framework/Content/ContentLoadException.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "SkinningModel.hpp"
#include "Sky.hpp"

namespace ReachGraphicsDemoSample::ContentReaders {

using Microsoft::Xna::Framework::Content::ContentLoadException;
using Microsoft::Xna::Framework::Content::ContentReader;
using Microsoft::Xna::Framework::Content::ContentTypeReader;
using Microsoft::Xna::Framework::Content::ContentTypeReaderBase;
using Microsoft::Xna::Framework::Content::ContentTypeReaderManager;
using Microsoft::Xna::Framework::Graphics::Model;
using Microsoft::Xna::Framework::Graphics::Texture2D;

inline constexpr const char* SkinningDataReaderName =
    "Microsoft.Xna.Framework.Content.ReflectiveReader`1[[SkinnedModel.SkinningData]]";
inline constexpr const char* AnimationClipReaderName =
    "Microsoft.Xna.Framework.Content.ReflectiveReader`1[[SkinnedModel.AnimationClip]]";
inline constexpr const char* KeyframeReaderName =
    "Microsoft.Xna.Framework.Content.ReflectiveReader`1[[SkinnedModel.Keyframe]]";
inline constexpr const char* AnimationClipDictionaryReaderName =
    "Microsoft.Xna.Framework.Content.DictionaryReader`2[[System.String],[SkinnedModel.AnimationClip]]";
inline constexpr const char* KeyframeListReaderName =
    "Microsoft.Xna.Framework.Content.ListReader`1[[SkinnedModel.Keyframe]]";
inline constexpr const char* MatrixListReaderName =
    "Microsoft.Xna.Framework.Content.ListReader`1[[Microsoft.Xna.Framework.Matrix]]";
inline constexpr const char* Int32ListReaderName =
    "Microsoft.Xna.Framework.Content.ListReader`1[[System.Int32]]";
inline constexpr const char* SkyReaderName =
    "Microsoft.Xna.Framework.Content.ReflectiveReader`1[[GeneratedGeometry.Sky]]";

template <typename T>
T ReadMember(ContentReader& input, const char* readerName) {
    std::unique_ptr<ContentTypeReaderBase> reader =
        ContentTypeReaderManager::CreateReader(readerName);
    if (!reader) {
        throw ContentLoadException(
            std::string("ReachGraphicsDemo: content reader is not registered: ") +
            readerName);
    }
    return input.ReadObject<T>(*reader);
}

class KeyframeReader
    : public ContentTypeReader<std::shared_ptr<SkinnedModel::Keyframe>> {
public:
    KeyframeReader()
        : ContentTypeReader<std::shared_ptr<SkinnedModel::Keyframe>>(
              "SkinnedModel.Keyframe") {}

protected:
    std::shared_ptr<SkinnedModel::Keyframe> Read(
        ContentReader& input,
        std::optional<std::shared_ptr<SkinnedModel::Keyframe>>) override {
        int bone = ReadMember<int>(
            input, "Microsoft.Xna.Framework.Content.Int32Reader");
        System::TimeSpan time = ReadMember<System::TimeSpan>(
            input, "Microsoft.Xna.Framework.Content.TimeSpanReader");
        Microsoft::Xna::Framework::Matrix transform =
            ReadMember<Microsoft::Xna::Framework::Matrix>(
                input, "Microsoft.Xna.Framework.Content.MatrixReader");
        return std::make_shared<SkinnedModel::Keyframe>(bone, time, transform);
    }
};

class AnimationClipReader
    : public ContentTypeReader<std::shared_ptr<SkinnedModel::AnimationClip>> {
public:
    AnimationClipReader()
        : ContentTypeReader<std::shared_ptr<SkinnedModel::AnimationClip>>(
              "SkinnedModel.AnimationClip") {}

protected:
    std::shared_ptr<SkinnedModel::AnimationClip> Read(
        ContentReader& input,
        std::optional<std::shared_ptr<SkinnedModel::AnimationClip>>) override {
        System::TimeSpan duration = ReadMember<System::TimeSpan>(
            input, "Microsoft.Xna.Framework.Content.TimeSpanReader");
        auto keyframes =
            input.ReadObject<std::vector<std::shared_ptr<SkinnedModel::Keyframe>>>();
        return std::make_shared<SkinnedModel::AnimationClip>(
            duration, std::move(keyframes));
    }
};

class SkinningDataReader
    : public ContentTypeReader<std::shared_ptr<System::Object>> {
public:
    SkinningDataReader()
        : ContentTypeReader<std::shared_ptr<System::Object>>(
              "SkinnedModel.SkinningData") {}

protected:
    std::shared_ptr<System::Object> Read(
        ContentReader& input,
        std::optional<std::shared_ptr<System::Object>>) override {
        auto animationClips =
            input.ReadObject<SkinnedModel::SkinningData::AnimationClipDictionary>();
        auto bindPose =
            input.ReadObject<std::vector<Microsoft::Xna::Framework::Matrix>>();
        auto inverseBindPose =
            input.ReadObject<std::vector<Microsoft::Xna::Framework::Matrix>>();
        auto skeletonHierarchy = input.ReadObject<std::vector<int>>();

        return std::make_shared<SkinnedModel::SkinningData>(
            std::move(animationClips), std::move(bindPose),
            std::move(inverseBindPose), std::move(skeletonHierarchy));
    }
};

class AnimationClipDictionaryReader
    : public ContentTypeReader<SkinnedModel::SkinningData::AnimationClipDictionary> {
public:
    AnimationClipDictionaryReader()
        : ContentTypeReader<SkinnedModel::SkinningData::AnimationClipDictionary>(
              "System.Collections.Generic.Dictionary<System.String,SkinnedModel.AnimationClip>") {}

    bool getCanDeserializeIntoExistingObjectProperty() const override { return true; }

protected:
    SkinnedModel::SkinningData::AnimationClipDictionary Read(
        ContentReader& input,
        std::optional<SkinnedModel::SkinningData::AnimationClipDictionary>
            existingInstance) override {
        int count = input.ReadInt32();
        input.CheckCollectionElementCount(
            count, "Dictionary<System.String,SkinnedModel.AnimationClip>");

        auto dictionary = existingInstance.value_or(
            SkinnedModel::SkinningData::AnimationClipDictionary{});
        dictionary.clear();
        for (int i = 0; i < count; ++i) {
            std::string key = input.ReadObject<std::string>();
            auto value = input.ReadObject<std::shared_ptr<SkinnedModel::AnimationClip>>();
            dictionary.emplace(std::move(key), std::move(value));
        }
        return dictionary;
    }
};

class SkyReader : public ContentTypeReader<GeneratedGeometry::Sky> {
public:
    SkyReader()
        : ContentTypeReader<GeneratedGeometry::Sky>("GeneratedGeometry.Sky") {}

protected:
    GeneratedGeometry::Sky Read(
        ContentReader& input, std::optional<GeneratedGeometry::Sky>) override {
        Model model = input.ReadObject<Model>();
        Texture2D texture = input.ReadObject<Texture2D>();
        return GeneratedGeometry::Sky(std::move(model), std::move(texture));
    }
};

inline void Register() {
    ContentTypeReaderManager::AddTypeCreator(
        KeyframeReaderName,
        [] { return std::make_unique<KeyframeReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        AnimationClipReaderName,
        [] { return std::make_unique<AnimationClipReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        SkinningDataReaderName,
        [] { return std::make_unique<SkinningDataReader>(); });
    ContentTypeReaderManager::AddTypeCreator(
        AnimationClipDictionaryReaderName,
        [] {
            return std::make_unique<AnimationClipDictionaryReader>();
        });
    ContentTypeReaderManager::AddTypeCreator(
        KeyframeListReaderName,
        [] {
            return std::make_unique<CNA::Internal::Xnb::ListReader<
                std::shared_ptr<SkinnedModel::Keyframe>>>(
                    "System.Collections.Generic.List<SkinnedModel.Keyframe>",
                    KeyframeReaderName);
        });
    ContentTypeReaderManager::AddTypeCreator(
        MatrixListReaderName,
        [] {
            return std::make_unique<CNA::Internal::Xnb::ListReader<
                Microsoft::Xna::Framework::Matrix>>(
                    "System.Collections.Generic.List<Microsoft.Xna.Framework.Matrix>",
                    "Microsoft.Xna.Framework.Content.MatrixReader");
        });
    ContentTypeReaderManager::AddTypeCreator(
        Int32ListReaderName,
        [] {
            return std::make_unique<CNA::Internal::Xnb::ListReader<int>>(
                "System.Collections.Generic.List<System.Int32>",
                "Microsoft.Xna.Framework.Content.Int32Reader");
        });
    ContentTypeReaderManager::AddTypeCreator(
        SkyReaderName,
        [] { return std::make_unique<SkyReader>(); });
}

} // namespace ReachGraphicsDemoSample::ContentReaders
