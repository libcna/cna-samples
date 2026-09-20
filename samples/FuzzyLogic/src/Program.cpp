// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Program.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "FuzzyLogicGame.hpp"

#if defined(WINDOWS) || defined(XBOX)
/**
 * @brief The main entry point for the application.
 * @return The process exit code.
 */
int main()
{
    FuzzyLogic::FuzzyLogicGame game;
    game.Run();
    return 0;
}
#endif
