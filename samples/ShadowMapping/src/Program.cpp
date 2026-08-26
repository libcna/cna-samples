// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ShadowMapping.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ShadowMappingGame.hpp"

/**
 * @brief The main entry point for the application.
 *
 * The original declares this as a `static class Program` at the bottom of ShadowMapping.cs,
 * with no platform guard around it.
 *
 * @return The process exit code.
 */
int main()
{
    ShadowMapping::ShadowMappingGame game;
    game.Run();
    return 0;
}
