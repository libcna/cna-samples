// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// InstancedModelSampleGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "InstancedModelSampleGame.hpp"

/**
 * @brief The main entry point for the application.
 *
 * The original declares this as a `static class Program` at the bottom of
 * InstancedModelSampleGame.cs, with no platform guard around it.
 *
 * @return The process exit code.
 */
int main()
{
    InstancedModelSample::InstancedModelSampleGame game;
    game.Run();
    return 0;
}
