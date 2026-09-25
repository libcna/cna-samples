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
    // [assembly: AssemblyTitle("Particles2DPipelineSample")]
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"Particles2DPipelineSample"};

    // The original Windows and Xbox projects select HiDef; Phone selects Reach.
#if !defined(WINDOWS_PHONE)
    const CNA::ProjectGraphicsProfileEXT projectGraphicsProfile{
        Microsoft::Xna::Framework::Graphics::GraphicsProfile::HiDef};
#endif
}
