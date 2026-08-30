// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CustomModel.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CustomModel.hpp"

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "System/InvalidCastException.hpp"

namespace CustomModelSample
{
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Graphics::BasicEffect;
    using Microsoft::Xna::Framework::Graphics::EffectPass;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Graphics::PrimitiveType;

    const std::string& CustomModel::GetTypeName() const
    {
        static const std::string name = "CustomModelSample.CustomModel";
        return name;
    }

    const std::string& CustomModel::ModelPart::GetTypeName() const
    {
        static const std::string name = "CustomModelSample.CustomModel+ModelPart";
        return name;
    }

    void CustomModel::Draw(const Matrix& world, const Matrix& view, const Matrix& projection)
    {
        for (const std::shared_ptr<ModelPart>& modelPart : modelParts)
        {
            BasicEffect* effect = dynamic_cast<BasicEffect*>(modelPart->Effect.get());
            if (effect == nullptr)
            {
                throw System::InvalidCastException(
                    "CustomModelSample.CustomModel requires every model part to use BasicEffect.");
            }

            effect->EnableDefaultLighting();
            effect->setWorldProperty(world);
            effect->setViewProperty(view);
            effect->setProjectionProperty(projection);

            GraphicsDevice& device = *effect->getGraphicsDeviceProperty();
            device.SetVertexBuffer(modelPart->VertexBuffer.get());
            device.setIndicesProperty(modelPart->IndexBuffer.get());

            for (EffectPass& pass : effect->getCurrentTechniqueProperty()->getPassesProperty())
            {
                pass.Apply();
                device.DrawIndexedPrimitives(
                    PrimitiveType::TriangleList, 0, 0, modelPart->VertexCount,
                    0, modelPart->TriangleCount);
            }
        }
    }
}
