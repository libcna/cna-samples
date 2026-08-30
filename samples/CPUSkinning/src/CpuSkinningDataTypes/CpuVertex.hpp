// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CpuVertex.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"

namespace CpuSkinningDataTypes
{
    /**
     * @brief Contains the vertex information needed by the CPU skinning algorithm.
     *
     * This type is serialized content data and is not itself a GPU vertex type.
     */
    struct CpuVertex
    {
        /** @brief Position in bind-pose model space. */
        Microsoft::Xna::Framework::Vector3 Position;

        /** @brief Normal in bind-pose model space. */
        Microsoft::Xna::Framework::Vector3 Normal;

        /** @brief Texture coordinate copied unchanged to the GPU vertex. */
        Microsoft::Xna::Framework::Vector2 TextureCoordinate;

        /** @brief Four bone influence weights. */
        Microsoft::Xna::Framework::Vector4 BlendWeights;

        /** @brief Four bone indices stored as floating-point values by the original pipeline. */
        Microsoft::Xna::Framework::Vector4 BlendIndices;
    };
}
