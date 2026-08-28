// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ModelKeyframe.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ModelKeyframe.hpp"

namespace CustomModelAnimation
{
    ModelKeyframe::ModelKeyframe(int bone, System::TimeSpan time, const Matrix& transform)
        : bone(bone), time(time), transform(transform)
    {
    }

    const std::string& ModelKeyframe::GetTypeName() const
    {
        static const std::string name = "CustomModelAnimation.ModelKeyframe";
        return name;
    }
}
