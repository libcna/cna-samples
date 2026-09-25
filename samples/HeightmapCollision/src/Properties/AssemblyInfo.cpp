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
    // [assembly: AssemblyTitle("Heightmap Collision")]
    //
    // XNA takes the game window's title from this attribute, which differs from the assembly
    // name, "HeightmapCollision".
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"Heightmap Collision"};

    // Both original projects select HiDef. XNA embeds the project profile in the executable;
    // CNA carries the same build metadata here, before GraphicsDevice creation.
    const CNA::ProjectGraphicsProfileEXT projectGraphicsProfile{
        Microsoft::Xna::Framework::Graphics::GraphicsProfile::HiDef};
}
