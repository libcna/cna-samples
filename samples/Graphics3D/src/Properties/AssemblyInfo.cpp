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
    // [assembly: AssemblyTitle("Graphics 3D Sample")]
    //
    // XNA takes the game window's title from this attribute, which is why the original's
    // window is called "Graphics 3D Sample". Read from the upstream project this port follows:
    //   Graphics3DSample_4_0/Sample3DGraphics/Sample3DGraphics/Graphics3DSample.csproj
    // The remaining attributes in the original file are .NET assembly metadata with no
    // observable effect on the running game.
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"Graphics 3D Sample"};
}
