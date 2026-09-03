// SPDX-License-Identifier: MS-PL

#include "RacingGameManager.hpp"
#include "Helpers/Log.hpp"

#include <cstdio>
#include <exception>
#include <string>

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
        RacingGame::Helpers::Log::Write(
            std::string("Racing Game failed: ") + exception.what());
        std::fprintf(stderr, "Racing Game failed: %s\n", exception.what());
        return 1;
    }
}
