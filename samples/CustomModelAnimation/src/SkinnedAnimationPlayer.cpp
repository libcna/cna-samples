// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SkinnedAnimationPlayer.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "SkinnedAnimationPlayer.hpp"

#include "System/Exception.hpp"

namespace CustomModelAnimation
{
    SkinnedAnimationPlayer::SkinnedAnimationPlayer(const std::vector<Matrix>& bindPose,
                                                   const std::vector<Matrix>& inverseBindPose,
                                                   const std::vector<int>& skeletonHierarchy)
    {
        if (bindPose.empty())
            throw System::Exception("Bad arguments to model animation player");

        boneTransforms.resize(bindPose.size());
        worldTransforms.resize(bindPose.size());
        skinTransforms.resize(bindPose.size());

        this->bindPose = bindPose;
        this->inverseBindPose = inverseBindPose;
        this->skeletonHierarchy = skeletonHierarchy;
    }

    const std::string& SkinnedAnimationPlayer::GetTypeName() const
    {
        static const std::string name = "CustomModelAnimation.SkinnedAnimationPlayer";
        return name;
    }

    void SkinnedAnimationPlayer::InitClip()
    {
        boneTransforms = bindPose;
    }

    void SkinnedAnimationPlayer::SetKeyframe(const ModelKeyframe& keyframe)
    {
        boneTransforms[static_cast<std::size_t>(keyframe.getBoneProperty())] =
            keyframe.getTransformProperty();
    }

    void SkinnedAnimationPlayer::OnUpdate()
    {
        if (getCurrentClipProperty() != nullptr)
        {
            // Root bone.
            worldTransforms[0] = boneTransforms[0];
            skinTransforms[0] = inverseBindPose[0] * worldTransforms[0];

            // Child bones.
            for (std::size_t bone = 1; bone < worldTransforms.size(); bone++)
            {
                const std::size_t parentBone =
                    static_cast<std::size_t>(skeletonHierarchy[bone]);

                worldTransforms[bone] = boneTransforms[bone] * worldTransforms[parentBone];
                skinTransforms[bone] = inverseBindPose[bone] * worldTransforms[bone];
            }
        }
    }
}
