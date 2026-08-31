// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CustomVertexFormats.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CustomVertexFormats.hpp"

#include <cstddef>
#include <type_traits>

#include "Microsoft/Xna/Framework/Graphics/VertexElement.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementUsage.hpp"

namespace SoccerPitch
{
    using Microsoft::Xna::Framework::Graphics::VertexElement;
    using Microsoft::Xna::Framework::Graphics::VertexElementFormat;
    using Microsoft::Xna::Framework::Graphics::VertexElementUsage;

    static_assert(sizeof(VertexPositionNormal) == VertexPositionNormal::SizeInBytes);
    static_assert(std::is_trivially_copyable_v<VertexPositionNormal>);
    static_assert(offsetof(VertexPositionNormal, Position) == 0);
    static_assert(offsetof(VertexPositionNormal, Normal) == 12);

    static_assert(
        sizeof(VertexPositionNormalDualTexture) == VertexPositionNormalDualTexture::SizeInBytes);
    static_assert(std::is_trivially_copyable_v<VertexPositionNormalDualTexture>);
    static_assert(offsetof(VertexPositionNormalDualTexture, Position) == 0);
    static_assert(offsetof(VertexPositionNormalDualTexture, Normal) == 12);
    static_assert(offsetof(VertexPositionNormalDualTexture, TextureCoordinate0) == 24);
    static_assert(offsetof(VertexPositionNormalDualTexture, TextureCoordinate1) == 32);

    const Microsoft::Xna::Framework::Graphics::VertexDeclaration
        VertexPositionNormal::VertexDeclaration{
            SizeInBytes,
            {
                VertexElement(
                    0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0),
                VertexElement(
                    12, VertexElementFormat::Vector3, VertexElementUsage::Normal, 0),
            }};

    VertexPositionNormal::VertexPositionNormal(Vector3 position, Vector3 normal)
        : Position(position), Normal(normal)
    {
    }

    const Microsoft::Xna::Framework::Graphics::VertexDeclaration
        VertexPositionNormalDualTexture::VertexDeclaration{
            SizeInBytes,
            {
                VertexElement(
                    0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0),
                VertexElement(
                    12, VertexElementFormat::Vector3, VertexElementUsage::Normal, 0),
                VertexElement(
                    24, VertexElementFormat::Vector2, VertexElementUsage::TextureCoordinate, 0),
                VertexElement(
                    32, VertexElementFormat::Vector2, VertexElementUsage::TextureCoordinate, 1),
            }};

    VertexPositionNormalDualTexture::VertexPositionNormalDualTexture(
        Vector3 position, Vector3 normal, Vector2 uv0, Vector2 uv1)
        : Position(position),
          Normal(normal),
          TextureCoordinate0(uv0),
          TextureCoordinate1(uv1)
    {
    }
}
