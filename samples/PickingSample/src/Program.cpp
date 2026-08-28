// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Program.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "PickingSampleGame.hpp"

/**
 * @brief The main entry point for the application.
 *
 * The original declares this inside `#if WINDOWS || XBOX`, which is satisfied by the Windows
 * build this port is.
 *
 * @return The process exit code.
 */
int main()
{
    PickingSample::PickingSampleGame game;
    game.Run();
    return 0;
}
