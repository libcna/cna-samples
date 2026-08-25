// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PathDrawing
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "PathDrawingGame.hpp"

/**
 * @brief The main entry point for the application.
 *
 * The upstream project targets Windows Phone 7 and has no Program.cs of its own -- the
 * WP7 build targets generate the entry point. This is that entry point.
 */
int main()
{
    PathDrawing::PathDrawingGame game;
    game.Run();
    return 0;
}
