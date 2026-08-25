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
    // [assembly: AssemblyTitle("Primitives3D")]
    //
    // XNA takes the game window's title from this attribute, which is why the original's
    // window is called "Primitives3D". Read from the upstream project this port follows:
    //   Primitives3DSample_4_0/Primitives3D/Primitives3DWindows.csproj
    // The remaining attributes in the original file are .NET assembly metadata with no
    // observable effect on the running game.
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"Primitives3D"};
}
