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
    // [assembly: AssemblyTitle("MicrophoneEchoSample")]
    //
    // XNA takes the game window's title from this attribute, which is why the original's
    // window is called "MicrophoneEchoSample". Read from the upstream project this port follows:
    //   MicrophoneEchoSample_4_0/MicrophoneEchoSample/MicrophoneEchoSampleWindows.csproj
    // The remaining attributes in the original file are .NET assembly metadata with no
    // observable effect on the running game.
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"MicrophoneEchoSample"};

    // The original project stores this profile in Microsoft.Xna.Framework.RuntimeProfile.
#if defined(WINDOWS_PHONE)
    const CNA::ProjectGraphicsProfileEXT projectGraphicsProfile{
        Microsoft::Xna::Framework::Graphics::GraphicsProfile::Reach};
#else
    const CNA::ProjectGraphicsProfileEXT projectGraphicsProfile{
        Microsoft::Xna::Framework::Graphics::GraphicsProfile::HiDef};
#endif
}
