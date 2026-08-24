// SPDX-License-Identifier: MS-PL

#include "RetroProjectiles.hpp"

#include <algorithm>

#include "SpacewarGame.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    RetroProjectiles::RetroProjectiles(Game* game)
        : Projectiles(game)
    {
        Create();
    }

    void RetroProjectiles::Add(PlayerIndex player, Vector3 position, Vector3 velocity,
                               float angle, System::TimeSpan time, Particles*)
    {
        SceneItem::Add(std::make_unique<Projectile>(getGameInstanceProperty(), player, position,
                                                     velocity, angle, time, nullptr));
    }

    void RetroProjectiles::Render()
    {
        if (getCountProperty() == 0)
            return;
        Projectiles::Render();
        auto& device = getGameInstanceProperty()->getGraphicsDeviceProperty();
        const int totalCount = std::min(static_cast<int>(getCountProperty()), MaxTriangleCount);
        int position = 0;
        for (const auto& item : getChildrenProperty())
        {
            if (position < MaxProjectileCount)
            {
                Vector3 bulletPosition = item->getPositionProperty();
                data_[position++].Position = bulletPosition;
                data_[position++].Position = bulletPosition + Vector3(-1, 0, 0);
                data_[position++].Position = bulletPosition + Vector3(0, 1, 0);
                bulletPosition.X += 0.5f;
                data_[position++].Position = bulletPosition;
                data_[position++].Position = bulletPosition + Vector3(-1, 0, 0);
                data_[position++].Position = bulletPosition + Vector3(0, 1, 0);
            }
        }
        device.setRasterizerStateProperty(RasterizerState::CullNone);
        worldViewProjectionParam_->SetValue(
            SpacewarGame::getCameraProperty().getViewProperty() *
            SpacewarGame::getCameraProperty().getProjectionProperty());
        effect_->getTechniquesProperty()[0].getPassesProperty()[0].Apply();
        device.DrawUserPrimitives(PrimitiveType::TriangleList, data_.data(), 0, totalCount * 2);
    }

    void RetroProjectiles::Create()
    {
        if (!effect_ || effect_->getIsDisposedProperty())
            OnCreateDevice();
    }

    void RetroProjectiles::OnCreateDevice()
    {
        effect_ = SpacewarGame::getContentManagerProperty().Load<std::shared_ptr<Effect>>(
            SpacewarGame::getSettingsProperty().MediaPath + "shaders/simple");
        worldViewProjectionParam_ = effect_->getParametersProperty()["worldViewProjection"];
        buffer_ = std::make_unique<VertexBuffer>(getGameInstanceProperty()->getGraphicsDeviceProperty(),
            VertexPositionColor::getVertexDeclarationStatic(), MaxProjectileCount * 4, BufferUsage::WriteOnly);
        data_.assign(MaxProjectileCount * 4,
                     VertexPositionColor(Vector3(0, 0, 0), Color::White));
    }
}
