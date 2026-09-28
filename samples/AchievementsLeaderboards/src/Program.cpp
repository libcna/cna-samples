// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Program.cs
//
// An XNA Game Studio 4.0 program written for CNA; see AchievementsLeaderboardsGame.cs.
//-----------------------------------------------------------------------------

#include "CNA/Platform/Entrypoint.hpp"
#include "AchievementsLeaderboardsGame.hpp"

/**
 * @brief The main entry point for the application.
 *
 * @return The process exit code.
 */
int main()
{
    AchievementsLeaderboards::AchievementsLeaderboardsGame game;
    game.Run();
    return 0;
}
