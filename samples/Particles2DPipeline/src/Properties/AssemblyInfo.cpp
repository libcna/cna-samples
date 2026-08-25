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
    // [assembly: AssemblyTitle("Particles2DPipelineSample")]
    //
    // XNA takes the game window's title from this attribute, which is why the original's
    // window is called "Particles2DPipelineSample". Read from the upstream project this port follows:
    //   Particles2DPipeline_4_0/Particles2DPipelineSample/Particles2DPipelineSample/Particles2DPipelineSample (Windows).csproj
    // The remaining attributes in the original file are .NET assembly metadata with no
    // observable effect on the running game.
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"Particles2DPipelineSample"};
}
