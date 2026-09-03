// SPDX-License-Identifier: MS-PL

#include "RacingGameManager.hpp"

#include <cstdio>
#include <exception>

int main()
{
    try
    {
        RacingGame::RacingGameManager game;
        game.Run();
        game.Dispose();
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::fprintf(stderr, "Racing Game failed: %s\n", exception.what());
        return 1;
    }
}
