// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// QuadDrawer.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "QuadDrawer.hpp"

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/CompareFunction.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"

namespace Audio3D
{
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::CompareFunction;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Graphics::PrimitiveType;
    using Microsoft::Xna::Framework::Graphics::Texture2D;
    using SharpRuntime::Single;

    QuadDrawer::QuadDrawer(GraphicsDevice& device)
        : graphicsDevice(device),
          effect(device)
    {
        effect.setAlphaFunctionProperty(CompareFunction::Greater);
        effect.setReferenceAlphaProperty(128);

        vertices[0].Position = Vector3(1.0f, 1.0f, 0.0f);
        vertices[1].Position = Vector3(-1.0f, 1.0f, 0.0f);
        vertices[2].Position = Vector3(1.0f, -1.0f, 0.0f);
        vertices[3].Position = Vector3(-1.0f, -1.0f, 0.0f);
    }

    void QuadDrawer::DrawQuad(
        Texture2D& texture,
        Single textureRepeats,
        const Matrix& world,
        const Matrix& view,
        const Matrix& projection)
    {
        effect.setTextureProperty(&texture);
        effect.setWorldProperty(world);
        effect.setViewProperty(view);
        effect.setProjectionProperty(projection);

        vertices[0].TextureCoordinate = Vector2(0.0f, 0.0f);
        vertices[1].TextureCoordinate = Vector2(textureRepeats, 0.0f);
        vertices[2].TextureCoordinate = Vector2(0.0f, textureRepeats);
        vertices[3].TextureCoordinate = Vector2(textureRepeats, textureRepeats);

        effect.getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();
        graphicsDevice.DrawUserPrimitives(PrimitiveType::TriangleStrip, vertices, 0, 2);
    }
}
