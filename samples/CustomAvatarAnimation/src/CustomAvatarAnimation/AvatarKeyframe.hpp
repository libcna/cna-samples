// SPDX-License-Identifier: MS-PL
// Copyright (C) Microsoft Corporation. All rights reserved.
#pragma once
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "System/TimeSpan.hpp"
namespace CustomAvatarAnimation {
/** @brief One bone transform at a point in the animation. */
struct AvatarKeyFrame {
    /** @brief Target bone index. */
    SharpRuntime::intcs Bone = 0;
    /** @brief Time offset from animation start. */
    System::TimeSpan Time;
    /** @brief Bone transform at the keyframe. */
    Microsoft::Xna::Framework::Matrix Transform;
    /** @brief Default value used when deserializing the struct. */
    AvatarKeyFrame() = default;
    /** @brief Creates a keyframe. @param bone Bone index. @param time Time. @param transform Transform. */
    AvatarKeyFrame(SharpRuntime::intcs bone, System::TimeSpan time, Microsoft::Xna::Framework::Matrix transform)
        : Bone(bone), Time(time), Transform(transform) {}
};
}
