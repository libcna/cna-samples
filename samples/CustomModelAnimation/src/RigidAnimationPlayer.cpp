// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// RigidAnimationPlayer.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "RigidAnimationPlayer.hpp"

#include "System/Exception.hpp"

namespace CustomModelAnimation
{
    RigidAnimationPlayer::RigidAnimationPlayer(int count)
    {
        if (count <= 0)
            throw System::Exception("Bad arguments to model animation player");

        boneTransforms.resize(static_cast<std::size_t>(count));
    }

    const std::string& RigidAnimationPlayer::GetTypeName() const
    {
        static const std::string name = "CustomModelAnimation.RigidAnimationPlayer";
        return name;
    }

    void RigidAnimationPlayer::InitClip()
    {
        for (std::size_t i = 0; i < boneTransforms.size(); i++)
            boneTransforms[i] = Matrix::getIdentityProperty();
    }

    void RigidAnimationPlayer::SetKeyframe(const ModelKeyframe& keyframe)
    {
        boneTransforms[static_cast<std::size_t>(keyframe.getBoneProperty())] =
            keyframe.getTransformProperty();
    }
}
