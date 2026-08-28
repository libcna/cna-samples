// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ModelAnimationClip.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ModelAnimationClip.hpp"

#include <utility>

namespace CustomModelAnimation
{
    ModelAnimationClip::ModelAnimationClip(System::TimeSpan duration,
                                           std::vector<std::shared_ptr<ModelKeyframe>> keyframes)
        : duration(duration), keyframes(std::move(keyframes))
    {
    }

    const std::string& ModelAnimationClip::GetTypeName() const
    {
        static const std::string name = "CustomModelAnimation.ModelAnimationClip";
        return name;
    }
}
