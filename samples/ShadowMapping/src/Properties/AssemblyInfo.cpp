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
    // [assembly: AssemblyTitle("Shadow Mapping")]
    //
    // XNA takes the game window's title from this attribute; it is spaced and does not match
    // the assembly name, which is "ShadowMapping".
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"Shadow Mapping"};

    // The Windows and Xbox project files both select HiDef. XNA embeds that project setting
    // in its executable; it is not a line in ShadowMapping.cs.
    const CNA::ProjectGraphicsProfileEXT projectGraphicsProfile{
        Microsoft::Xna::Framework::Graphics::GraphicsProfile::HiDef};
}
