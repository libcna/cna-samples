// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SkinningData.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "SkinningData.hpp"

#include <utility>

namespace CpuSkinningDataTypes
{
    SkinningData::SkinningData(AnimationClipDictionary animationClips,
                               std::vector<Matrix> bindPose,
                               std::vector<Matrix> inverseBindPose,
                               std::vector<int> skeletonHierarchy)
        : animationClips(std::move(animationClips))
        , bindPose(std::move(bindPose))
        , inverseBindPose(std::move(inverseBindPose))
        , skeletonHierarchy(std::move(skeletonHierarchy))
    {
    }

    const std::string& SkinningData::GetTypeName() const
    {
        static const std::string name = "CpuSkinningDataTypes.SkinningData";
        return name;
    }
}
