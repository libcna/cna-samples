// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Block.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace TransformedCollision
{
    using Microsoft::Xna::Framework::Vector2;

    /**
     * @brief A falling and spinning object to be avoided.
     */
    class Block
    {
    public:
        /** @brief World position of the block's centre. */
        Vector2 Position;
        /** @brief Rotation of the block, in radians. */
        float Rotation = 0.0f;
    };
}
