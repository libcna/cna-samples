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
    // [assembly: AssemblyTitle("Lens Flare")]
    //
    // XNA takes the game window's title from this attribute, which is spaced where the assembly
    // name, "LensFlare", is not.
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"Lens Flare"};

    // Both original projects select HiDef; XNA embeds that project setting in the executable.
    const CNA::ProjectGraphicsProfileEXT projectGraphicsProfile{
        Microsoft::Xna::Framework::Graphics::GraphicsProfile::HiDef};
}
