// SPDX-License-Identifier: MS-PL
// Copyright (C) Microsoft Corporation. All rights reserved.
#pragma once
#include "CustomAvatarAnimationData.hpp"
#include "Microsoft/Xna/Framework/GamerServices/IAvatarAnimation.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarRenderer.hpp"
namespace CustomAvatarAnimation {
/** @brief Plays the original step keyframes forward or backward with optional looping. */
class CustomAvatarAnimationPlayer : public CustomAvatarAnimationData,
    public Microsoft::Xna::Framework::GamerServices::IAvatarAnimation {
public:
    /** @brief Creates a player. @param name Name. @param length Duration. @param keyframes Bone keys.
     * @param expressionKeyframes Nullable expression keys. */
    CustomAvatarAnimationPlayer(std::string name, System::TimeSpan length, std::shared_ptr<BoneKeys> keyframes,
        std::shared_ptr<ExpressionKeys> expressionKeyframes);
    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;
    /** @brief Gets playback position. @return Position. */
    [[nodiscard]] System::TimeSpan getCurrentPositionProperty() const override { return currentPosition; }
    /** @brief Seeks and resets key cursors. @param value Position. */
    void setCurrentPositionProperty(System::TimeSpan value) override;
    /** @brief Gets duration. @return Duration. */
    [[nodiscard]] System::TimeSpan getLengthProperty() const override { return CustomAvatarAnimationData::getLengthProperty(); }
    /** @brief Gets the live bone array. @return Read-only view. */
    [[nodiscard]] System::Collections::ObjectModel::ReadOnlyCollection<Microsoft::Xna::Framework::Matrix> getBoneTransformsProperty() const override { return boneTransforms; }
    /** @brief Gets the current expression. @return Expression. */
    [[nodiscard]] Microsoft::Xna::Framework::GamerServices::AvatarExpression getExpressionProperty() const override { return avatarExpression; }
    /** @brief Advances position and keys. @param timeSpan Elapsed time. @param loop Loop. */
    void Update(System::TimeSpan timeSpan, bool loop) override;
private:
    SharpRuntime::intcs currentKeyframe = 0;
    SharpRuntime::intcs currentExpressionKeyframe = 0;
    System::TimeSpan currentPosition;
    std::shared_ptr<std::vector<Microsoft::Xna::Framework::Matrix>> avatarBoneTransforms;
    System::Collections::ObjectModel::ReadOnlyCollection<Microsoft::Xna::Framework::Matrix> boneTransforms;
    Microsoft::Xna::Framework::GamerServices::AvatarExpression avatarExpression;
    void UpdateBoneTransforms(bool playingForward);
    void UpdateAvatarExpression(bool playingForward);
};
}
