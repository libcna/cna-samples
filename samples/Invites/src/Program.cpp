// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// InvitesGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CNA/Platform/Entrypoint.hpp"
#include "InvitesGame.hpp"

/**
 * @brief Runs the invites sample.
 *
 * @return The process exit code.
 */
int main()
{
    Invites::InvitesGame game;
    game.Run();
    return 0;
}
