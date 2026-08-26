// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Billboard.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "BillboardGame.hpp"

/**
 * @brief The main entry point for the application.
 *
 * The original declares this as a `static class Program` at the bottom of Billboard.cs, with no
 * platform guard around it.
 *
 * @return The process exit code.
 */
int main()
{
    Billboard::BillboardGame game;
    game.Run();
    return 0;
}
