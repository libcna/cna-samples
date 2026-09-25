// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Program.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ParticleSampleGame.hpp"

/**
 * @brief The main entry point for the application.
 * XNA defines WINDOWS or XBOX for desktop/console builds. CNA's native and
 * browser hosts define neither, but use the same desktop entry point.
 * @return The process exit code.
 */
#if !defined(WINDOWS_PHONE)
int main()
{
    Particles2DPipelineSample::ParticleSampleGame game;
    game.Run();
    return 0;
}
#endif
