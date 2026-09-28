// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CNA/Platform/Entrypoint.hpp"
#include "NetworkStateManagementGame.hpp"

/**
 * @brief The main entry point for the application.
 *
 * @return The process exit code.
 */
int main()
{
    NetworkStateManagement::NetworkStateManagementGame game;
    game.Run();
    return 0;
}
