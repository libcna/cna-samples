// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AnimationClip.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "AnimationClip.hpp"

#include <utility>

namespace CpuSkinningDataTypes
{
    AnimationClip::AnimationClip(System::TimeSpan duration, KeyframeList keyframes)
        : duration(std::move(duration)), keyframes(std::move(keyframes))
    {
    }

    const std::string& AnimationClip::GetTypeName() const
    {
        static const std::string name = "CpuSkinningDataTypes.AnimationClip";
        return name;
    }
}
