// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Program.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "FlockingSample.hpp"

/**
 * @brief The main entry point for the application.
 *
 * The original wraps this in `#if WINDOWS || XBOX`, so the phone build has no entry
 * point of its own. This is the desktop build, which is the audited configuration.
 */
int main()
{
    Flocking::FlockingSample game;
    game.Run();
    return 0;
}
