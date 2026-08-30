// SPDX-License-Identifier: MS-PL

#include "CNA/Platform/Entrypoint.hpp"
#include "IKSample.hpp"

int main()
{
    InverseKinematicsSample::IKSample game;
    game.Run();
    return 0;
}
