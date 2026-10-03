// SPDX-License-Identifier: MS-PL
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
#pragma once

#include "Microsoft/Xna/Framework/GamerServices/AvatarAnimation.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarRenderer.hpp"
#include <memory>
#include <vector>

namespace AvatarAnimationBlendingSample
{
    /** @brief Blends an avatar's current and target animations over a quarter second. */
    class AvatarBlendedAnimation : public Microsoft::Xna::Framework::GamerServices::IAvatarAnimation
    {
    public:
        /** @brief Gets the current blended bones. @return The live bone collection. */
        [[nodiscard]] System::Collections::ObjectModel::ReadOnlyCollection<Microsoft::Xna::Framework::Matrix>
        getBoneTransformsProperty() const override;

        /** @brief Gets the current animation's unblended expression. @return Facial expression. */
        [[nodiscard]] Microsoft::Xna::Framework::GamerServices::AvatarExpression getExpressionProperty() const override;

        /** @brief Starts with the supplied animation. @param startingAnimation First animation to play. */
        explicit AvatarBlendedAnimation(Microsoft::Xna::Framework::GamerServices::AvatarAnimation* startingAnimation);

        /**
         * @brief Advances both animations and blends the bone rotations and translations.
         * @param elapsedAnimationTime Time since the last update.
         * @param loop Whether the animation loops.
         */
        void Update(System::TimeSpan elapsedAnimationTime, bool loop) override;

        /** @brief Starts blending to a new animation. @param nextAnimation Next animation to play. */
        void Play(Microsoft::Xna::Framework::GamerServices::AvatarAnimation* nextAnimation);

        /** @brief Gets the target's position while blending, otherwise the current one. @return Playback position. */
        [[nodiscard]] System::TimeSpan getCurrentPositionProperty() const override;
        /** @brief Sets the target's position while blending. @param value New playback position. */
        void setCurrentPositionProperty(System::TimeSpan value) override;
        /** @brief Gets the target's length while blending. @return Animation length. */
        [[nodiscard]] System::TimeSpan getLengthProperty() const override;

    private:
        std::shared_ptr<std::vector<Microsoft::Xna::Framework::Matrix>> avatarBones;
        System::Collections::ObjectModel::ReadOnlyCollection<Microsoft::Xna::Framework::Matrix> boneTransforms;
        Microsoft::Xna::Framework::GamerServices::AvatarAnimation* currentAnimation;
        Microsoft::Xna::Framework::GamerServices::AvatarAnimation* targetAnimation = nullptr;
        System::TimeSpan blendTotalTime{0, 0, 0, 0, 250};
        System::TimeSpan blendCurrentTime;
    };
}
