// SPDX-License-Identifier: MS-PL
#pragma once

#include <type_traits>

#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"

namespace PerformanceMeasuring
{
    using Microsoft::Xna::Framework::Vector3;

    /** @brief Custom vertex containing a position and normal without texture coordinates. */
    struct VertexPositionNormal
    {
        /** @brief Vertex position. */
        Vector3 Position;
        /** @brief Vertex normal. */
        Vector3 Normal;

        /** @brief Constructs a zero-initialized vertex. */
        VertexPositionNormal() = default;

        /** @brief Constructs a position-and-normal vertex. */
        VertexPositionNormal(Vector3 position, Vector3 normal)
            : Position(position), Normal(normal)
        {
        }

        /** @brief Declaration of the two elements stored in this vertex. */
        static const Microsoft::Xna::Framework::Graphics::VertexDeclaration VertexDeclaration;

        /** @brief Returns this type's vertex declaration. */
        [[nodiscard]] const Microsoft::Xna::Framework::Graphics::VertexDeclaration&
        getVertexDeclarationProperty() const
        {
            return VertexDeclaration;
        }
    };

    static_assert(sizeof(VertexPositionNormal) == 24);
    static_assert(std::is_trivially_copyable_v<VertexPositionNormal>);
}
