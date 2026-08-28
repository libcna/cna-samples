// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Program.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Graphics3DSampleGame.hpp"

/**
 * @brief The main entry point for the application.
 *
 * The original's Program.cs sits behind `#if WINDOWS || XBOX` and does not compile: it constructs
 * a type named `Sample3DGraphics`, while the game class is `Graphics3DSampleGame`. The project
 * only ever built for Windows Phone, so that block was never compiled. This is the entry point it
 * was meant to be.
 *
 * @return The process exit code.
 */
int main()
{
    Graphics3DSample::Graphics3DSampleGame game;
    game.Run();
    return 0;
}
