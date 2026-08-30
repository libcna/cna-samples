// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AnimationPlayer.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "AnimationPlayer.hpp"

#include "System/ArgumentOutOfRangeException.hpp"
#include "System/InvalidOperationException.hpp"

namespace SkinnedModel
{
    AnimationPlayer::AnimationPlayer(const SkinningData& skinningData)
        : boneTransforms(skinningData.getBindPoseProperty().size())
        , worldTransforms(skinningData.getBindPoseProperty().size())
        , skinTransforms(skinningData.getBindPoseProperty().size())
        , skinningData(&skinningData)
    {
    }

    const std::string& AnimationPlayer::GetTypeName() const
    {
        static const std::string name = "SkinnedModel.AnimationPlayer";
        return name;
    }

    void AnimationPlayer::StartClip(const AnimationClip& clip)
    {
        currentClip = &clip;
        currentTime = System::TimeSpan::Zero;
        currentKeyframe = 0;
        boneTransforms = skinningData->getBindPoseProperty();
    }

    void AnimationPlayer::Update(System::TimeSpan time, bool relativeToCurrentTime,
                                 const Matrix& rootTransform)
    {
        UpdateBoneTransforms(time, relativeToCurrentTime);
        UpdateWorldTransforms(rootTransform, boneTransforms);
        UpdateSkinTransforms();
    }

    void AnimationPlayer::UpdateBoneTransforms(System::TimeSpan time,
                                               bool relativeToCurrentTime)
    {
        if (currentClip == nullptr)
        {
            throw System::InvalidOperationException(
                "AnimationPlayer.Update was called before StartClip");
        }

        if (relativeToCurrentTime)
        {
            time += currentTime;

            while (time >= currentClip->getDurationProperty())
                time -= currentClip->getDurationProperty();
        }

        if (time < System::TimeSpan::Zero || time >= currentClip->getDurationProperty())
            throw System::ArgumentOutOfRangeException("time");

        if (time < currentTime)
        {
            currentKeyframe = 0;
            boneTransforms = skinningData->getBindPoseProperty();
        }

        currentTime = time;

        const KeyframeList& keyframes = currentClip->getKeyframesProperty();
        while (currentKeyframe < static_cast<int>(keyframes.size()))
        {
            const Keyframe& keyframe = *keyframes[static_cast<std::size_t>(currentKeyframe)];
            if (keyframe.getTimeProperty() > currentTime)
                break;

            boneTransforms[static_cast<std::size_t>(keyframe.getBoneProperty())] =
                keyframe.getTransformProperty();
            ++currentKeyframe;
        }
    }

    void AnimationPlayer::UpdateWorldTransforms(
        const Matrix& rootTransform, const std::vector<Matrix>& transforms)
    {
        worldTransforms[0] = transforms[0] * rootTransform;

        for (std::size_t bone = 1; bone < worldTransforms.size(); ++bone)
        {
            const std::size_t parentBone = static_cast<std::size_t>(
                skinningData->getSkeletonHierarchyProperty()[bone]);
            worldTransforms[bone] = transforms[bone] * worldTransforms[parentBone];
        }
    }

    void AnimationPlayer::UpdateSkinTransforms()
    {
        const std::vector<Matrix>& inverseBindPose = skinningData->getInverseBindPoseProperty();
        for (std::size_t bone = 0; bone < skinTransforms.size(); ++bone)
            skinTransforms[bone] = inverseBindPose[bone] * worldTransforms[bone];
    }
}
