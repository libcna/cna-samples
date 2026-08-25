// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// FlockingSample.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

namespace Flocking
{
    /**
     * @brief The tunable weights the flocking AI runs on.
     *
     * Upstream this struct is declared at the top of `FlockingSample.cs`, above the game
     * class. C++ needs it as its own header because `Bird` and every `Behavior` take it by
     * reference and are compiled before the game is; nothing about the type changes.
     */
    struct AIParameters
    {
        /** @brief How far an animal can see another animal. */
        float DetectionDistance = 0.0f;
        /** @brief Inside this distance an animal pushes away instead of pulling closer. */
        float SeparationDistance = 0.0f;
        /** @brief Weight of the direction the animal is already travelling in. */
        float MoveInOldDirectionInfluence = 0.0f;
        /** @brief Weight of the direction the rest of the flock is travelling in. */
        float MoveInFlockDirectionInfluence = 0.0f;
        /** @brief Weight of a random nudge, so the flock does not lock into a line. */
        float MoveInRandomDirectionInfluence = 0.0f;
        /** @brief How far an animal may turn in one second, in radians. */
        float MaxTurnRadians = 0.0f;
        /** @brief Weight applied per flock member reacted to. */
        float PerMemberWeight = 0.0f;
        /** @brief Weight applied per danger reacted to. */
        float PerDangerWeight = 0.0f;
    };
}
