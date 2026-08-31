// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Program.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "SplitScreenGame.hpp"

#if defined(WINDOWS) || defined(XBOX)
/**
 * @brief Runs the selected Windows/Xbox sample product.
 * @return The process exit code.
 */
int main()
{
    SplitScreenSample::SplitScreenGame game;
    game.Run();
    return 0;
}
#endif
