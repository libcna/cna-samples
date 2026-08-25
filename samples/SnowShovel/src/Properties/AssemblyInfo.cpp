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
    // [assembly: AssemblyTitle("SnowShovel")]
    //
    // XNA takes the game window's title from this attribute, which is why the original's
    // window is called "SnowShovel". Read from the upstream project this port follows:
    //   SnowShovelSample_4_0/SnowShovel/SnowShovel/SnowShovelWindows.csproj
    // The remaining attributes in the original file are .NET assembly metadata with no
    // observable effect on the running game.
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"SnowShovel"};
}
