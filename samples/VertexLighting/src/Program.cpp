// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// VertexLighting.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "VertexLightingGame.hpp"

/**
 * @brief The main entry point for the application.
 *
 * The original declares this as the game class's own static Main, with no platform guard
 * around it.
 *
 * @return The process exit code.
 */
int main()
{
    VertexLightingSample::VertexLighting game;
    game.Run();
    return 0;
}
