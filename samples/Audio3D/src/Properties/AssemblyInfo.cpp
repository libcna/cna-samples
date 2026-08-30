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
    // [assembly: AssemblyTitle("Audio 3D")]
    //
    // XNA takes the game window's title from this attribute, which is why the original's
    // window is called "Audio 3D". Read from the upstream source this port follows:
    //   Audio3DSample_4_0/Audio3D/Properties/AssemblyInfo.cs
    // The remaining attributes in the original file are .NET assembly metadata with no
    // observable effect on the running game.
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"Audio 3D"};
}
