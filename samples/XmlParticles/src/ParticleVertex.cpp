// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleVertex.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ParticleVertex.hpp"

#include "Microsoft/Xna/Framework/Graphics/VertexElement.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementUsage.hpp"

namespace Particle3DSample
{
    // The struct is uploaded raw, so its C++ layout has to be exactly the 36 bytes the
    // declaration below describes -- no padding, no vtable, nothing the compiler adds.
    static_assert(sizeof(ParticleVertex) == ParticleVertex::SizeInBytes);
    static_assert(std::is_trivially_copyable_v<ParticleVertex>);
    static_assert(offsetof(ParticleVertex, Position) == 4);
    static_assert(sizeof(ParticleVertex::Corner) == 4);
    static_assert(offsetof(ParticleVertex, Velocity) == 16);
    static_assert(offsetof(ParticleVertex, Random) == 28);
    static_assert(offsetof(ParticleVertex, Time) == 32);

    const VertexDeclaration& ParticleVertex::GetVertexDeclaration()
    {
        static const VertexDeclaration declaration{
            VertexElement(0, VertexElementFormat::Short2,
                             VertexElementUsage::Position, 0),

            VertexElement(4, VertexElementFormat::Vector3,
                             VertexElementUsage::Position, 1),

            VertexElement(16, VertexElementFormat::Vector3,
                              VertexElementUsage::Normal, 0),

            VertexElement(28, VertexElementFormat::Color,
                              VertexElementUsage::Color, 0),

            VertexElement(32, VertexElementFormat::Single,
                              VertexElementUsage::TextureCoordinate, 0)
        };
        return declaration;
    }
}
