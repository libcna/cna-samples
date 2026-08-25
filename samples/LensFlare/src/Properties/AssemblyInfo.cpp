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
    // [assembly: AssemblyTitle("Lens Flare")]
    //
    // XNA takes the game window's title from this attribute, which is why the original's
    // window is called "Lens Flare". Read from the upstream project this port follows:
    //   LensFlareSample_4_0/LensFlare/LensFlareWindows.csproj
    // The remaining attributes in the original file are .NET assembly metadata with no
    // observable effect on the running game.
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"Lens Flare"};
}
