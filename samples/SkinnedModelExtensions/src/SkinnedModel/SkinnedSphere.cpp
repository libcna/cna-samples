// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SkinnedSphere.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "SkinnedSphere.hpp"

namespace SkinnedModel
{
    const std::string& SkinnedSphere::GetTypeName() const
    {
        static const std::string name = "SkinnedModel.SkinnedSphere";
        return name;
    }
}
