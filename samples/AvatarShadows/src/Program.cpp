// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Program.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "AvatarShadowsGame.hpp"
#include "CNA/Platform/Entrypoint.hpp"

/**
 * @brief Runs the avatar shadows sample.
 *
 * @return The process exit code.
 */
int main()
{
    AvatarShadows::AvatarShadowsGame game;
    game.Run();
    return 0;
}
