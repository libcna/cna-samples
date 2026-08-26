// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// NormalMappingEffect.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "NormalMappingEffectGame.hpp"

/**
 * @brief The main entry point for the application.
 *
 * The original declares this as a `static class Program` at the bottom of
 * NormalMappingEffect.cs, with no platform guard around it.
 *
 * @return The process exit code.
 */
int main()
{
    NormalMappingEffect::NormalMappingEffectGame game;
    game.Run();
    return 0;
}
