// SPDX-License-Identifier: MS-PL
// Copyright (C) Microsoft Corporation. All rights reserved.
#include "CustomAvatarAnimationData.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/ArgumentOutOfRangeException.hpp"
namespace CustomAvatarAnimation {
CustomAvatarAnimationData::CustomAvatarAnimationData(std::string nameValue, System::TimeSpan lengthValue,
    std::shared_ptr<BoneKeys> keys, std::shared_ptr<ExpressionKeys> expressions) {
    if (nameValue.empty()) throw System::ArgumentNullException("name");
    if (lengthValue <= System::TimeSpan::Zero)
        throw System::ArgumentOutOfRangeException("length", "The length of the animation cannot be zero.");
    if (!keys || keys->getCountProperty() <= 0) throw System::ArgumentNullException("keyframes");
    name = std::move(nameValue); length = lengthValue; keyframes = std::move(keys); expressionKeyframes = std::move(expressions);
}
const std::string& CustomAvatarAnimationData::GetTypeName() const {
    static const std::string name = "CustomAvatarAnimation.CustomAvatarAnimationData"; return name;
}
}
