// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CustomVertexFormats.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace SoccerPitch
{
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;
    using SharpRuntime::intcs;

    /** @brief Vertex containing a position and normal without texture coordinates. */
    struct VertexPositionNormal
    {
        /** @brief Position of the vertex. */
        Vector3 Position;

        /** @brief Surface normal of the vertex. */
        Vector3 Normal;

        /** @brief Creates a zero-initialized vertex. */
        VertexPositionNormal() = default;

        /**
         * @brief Creates a vertex from its position and normal.
         *
         * @param position Position of the vertex.
         * @param normal Surface normal of the vertex.
         */
        VertexPositionNormal(Vector3 position, Vector3 normal);

        /** @brief Describes the layout of this vertex type. */
        static const Microsoft::Xna::Framework::Graphics::VertexDeclaration VertexDeclaration;

        /** @brief Size of this vertex type in bytes. */
        static constexpr intcs SizeInBytes = 24;
    };

    /** @brief Vertex containing a position, normal, and two independent texture coordinates. */
    struct VertexPositionNormalDualTexture
    {
        /** @brief Position of the vertex. */
        Vector3 Position;

        /** @brief Surface normal of the vertex. */
        Vector3 Normal;

        /** @brief Texture coordinate consumed by the first texture. */
        Vector2 TextureCoordinate0;

        /** @brief Texture coordinate consumed by the second texture. */
        Vector2 TextureCoordinate1;

        /** @brief Creates a zero-initialized vertex. */
        VertexPositionNormalDualTexture() = default;

        /**
         * @brief Creates a vertex from its position, normal, and two texture coordinates.
         *
         * @param position Position of the vertex.
         * @param normal Surface normal of the vertex.
         * @param uv0 Texture coordinate for the first texture.
         * @param uv1 Texture coordinate for the second texture.
         */
        VertexPositionNormalDualTexture(
            Vector3 position, Vector3 normal, Vector2 uv0, Vector2 uv1);

        /** @brief Describes the layout of this vertex type. */
        static const Microsoft::Xna::Framework::Graphics::VertexDeclaration VertexDeclaration;

        /** @brief Size of this vertex type, including both texture coordinates, in bytes. */
        static constexpr intcs SizeInBytes = 40;
    };
}
