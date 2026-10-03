// SPDX-License-Identifier: MS-PL
// Copyright (C) Microsoft Corporation. All rights reserved.
#pragma once
#include "AvatarKeyframe.hpp"
#include "AvatarExpressionKeyframe.hpp"
#include "System/Collections/Generic/List.hpp"
#include "System/Object.hpp"
#include <memory>
namespace CustomAvatarAnimation {
class CustomAvatarAnimationContentReaderRegistrationEXT;
/** @brief Serialized custom-animation name, duration, bone keys and expression keys. */
class CustomAvatarAnimationData : public System::Object {
public:
    using BoneKeys = System::Collections::Generic::List<AvatarKeyFrame>;
    using ExpressionKeys = System::Collections::Generic::List<AvatarExpressionKeyFrame>;
    /** @brief Default construction used by the AOT counterpart of XNB deserialization. */
    CNAEXT CustomAvatarAnimationData() = default;
    /** @brief Creates data with the original validation. @param name Name. @param length Duration.
     * @param keyframes Sorted bone keys. @param expressionKeyframes Optional expression keys. */
    CustomAvatarAnimationData(std::string name, System::TimeSpan length, std::shared_ptr<BoneKeys> keyframes,
        std::shared_ptr<ExpressionKeys> expressionKeyframes);
    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;
    /** @brief Gets the name. @return Name. */
    [[nodiscard]] const std::string& getNameProperty() const { return name; }
    /** @brief Gets the duration. @return Duration. */
    [[nodiscard]] System::TimeSpan getLengthProperty() const { return length; }
    /** @brief Gets the shared bone key list. @return List. */
    [[nodiscard]] std::shared_ptr<BoneKeys> getKeyframesProperty() const { return keyframes; }
    /** @brief Gets the nullable expression key list. @return List or null. */
    [[nodiscard]] std::shared_ptr<ExpressionKeys> getExpressionKeyframesProperty() const { return expressionKeyframes; }
private:
    std::string name;
    System::TimeSpan length;
    std::shared_ptr<BoneKeys> keyframes;
    std::shared_ptr<ExpressionKeys> expressionKeyframes;
    friend class CustomAvatarAnimationContentReaderRegistrationEXT;
};
}
