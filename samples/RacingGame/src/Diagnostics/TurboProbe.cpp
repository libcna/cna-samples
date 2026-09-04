// SPDX-License-Identifier: MS-PL

#include <cmath>
#include <iostream>

#include "GameLogic/CarPhysics.hpp"

#if !defined(RACING_GAME_TURBO)
#error RacingGameTurboProbe requires RACING_GAME_TURBO
#endif

namespace
{
    bool NearlyEqual(const float left, const float right)
    {
        return std::abs(left - right) <= 0.0001f;
    }
}

int main()
{
    using RacingGame::GameLogic::CarPhysics;

    const float defaultMaxSpeedMph =
        CarPhysics::DefaultMaxSpeed * CarPhysics::MeterPerSecToMph;
    const float maxPossibleSpeedMph =
        CarPhysics::MaxPossibleSpeed * CarPhysics::MeterPerSecToMph;
    const bool passed =
        NearlyEqual(defaultMaxSpeedMph, 550.0f) &&
        NearlyEqual(maxPossibleSpeedMph, 580.0f) &&
        NearlyEqual(CarPhysics::DefaultMaxAccelerationPerSec, 5.0f) &&
        NearlyEqual(CarPhysics::MaxAcceleration, 11.5f);

    std::cout << "TURBO enabled=1 defaultMaxSpeedMph="
              << defaultMaxSpeedMph
              << " maxPossibleSpeedMph=" << maxPossibleSpeedMph
              << " defaultAcceleration="
              << CarPhysics::DefaultMaxAccelerationPerSec
              << " maxAcceleration=" << CarPhysics::MaxAcceleration
              << '\n'
              << (passed ? "RESULT PASS\n" : "RESULT FAIL\n");
    return passed ? 0 : 1;
}
