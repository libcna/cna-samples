// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AssemblyInfo.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CNA/AssemblyInfo.hpp"

namespace
{
    // [assembly: AssemblyTitle("Role-Playing Game Starter Kit")]
    //
    // XNA takes the game window's title from this attribute, which is why the original's
    // window is called "Role-Playing Game Starter Kit". Read from the upstream project this port follows:
    //   RolePlayingGame_4_0_Win_Xbox/RolePlayingGame_4_0_Win_Xbox/RolePlayingGame/RolePlayingGameWindows.csproj
    // The remaining attributes in the original file are .NET assembly metadata with no
    // observable effect on the running game.
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"Role-Playing Game Starter Kit"};
}
