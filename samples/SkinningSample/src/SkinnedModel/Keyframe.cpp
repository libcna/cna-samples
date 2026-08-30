// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Keyframe.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Keyframe.hpp"

#include <utility>

namespace SkinnedModel
{
    Keyframe::Keyframe(int bone, System::TimeSpan time, const Matrix& transform)
        : bone(bone), time(std::move(time)), transform(transform)
    {
    }

    const std::string& Keyframe::GetTypeName() const
    {
        static const std::string name = "SkinnedModel.Keyframe";
        return name;
    }
}
