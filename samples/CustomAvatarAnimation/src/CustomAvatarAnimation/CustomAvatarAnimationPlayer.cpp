// SPDX-License-Identifier: MS-PL
// Copyright (C) Microsoft Corporation. All rights reserved.
#include "CustomAvatarAnimationPlayer.hpp"
#include "System/NullReferenceException.hpp"
namespace CustomAvatarAnimation {
using Microsoft::Xna::Framework::Matrix;
CustomAvatarAnimationPlayer::CustomAvatarAnimationPlayer(std::string name, System::TimeSpan length,
    std::shared_ptr<BoneKeys> keyframes, std::shared_ptr<ExpressionKeys> expressionKeyframes)
    : CustomAvatarAnimationData(std::move(name), length, std::move(keyframes), std::move(expressionKeyframes)),
      avatarBoneTransforms(std::make_shared<std::vector<Matrix>>(
          Microsoft::Xna::Framework::GamerServices::AvatarRenderer::BoneCount, Matrix::getIdentityProperty())),
      boneTransforms(avatarBoneTransforms) {
    Update(System::TimeSpan::Zero, false);
}
const std::string& CustomAvatarAnimationPlayer::GetTypeName() const {
    static const std::string name = "CustomAvatarAnimation.CustomAvatarAnimationPlayer"; return name;
}
void CustomAvatarAnimationPlayer::setCurrentPositionProperty(System::TimeSpan value) {
    currentPosition = value; currentKeyframe = 0; currentExpressionKeyframe = 0;
    Update(System::TimeSpan::Zero, false);
}
void CustomAvatarAnimationPlayer::Update(System::TimeSpan timeSpan, bool loop) {
    currentPosition += timeSpan;
    if (currentPosition > getLengthProperty()) {
        if (loop) {
            while (currentPosition > getLengthProperty()) currentPosition -= getLengthProperty();
            currentKeyframe = 0; currentExpressionKeyframe = 0;
        } else currentPosition = getLengthProperty();
    } else if (currentPosition < System::TimeSpan::Zero) {
        if (loop) {
            while (currentPosition < System::TimeSpan::Zero) currentPosition += getLengthProperty();
            currentKeyframe = getKeyframesProperty()->getCountProperty() - 1;
            if (!getExpressionKeyframesProperty()) throw System::NullReferenceException();
            currentExpressionKeyframe = getExpressionKeyframesProperty()->getCountProperty() - 1;
        } else currentPosition = System::TimeSpan::Zero;
    }
    UpdateBoneTransforms(timeSpan >= System::TimeSpan::Zero);
    UpdateAvatarExpression(timeSpan >= System::TimeSpan::Zero);
}
void CustomAvatarAnimationPlayer::UpdateBoneTransforms(bool playingForward) {
    const auto keys = getKeyframesProperty();
    if (playingForward) {
        while (currentKeyframe < keys->getCountProperty()) {
            const AvatarKeyFrame keyframe = (*keys)[currentKeyframe];
            if (keyframe.Time >= currentPosition) break;
            avatarBoneTransforms->at(keyframe.Bone) = keyframe.Transform;
            currentKeyframe++;
        }
    } else {
        while (currentKeyframe >= 0) {
            const AvatarKeyFrame keyframe = (*keys)[currentKeyframe];
            if (keyframe.Time <= currentPosition) break;
            avatarBoneTransforms->at(keyframe.Bone) = keyframe.Transform;
            currentKeyframe--;
        }
    }
}
void CustomAvatarAnimationPlayer::UpdateAvatarExpression(bool playingForward) {
    const auto keys = getExpressionKeyframesProperty();
    if (!keys || keys->getCountProperty() == 0) return;
    if (playingForward) {
        while (currentExpressionKeyframe < keys->getCountProperty()) {
            const AvatarExpressionKeyFrame keyframe = (*keys)[currentExpressionKeyframe];
            if (keyframe.Time >= currentPosition) break;
            avatarExpression = keyframe.Expression;
            currentExpressionKeyframe++;
        }
    } else {
        while (currentExpressionKeyframe >= 0) {
            const AvatarExpressionKeyFrame keyframe = (*keys)[currentExpressionKeyframe];
            if (keyframe.Time <= currentPosition) break;
            avatarExpression = keyframe.Expression;
            currentExpressionKeyframe--;
        }
    }
}
}
