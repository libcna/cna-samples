// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Distorter.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Distorter.hpp"

namespace DistortionSample
{
    String Distorter::ToString() const
    {
        String output =
            DistortionComponent::GetDistortionTechniqueFriendlyName(Technique) +
            " (" + ModelName + ")";
        if (DistortionBlur)
        {
            output += ", Blurred";
        }
        return output;
    }
}
