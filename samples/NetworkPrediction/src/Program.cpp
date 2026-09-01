// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// NetworkPredictionGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CNA/Platform/Entrypoint.hpp"
#include "NetworkPredictionGame.hpp"

/**
 * @brief Runs the network prediction sample.
 *
 * @return The process exit code.
 */
int main()
{
    NetworkPrediction::NetworkPredictionGame game;
    game.Run();
    return 0;
}
