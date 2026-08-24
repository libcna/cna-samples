// SPDX-License-Identifier: MS-PL

#include "VectorShape.hpp"

#include "SpacewarGame.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework::Graphics;

    VectorShape::VectorShape(Microsoft::Xna::Framework::Game* game)
        : Shape(game)
    {
    }

    void VectorShape::Create()
    {
        const int count = 2 * getNumberOfVectorsProperty();
        buffer_ = std::make_unique<VertexBuffer>(
            getGameInstanceProperty()->getGraphicsDeviceProperty(),
            VertexPositionColor::getVertexDeclarationStatic(), count, BufferUsage::WriteOnly);
        std::vector<VertexPositionColor> data(static_cast<std::size_t>(count));
        FillBuffer(data);
        buffer_->SetData(data.data(), count);
        if (!effect_ || effect_->getIsDisposedProperty())
            OnCreateDevice();
    }

    void VectorShape::Render()
    {
        Shape::Render();
        auto& device = getGameInstanceProperty()->getGraphicsDeviceProperty();
        device.SetVertexBuffer(buffer_.get());
        worldViewProjectionParam_->SetValue(
            getWorldProperty() * SpacewarGame::getCameraProperty().getViewProperty() *
            SpacewarGame::getCameraProperty().getProjectionProperty());
        effect_->getTechniquesProperty()[0].getPassesProperty()[0].Apply();
        device.DrawPrimitives(PrimitiveType::LineList, 0, getNumberOfVectorsProperty());
    }

    void VectorShape::OnCreateDevice()
    {
        effect_ = SpacewarGame::getContentManagerProperty().Load<std::shared_ptr<Effect>>(
            SpacewarGame::getSettingsProperty().MediaPath + "shaders/simple");
        worldViewProjectionParam_ = effect_->getParametersProperty()["worldViewProjection"];

        const int count = 2 * getNumberOfVectorsProperty();
        buffer_ = std::make_unique<VertexBuffer>(
            getGameInstanceProperty()->getGraphicsDeviceProperty(),
            VertexPositionColor::getVertexDeclarationStatic(), count, BufferUsage::WriteOnly);
        std::vector<VertexPositionColor> data(static_cast<std::size_t>(count));
        FillBuffer(data);
        buffer_->SetData(data.data(), count);
    }
}
