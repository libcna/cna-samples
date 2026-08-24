// SPDX-License-Identifier: MS-PL

#include "EvolvedBackdrop.hpp"

#include <array>
#include <cmath>

#include "SpacewarGame.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    EvolvedBackdrop::EvolvedBackdrop(Game* game)
        : Shape(game), layer1Offset_(Vector4::Zero), layer2Offset_(Vector4::Zero)
    {
        Create();
    }

    void EvolvedBackdrop::Create()
    {
        buffer_ = std::make_unique<VertexBuffer>(getGameInstanceProperty()->getGraphicsDeviceProperty(),
            VertexPositionColor::getVertexDeclarationStatic(), 6, BufferUsage::WriteOnly);
        const std::array<VertexPositionColor, 6> data{
            VertexPositionColor(Vector3(0, 0, 0), Color::White),
            VertexPositionColor(Vector3(1280, 0, 0), Color::White),
            VertexPositionColor(Vector3(1280, 720, 0), Color::White),
            VertexPositionColor(Vector3(0, 0, 0), Color::White),
            VertexPositionColor(Vector3(1280, 720, 0), Color::White),
            VertexPositionColor(Vector3(0, 720, 0), Color::White),
        };
        buffer_->SetData(data.data(), static_cast<int>(data.size()));

        effect_ = SpacewarGame::getContentManagerProperty().Load<std::shared_ptr<Effect>>(
            SpacewarGame::getSettingsProperty().MediaPath + "shaders/backdrop");
        auto& parameters = effect_->getParametersProperty();
        layer1TextureParam_ = parameters["layer1"];
        layer2TextureParam_ = parameters["layer2"];
        layer3TextureParam_ = parameters["layer3"];
        layerFactorParam_ = parameters["layerFactor"];
        layer1OffsetParam_ = parameters["layer1Offset"];
        layer2OffsetParam_ = parameters["layer2Offset"];
        layer1_.emplace(SpacewarGame::getContentManagerProperty().Load<Texture2D>(
            SpacewarGame::getSettingsProperty().MediaPath + "textures/B1_nebula01"));
        layer2_.emplace(SpacewarGame::getContentManagerProperty().Load<Texture2D>(
            SpacewarGame::getSettingsProperty().MediaPath + "textures/B1_nebula02"));
        layer3_.emplace(SpacewarGame::getContentManagerProperty().Load<Texture2D>(
            SpacewarGame::getSettingsProperty().MediaPath + "textures/B1_stars"));
    }

    void EvolvedBackdrop::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
    {
        Shape::Update(time, elapsedTime);
        timeFactor1_ += static_cast<float>(elapsedTime.getTotalSecondsProperty());
        timeFactor2_ += static_cast<float>(elapsedTime.getTotalSecondsProperty() * 2.0);
        const Settings& settings = SpacewarGame::getSettingsProperty();
        layerFactor_ = static_cast<float>(std::sin(timeFactor2_ * settings.CrossFadeSpeed) * 0.5 + 0.5);
        layer1Offset_.X = static_cast<float>((100.0 / 1480.0) * 0.3 *
            (std::sin(timeFactor1_ * settings.OffsetSpeed / 2.0) + 1.0));
        layer1Offset_.Y = static_cast<float>((100.0 / 920.0) *
            (std::cos(timeFactor1_ * settings.OffsetSpeed / 1.4) + 1.0));
        layer2Offset_.X = static_cast<float>((100.0 / 1480.0) *
            (std::sin(timeFactor1_ * settings.OffsetSpeed) + 1.0));
        layer2Offset_.Y = static_cast<float>((100.0 / 920.0) * 0.7 *
            (std::cos(timeFactor1_ * settings.OffsetSpeed / 1.3) + 1.0));
    }

    void EvolvedBackdrop::Render()
    {
        auto& device = getGameInstanceProperty()->getGraphicsDeviceProperty();
        Shape::Render();
        device.SetVertexBuffer(buffer_.get());
        layer1TextureParam_->SetValue(&*layer1_);
        layer2TextureParam_->SetValue(&*layer2_);
        layer3TextureParam_->SetValue(&*layer3_);
        layerFactorParam_->SetValue(layerFactor_);
        layer1OffsetParam_->SetValue(layer1Offset_);
        layer2OffsetParam_->SetValue(layer2Offset_);
        effect_->getTechniquesProperty()[0].getPassesProperty()[0].Apply();
        device.DrawPrimitives(PrimitiveType::TriangleList, 0, XCount * YCount * 2);
    }

    void EvolvedBackdrop::Dispose()
    {
        if (effect_)
            effect_->Dispose();
        effect_.reset();
        Shape::Dispose();
    }
}
