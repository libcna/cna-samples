// SPDX-License-Identifier: MS-PL

#include "CNA/Platform/Entrypoint.hpp"
#include "InputReporterGame.hpp"

int main()
{
    InputReporter::InputReporterGame game;
    game.Run();
    return 0;
}
