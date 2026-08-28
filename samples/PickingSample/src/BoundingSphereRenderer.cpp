// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// BoundingSphereRenderer.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "BoundingSphereRenderer.hpp"

#include <cmath>
#include <optional>
#include <stdexcept>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include "System/InvalidOperationException.hpp"

namespace BoundingVolumeRendering
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::BasicEffect;
    using Microsoft::Xna::Framework::Graphics::BufferUsage;
    using Microsoft::Xna::Framework::Graphics::PrimitiveType;
    using Microsoft::Xna::Framework::Graphics::VertexBuffer;
    using Microsoft::Xna::Framework::Graphics::VertexPositionColor;

    namespace
    {
        // The C# statics. std::optional stands in for a null reference type, which is how the
        // original's `effect == null` guard below tells "Initialize has not run yet".
        std::optional<VertexBuffer> vertBuffer;
        std::optional<BasicEffect> effect;
        int lineCount = 0;
    }

    void BoundingSphereRenderer::Initialize(GraphicsDevice& graphicsDevice, int sphereResolution)
    {
        effect.emplace(graphicsDevice);
        effect->setLightingEnabledProperty(false);
        effect->setVertexColorEnabledProperty(true);

        lineCount = (sphereResolution + 1) * 3;

        std::vector<VertexPositionColor> verts(static_cast<std::size_t>(lineCount) * 2);
        float step = MathHelper::TwoPi / static_cast<float>(sphereResolution);
        std::size_t index = 0;

        for (float a = 0.0f; a < MathHelper::TwoPi; a += step)
        {
            verts[index++] = VertexPositionColor(
                Vector3(std::cos(a), std::sin(a), 0.0f), Color::Blue);
            verts[index++] = VertexPositionColor(
                Vector3(std::cos(a + step), std::sin(a + step), 0.0f), Color::Blue);
        }

        for (float a = 0.0f; a < MathHelper::TwoPi; a += step)
        {
            verts[index++] = VertexPositionColor(
                Vector3(std::cos(a), 0.0f, std::sin(a)), Color::Red);
            verts[index++] = VertexPositionColor(
                Vector3(std::cos(a + step), 0.0f, std::sin(a + step)), Color::Red);
        }

        for (float a = 0.0f; a < MathHelper::TwoPi; a += step)
        {
            verts[index++] = VertexPositionColor(
                Vector3(0.0f, std::cos(a), std::sin(a)), Color::Green);
            verts[index++] = VertexPositionColor(
                Vector3(0.0f, std::cos(a + step), std::sin(a + step)), Color::Green);
        }

        vertBuffer.emplace(graphicsDevice, VertexPositionColor::getVertexDeclarationStatic(),
                           static_cast<int>(verts.size()), BufferUsage::WriteOnly);
        vertBuffer->SetData(verts.data(), static_cast<int>(verts.size()));
    }

    void BoundingSphereRenderer::Draw(const BoundingSphere& sphere, const Matrix& view,
                                      const Matrix& projection)
    {
        if (!effect.has_value())
        {
            throw System::InvalidOperationException(
                "You must call Initialize before you can render any spheres.");
        }

        effect->getGraphicsDeviceProperty()->SetVertexBuffer(&*vertBuffer);

        effect->setWorldProperty(Matrix::CreateScale(sphere.Radius)
                                 * Matrix::CreateTranslation(sphere.Center));
        effect->setViewProperty(view);
        effect->setProjectionProperty(projection);

        effect->getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();

        effect->getGraphicsDeviceProperty()->DrawPrimitives(PrimitiveType::LineList, 0, lineCount);
    }
}
