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
    // [assembly: AssemblyTitle("Billboard Sample")]
    //
    // XNA takes the game window's title from this attribute; it carries a "Sample" suffix the
    // assembly name, "Billboard", does not.
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"Billboard Sample"};

    // Both original projects select HiDef; XNA embeds this setting in the executable.
    const CNA::ProjectGraphicsProfileEXT projectGraphicsProfile{
        Microsoft::Xna::Framework::Graphics::GraphicsProfile::HiDef};
}
