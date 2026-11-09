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
    // [assembly: AssemblyTitle("SpaceWar")]
    //
    // XNA takes the game window's title from this attribute, which is why the original's
    // window is called "SpaceWar". Read from the upstream project this port follows:
    //   Spacewar_4_0/SpaceWarWindows.csproj
    // The remaining attributes in the original file are .NET assembly metadata with no
    // observable effect on the running game.
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"SpaceWar"};

    // The Windows and Xbox project files both select HiDef. XNA embeds that project setting
    // in its executable; it is not a line in the game's C# source.
    const CNA::ProjectGraphicsProfileEXT projectGraphicsProfile{
        Microsoft::Xna::Framework::Graphics::GraphicsProfile::HiDef};
}
