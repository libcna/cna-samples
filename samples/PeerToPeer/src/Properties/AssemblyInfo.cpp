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
    // [assembly: AssemblyTitle("Networking:  Peer-to-Peer")]
    //
    // XNA takes the game window's title from this attribute, which is why the original's
    // window is called "Networking:  Peer-to-Peer". Read from the upstream project this port follows:
    //   PeerToPeerSample_4_0/PeerToPeer/PeerToPeerWindows.csproj
    // The remaining attributes in the original file are .NET assembly metadata with no
    // observable effect on the running game.
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"Networking:  Peer-to-Peer"};
}
