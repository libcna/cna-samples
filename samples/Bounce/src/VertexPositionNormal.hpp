// SPDX-License-Identifier: MS-PL
#pragma once

#include <type_traits>

#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"

namespace Bounce
{
    struct VertexPositionNormal
    {
        Microsoft::Xna::Framework::Vector3 Position;
        Microsoft::Xna::Framework::Vector3 Normal;

        VertexPositionNormal(
            Microsoft::Xna::Framework::Vector3 position,
            Microsoft::Xna::Framework::Vector3 normal)
            : Position(position), Normal(normal)
        {
        }

        static const Microsoft::Xna::Framework::Graphics::VertexDeclaration
            VertexDeclaration;

        static constexpr int SizeInBytes = 24;
    };

    static_assert(sizeof(VertexPositionNormal) == VertexPositionNormal::SizeInBytes);
    static_assert(std::is_trivially_copyable_v<VertexPositionNormal>);
}
