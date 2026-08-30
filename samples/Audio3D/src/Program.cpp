// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs -- Program
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CNA/Platform/Entrypoint.hpp"
#include "Audio3DGame.hpp"

int main()
{
    Audio3D::Audio3DGame game;
    game.Run();
    return 0;
}
