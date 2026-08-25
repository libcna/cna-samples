// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "AimingGame.hpp"

/**
 * @brief The main entry point for the application.
 *
 * Upstream this is a nested `Program` class at the bottom of `Game.cs`, inside
 * `#if WINDOWS || XBOX`. This is the desktop build, which is the audited configuration.
 */
int main()
{
    Aiming::AimingGame game;
    game.Run();
    return 0;
}
