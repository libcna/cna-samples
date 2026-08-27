// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Particle3DSampleGame.hpp"

/**
 * @brief The main entry point for the application.
 *
 * The original declares this as a `static class Program` at the bottom of Game.cs, with no
 * platform guard around it.
 *
 * @return The process exit code.
 */
int main()
{
    Particle3DSample::Particle3DSampleGame game;
    game.Run();
    return 0;
}
