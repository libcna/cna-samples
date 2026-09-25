// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AssemblyInfo.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CNA/AssemblyInfo.hpp"
#include "CNA/ProjectGraphicsProfile.hpp"

namespace
{
    // [assembly: AssemblyTitle("CustomModelAnimationSample")]
    //
    // XNA takes the game window's title from this attribute; here it matches the assembly name.
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"CustomModelAnimationSample"};

    // The original Windows and Xbox projects both declare <XnaProfile>HiDef</XnaProfile>.
    const CNA::ProjectGraphicsProfileEXT projectGraphicsProfile{
        Microsoft::Xna::Framework::Graphics::GraphicsProfile::HiDef};
}
