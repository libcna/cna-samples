// SPDX-License-Identifier: MS-PL

#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "GameLogic/Physics/SpringPhysicsObject.hpp"
#include "Helpers/Vector3Helper.hpp"

namespace
{
    using Microsoft::Xna::Framework::Vector3;
    using RacingGame::GameLogic::Physics::SpringPhysicsObject;
    using RacingGame::Helpers::Vector3Helper;

    constexpr std::uint64_t OffsetBasis = UINT64_C(14695981039346656037);

    std::uint64_t HashByte(std::uint64_t hash, const std::uint8_t value)
    {
        return (hash ^ value) * UINT64_C(1099511628211);
    }

    std::uint64_t HashSingle(std::uint64_t hash, const float value)
    {
        const std::uint32_t bits = std::bit_cast<std::uint32_t>(value);
        hash = HashByte(hash, static_cast<std::uint8_t>(bits));
        hash = HashByte(hash, static_cast<std::uint8_t>(bits >> 8));
        hash = HashByte(hash, static_cast<std::uint8_t>(bits >> 16));
        return HashByte(hash, static_cast<std::uint8_t>(bits >> 24));
    }

    std::uint64_t HashSpringState(
        std::uint64_t hash, const SpringPhysicsObject& spring)
    {
        hash = HashSingle(hash, spring.pos);
        hash = HashSingle(hash, spring.velocity);
        return HashSingle(hash, spring.force);
    }

    std::uint64_t ProbeVectors()
    {
        std::uint64_t hash = OffsetBasis;
        hash = HashSingle(hash, Vector3Helper::GetAngleBetweenVectors(
            Vector3::UnitX, Vector3::UnitY));
        hash = HashSingle(hash, Vector3Helper::GetAngleBetweenVectors(
            Vector3(0.6f, 0.8f, 0.0f), Vector3::UnitX));
        hash = HashSingle(hash, Vector3Helper::DistanceToLine(
            Vector3(2.0f, 3.0f, 4.0f), Vector3(-1.0f, 0.5f, 2.0f),
            Vector3(5.0f, 4.5f, -2.0f)));
        return HashSingle(hash, Vector3Helper::SignedDistanceToPlane(
            Vector3(2.0f, -1.0f, 5.0f), Vector3(0.0f, 0.0f, 2.0f),
            Vector3::UnitZ));
    }

    std::uint64_t ProbeDefaultSpring()
    {
        SpringPhysicsObject spring;
        spring.ChangePos(1.25f);
        std::uint64_t hash = HashSpringState(OffsetBasis, spring);
        for (const float step : std::array{0.016f, 0.010f, 0.033f, 0.025f,
                                           0.016f, 0.008f})
        {
            spring.Simulate(step);
            hash = HashSpringState(hash, spring);
        }
        return hash;
    }

    std::uint64_t ProbeCarSpring()
    {
        SpringPhysicsObject spring(0.75f, 0.65f, 2.25f, -0.4f);
        spring.force = 0.125f;
        std::uint64_t hash = HashSpringState(OffsetBasis, spring);
        for (const float step : std::array{0.020f, 0.020f, 0.015f, 0.040f})
        {
            spring.Simulate(step);
            hash = HashSpringState(hash, spring);
        }
        spring.ChangePos(-0.35f);
        return HashSpringState(hash, spring);
    }
}

int main(int argc, char** argv)
{
    std::ofstream file;
    std::ostream* output = &std::cout;
    if (argc == 2)
    {
        file.open(argv[1], std::ios::trunc);
        output = &file;
    }
    else if (argc != 1)
    {
        std::fprintf(stderr, "usage: RacingGamePhysicsProbe [report path]\n");
        return 2;
    }

    *output << "FORMAT racing-cna-physics-oracle-v1\n"
            << "VECTOR hash=" << std::hex << std::setw(16)
            << std::setfill('0') << ProbeVectors() << '\n'
            << "SPRING defaultHash=" << std::setw(16) << ProbeDefaultSpring()
            << " carHash=" << std::setw(16) << ProbeCarSpring() << std::dec
            << '\n'
            << "RESULT PASS\n";
    return 0;
}
