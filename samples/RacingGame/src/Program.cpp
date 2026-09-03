// SPDX-License-Identifier: MS-PL

#include "RacingGameManager.hpp"
#include "Helpers/Log.hpp"
#include "Platform/PersistentStorage.hpp"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>

#include <cstdio>
#include <exception>
#include <string>

int main(int, char**)
{
    try
    {
        RacingGame::Platform::PersistentStorage::Prepare();
        RacingGame::RacingGameManager game;
        game.Run();
        game.Dispose();
        return 0;
    }
    catch (const std::exception& exception)
    {
        RacingGame::Helpers::Log::Write(
            std::string("Racing Game failed: ") + exception.what());
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Racing Game failed: %s", exception.what());
        std::fprintf(stderr, "Racing Game failed: %s\n", exception.what());
        return 1;
    }
}
