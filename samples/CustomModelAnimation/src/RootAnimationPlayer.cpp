// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// RootAnimationPlayer.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "RootAnimationPlayer.hpp"

namespace CustomModelAnimation
{
    const std::string& RootAnimationPlayer::GetTypeName() const
    {
        static const std::string name = "CustomModelAnimation.RootAnimationPlayer";
        return name;
    }

    void RootAnimationPlayer::InitClip()
    {
        currentTransform = Matrix::getIdentityProperty();
    }

    void RootAnimationPlayer::SetKeyframe(const ModelKeyframe& keyframe)
    {
        currentTransform = keyframe.getTransformProperty();
    }
}
