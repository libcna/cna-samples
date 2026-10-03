// SPDX-License-Identifier: MS-PL
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
#include "AvatarBlendedAnimation.hpp"
#include "Microsoft/Xna/Framework/Quaternion.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace AvatarAnimationBlendingSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::GamerServices;

    AvatarBlendedAnimation::AvatarBlendedAnimation(AvatarAnimation* startingAnimation)
        : avatarBones(std::make_shared<std::vector<Matrix>>(AvatarRenderer::BoneCount)),
          boneTransforms(avatarBones), currentAnimation(startingAnimation)
    {
    }

    System::Collections::ObjectModel::ReadOnlyCollection<Matrix> AvatarBlendedAnimation::getBoneTransformsProperty() const
    {
        return boneTransforms;
    }

    AvatarExpression AvatarBlendedAnimation::getExpressionProperty() const
    {
        return currentAnimation->getExpressionProperty();
    }

    void AvatarBlendedAnimation::Update(System::TimeSpan elapsedAnimationTime, bool loop)
    {
        currentAnimation->Update(elapsedAnimationTime, loop);
        if (targetAnimation == nullptr)
        {
            currentAnimation->getBoneTransformsProperty().CopyTo(*avatarBones, 0);
        }
        else
        {
            targetAnimation->Update(elapsedAnimationTime, loop);
            const auto currentBoneTransforms = currentAnimation->getBoneTransformsProperty();
            const auto targetBoneTransforms = targetAnimation->getBoneTransformsProperty();
            blendCurrentTime += elapsedAnimationTime;
            SharpRuntime::Single blendFactor = static_cast<SharpRuntime::Single>(
                blendCurrentTime.getTotalSecondsProperty() / blendTotalTime.getTotalSecondsProperty());
            if (blendFactor >= 1.0f)
            {
                currentAnimation = targetAnimation;
                targetAnimation = nullptr;
                blendFactor = 1.0f;
            }
            for (SharpRuntime::intcs i = 0; i < static_cast<SharpRuntime::intcs>(avatarBones->size()); ++i)
            {
                Quaternion currentRotation = Quaternion::CreateFromRotationMatrix(currentBoneTransforms[i]);
                Quaternion targetRotation = Quaternion::CreateFromRotationMatrix(targetBoneTransforms[i]);
                Quaternion finalRotation;
                Quaternion::Slerp(currentRotation, targetRotation, blendFactor, finalRotation);
                Vector3 currentTranslation = currentBoneTransforms[i].getTranslationProperty();
                Vector3 targetTranslation = targetBoneTransforms[i].getTranslationProperty();
                Vector3 finalTranslation;
                Vector3::Lerp(currentTranslation, targetTranslation, blendFactor, finalTranslation);
                (*avatarBones)[i] = Matrix::CreateFromQuaternion(finalRotation) * Matrix::CreateTranslation(finalTranslation);
            }
        }
    }

    void AvatarBlendedAnimation::Play(AvatarAnimation* nextAnimation)
    {
        if (currentAnimation == nextAnimation) return;
        targetAnimation = nextAnimation;
        targetAnimation->setCurrentPositionProperty(System::TimeSpan::Zero);
        blendCurrentTime = System::TimeSpan::Zero;
    }

    System::TimeSpan AvatarBlendedAnimation::getCurrentPositionProperty() const
    {
        return (targetAnimation != nullptr ? targetAnimation : currentAnimation)->getCurrentPositionProperty();
    }

    void AvatarBlendedAnimation::setCurrentPositionProperty(System::TimeSpan value)
    {
        (targetAnimation != nullptr ? targetAnimation : currentAnimation)->setCurrentPositionProperty(value);
    }

    System::TimeSpan AvatarBlendedAnimation::getLengthProperty() const
    {
        return (targetAnimation != nullptr ? targetAnimation : currentAnimation)->getLengthProperty();
    }
}
