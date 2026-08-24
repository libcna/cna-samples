// SPDX-License-Identifier: MS-PL

#include "CNA/Platform/Entrypoint.hpp"
#include "SafeAreaGame.hpp"

int main()
{
    SafeArea::SafeAreaGame game;
    game.Run();
    return 0;
}
