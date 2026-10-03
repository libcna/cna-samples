// SPDX-License-Identifier: MS-PL
// Copyright (C) Microsoft Corporation. All rights reserved.
#pragma once
#include "Microsoft/Xna/Framework/GamerServices/AvatarExpression.hpp"
#include "System/TimeSpan.hpp"
namespace CustomAvatarAnimation {
/** @brief One facial expression at a point in the animation. */
struct AvatarExpressionKeyFrame {
    /** @brief Time offset from animation start. */
    System::TimeSpan Time;
    /** @brief Facial expression at the keyframe. */
    Microsoft::Xna::Framework::GamerServices::AvatarExpression Expression;
    /** @brief Default value used when deserializing the struct. */
    AvatarExpressionKeyFrame() = default;
    /** @brief Creates a keyframe. @param time Time. @param expression Expression. */
    AvatarExpressionKeyFrame(System::TimeSpan time, Microsoft::Xna::Framework::GamerServices::AvatarExpression expression)
        : Time(time), Expression(expression) {}
};
}
