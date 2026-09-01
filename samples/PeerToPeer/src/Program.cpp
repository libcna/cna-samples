// SPDX-License-Identifier: MS-PL

#include "CNA/Platform/Entrypoint.hpp"
#include "PeerToPeerGame.hpp"

/**
 * @brief Runs the peer-to-peer networking sample.
 *
 * @return The process exit code.
 */
int main()
{
    PeerToPeer::PeerToPeerGame game;
    game.Run();
    return 0;
}
