// SPDX-License-Identifier: MS-PL

#include "EvolvedSun.hpp"

#include "SpacewarGame.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "System/Int32.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    EvolvedSun::EvolvedSun(Game* game)
        : Shape(game)
    {
        Create();
    }

    void EvolvedSun::Create()
    {
        buffer_ = Plane(XCount, YCount);
        effect_ = SpacewarGame::getContentManagerProperty().Load<std::shared_ptr<Effect>>(
            SpacewarGame::getSettingsProperty().MediaPath + "shaders/sun");
        auto& parameters = effect_->getParametersProperty();
        worldParam_ = parameters["world"];
        worldViewProjectionParam_ = parameters["worldViewProjection"];
        sun0TextureParam_ = parameters["Sun_Tex0"];
        sun1TextureParam_ = parameters["Sun_Tex1"];
        blendFactor_ = parameters["blendFactor"];
        for (int i = 0; i < 5; ++i)
        {
            sun_[static_cast<std::size_t>(i)].emplace(
                SpacewarGame::getContentManagerProperty().Load<Texture2D>(
                    SpacewarGame::getSettingsProperty().MediaPath + "textures/suntest" + System::Int32::ToString(i + 1)));
        }
    }

    void EvolvedSun::Update(System::TimeSpan timeSpan, System::TimeSpan elapsedTime)
    {
        Shape::Update(timeSpan, elapsedTime);
        currentTime_ += elapsedTime.getTotalMillisecondsProperty();
        if (currentTime_ > 1000.0)
        {
            currentTime_ = 0.0;
            ++currentFrame_;
        }
        if (currentFrame_ > 4)
            currentFrame_ = 0;
    }

    void EvolvedSun::Render()
    {
        auto& device = getGameInstanceProperty()->getGraphicsDeviceProperty();
        Shape::Render();
        device.SetVertexBuffer(buffer_.get());
        worldParam_->SetValue(world_);
        worldViewProjectionParam_->SetValue(
            world_ * SpacewarGame::getCameraProperty().getViewProperty() *
            SpacewarGame::getCameraProperty().getProjectionProperty());
        blendFactor_->SetValue(static_cast<float>(currentTime_ / 1000.0));
        sun1TextureParam_->SetValue(&*sun_[static_cast<std::size_t>(currentFrame_)]);
        sun0TextureParam_->SetValue(&*sun_[static_cast<std::size_t>(currentFrame_ < 4 ? currentFrame_ + 1 : 0)]);
        effect_->getTechniquesProperty()[0].getPassesProperty()[0].Apply();
        device.DrawPrimitives(PrimitiveType::TriangleList, 0, XCount * YCount * 2);
    }
}
