// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleVertex.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <type_traits>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/Short2.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"

namespace Particle3DSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief Custom vertex structure for drawing particles.
     *
     * The layout is the GPU's, not the CPU's: this struct is uploaded verbatim, so the members'
     * order and their sizes are part of the contract with the shader.
     *
     * The two packed members are held as their raw 32-bit values rather than as `Short2` and
     * `Color`. Both of those inherit CNA's polymorphic `IPackedVectorT`, so a C++ object of either
     * carries a vtable pointer that must never reach a vertex buffer; the packed types are still
     * what computes the values, through `getPackedValueProperty()`.
     */
    struct ParticleVertex
    {
        /** @brief Stores which corner of the particle quad this vertex represents (packed Short2). */
        std::uint32_t Corner;

        /** @brief Stores the starting position of the particle. */
        Vector3 Position;

        /** @brief Stores the starting velocity of the particle. */
        Vector3 Velocity;

        /** @brief Four random values, used to make each particle look slightly different (packed Color). */
        std::uint32_t Random;

        /** @brief The time (in seconds) at which this particle was created. */
        float Time;

        /**
         * @brief Describes the layout of this vertex structure.
         * @return The VertexDeclaration the particle effect reads its inputs through.
         */
        [[nodiscard]] static const VertexDeclaration& GetVertexDeclaration();

        /** @brief Describes the size of this vertex structure. */
        static constexpr int SizeInBytes = 36;
    };
}
