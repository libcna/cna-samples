// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ShatterEffectGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ShatterEffectGame.hpp"

/**
 * @brief The main entry point for the application.
 *
 * The original declares this as a `static class Program` at the bottom of
 * ShatterEffectGame.cs, with no platform guard around it.
 *
 * @return The process exit code.
 */
int main()
{
    ShatterSample::ShatterEffectGame game;
    game.Run();
    return 0;
}
