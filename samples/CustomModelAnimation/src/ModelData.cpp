// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ModelData.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ModelData.hpp"

#include <utility>

namespace CustomModelAnimation
{
    ModelData::ModelData(ModelAnimationClipDictionary modelAnimationClips,
                         ModelAnimationClipDictionary rootAnimationClips,
                         std::vector<Matrix> bindPose,
                         std::vector<Matrix> inverseBindPose,
                         std::vector<int> skeletonHierarchy)
        : rootAnimationClips(std::move(rootAnimationClips))
        , modelAnimationClips(std::move(modelAnimationClips))
        , bindPose(std::move(bindPose))
        , inverseBindPose(std::move(inverseBindPose))
        , skeletonHierarchy(std::move(skeletonHierarchy))
    {
    }

    const std::string& ModelData::GetTypeName() const
    {
        static const std::string name = "CustomModelAnimation.ModelData";
        return name;
    }
}
