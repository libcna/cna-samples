// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Program.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "RimLightingGame.hpp"

/**
 * @brief The main entry point for the application.
 *
 * The original is a Windows Phone project, whose Program.cs is guarded by
 * `#if WINDOWS || XBOX` and — inside that guard — names a class `Game1` that does not exist
 * anywhere in the sample; the game class is `SampleGame`. On the platform the project targets,
 * none of that is ever compiled. This is the entry point the phone's own targets supply.
 *
 * @return The process exit code.
 */
int main()
{
    RimLighting::SampleGame game;
    game.Run();
    return 0;
}
