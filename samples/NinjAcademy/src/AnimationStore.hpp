// SPDX-License-Identifier: MS-PL
#pragma once

// AnimationStore.hpp — C++ port of NinjAcademyCommonTypes/AnimationStore.cs
// (XNA 4.0 NinjAcademy sample). Stores a collection of animations, keyed by
// alias.
//
// The original loads this through the custom AnimationProcessor. The C++ port
// registers the same reflective XNB shape in ContentReaders.cpp.

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "System/Collections/Generic/KeyNotFoundException.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"

#include "Animation.hpp"

namespace NinjAcademy {

using Microsoft::Xna::Framework::Content::ContentManager;

struct NinjAcademyContentReaderRegistrationEXT;

// Port of NinjAcademyCommonTypes/AnimationStore.cs.
class AnimationStore {
public:
    using AnimationDictionary = std::unordered_map<std::string, std::shared_ptr<Animation>>;

    void setAnimationsProperty(AnimationDictionary value) { animations_ = std::move(value); }

    // Returns an animation from the store which has the specified alias.
    Animation& operator[](const std::string& animationAlias) {
        auto it = animations_.find(animationAlias);
        if (it == animations_.end())
            throw System::Collections::Generic::KeyNotFoundException(
                "The given key was not present in the dictionary.");
        return *it->second;
    }

    // Initializes all contained animations by loading their sprite sheets.
    void Initialize(ContentManager& contentManager) {
        for (auto& [alias, animation] : animations_)
            animation->LoadSheet(contentManager);
    }

private:
    friend struct NinjAcademyContentReaderRegistrationEXT;
    AnimationDictionary animations_;
};

} // namespace NinjAcademy
