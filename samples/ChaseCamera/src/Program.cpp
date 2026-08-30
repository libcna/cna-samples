// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs -- Program
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CNA/Platform/Entrypoint.hpp"
#include "ChaseCameraGame.hpp"

int main()
{
    ChaseCameraSample::ChaseCameraGame game;
    game.Run();
    return 0;
}
