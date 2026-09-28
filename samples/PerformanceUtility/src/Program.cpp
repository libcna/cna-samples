// SPDX-License-Identifier: MS-PL
// Copyright (C) Microsoft Corporation. All rights reserved.

#include "CNA/Platform/Entrypoint.hpp"

#include "PerformanceUtilityGame.hpp"

#if defined(WINDOWS) || defined(XBOX)
int main()
{
    PerformanceUtility::PerformanceUtilityGame game;
    game.Run();
    return 0;
}
#endif
