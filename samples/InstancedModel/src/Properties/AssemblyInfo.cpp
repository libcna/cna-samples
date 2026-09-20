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
    // [assembly: AssemblyTitle("Instanced Model")]
    //
    // XNA takes the game window's title from this attribute, which differs from both the
    // assembly name ("InstancedModelSample") and the product name ("Instanced Model Sample").
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"Instanced Model"};

    // Both original projects select HiDef; XNA embeds that project setting in the executable.
    const CNA::ProjectGraphicsProfileEXT projectGraphicsProfile{
        Microsoft::Xna::Framework::Graphics::GraphicsProfile::HiDef};
}
