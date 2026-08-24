// SPDX-License-Identifier: MS-PL

#include "RetroStarfield.hpp"

#include <vector>

#include "SpacewarGame.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include "System/Random.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    RetroStarfield::RetroStarfield(Game* game)
        : Shape(game)
    {
        Create();
    }

    void RetroStarfield::Create() { OnCreateDevice(); }

    void RetroStarfield::Render()
    {
        auto& device = getGameInstanceProperty()->getGraphicsDeviceProperty();
        device.SetVertexBuffer(buffer_.get());
        worldViewProjectionParam_->SetValue(
            world_ * SpacewarGame::getCameraProperty().getViewProperty() *
            SpacewarGame::getCameraProperty().getProjectionProperty());
        effect_->getTechniquesProperty()[0].getPassesProperty()[0].Apply();
        device.DrawPrimitives(PrimitiveType::TriangleList, 0, NumberOfTriangles);
    }

    void RetroStarfield::OnCreateDevice()
    {
        effect_ = SpacewarGame::getContentManagerProperty().Load<std::shared_ptr<Effect>>(
            SpacewarGame::getSettingsProperty().MediaPath + "shaders/simple");
        worldViewProjectionParam_ = effect_->getParametersProperty()["worldViewProjection"];
        buffer_ = std::make_unique<VertexBuffer>(getGameInstanceProperty()->getGraphicsDeviceProperty(),
            VertexPositionColor::getVertexDeclarationStatic(), NumberOfPoints, BufferUsage::WriteOnly);

        std::vector<VertexPositionColor> data(NumberOfPoints);
        int pointCount = 0;
        int triangleCount = 0;
        System::Random random;
        while (triangleCount < NumberOfTriangles)
        {
            const auto greyValue = static_cast<SharpRuntime::bytecs>(random.Next(200) + 56);
            const Color color(greyValue, greyValue, greyValue);
            const Vector2 position(static_cast<float>(random.Next(560) - 280),
                                   static_cast<float>(random.Next(420) - 210));
            if (random.Next(100) < PercentBigStars && triangleCount + 2 < NumberOfTriangles)
            {
                data[pointCount++] = VertexPositionColor(Vector3(position.X, position.Y, 0), color);
                data[pointCount++] = VertexPositionColor(Vector3(position.X - 1.0f, position.Y, 0), color);
                data[pointCount++] = VertexPositionColor(Vector3(position.X, position.Y + 1.0f, 0), color);
                data[pointCount++] = VertexPositionColor(Vector3(position.X - 1.0f, position.Y, 0), color);
                data[pointCount++] = VertexPositionColor(Vector3(position.X - 1.0f, position.Y + 1.0f, 0), color);
                data[pointCount++] = VertexPositionColor(Vector3(position.X, position.Y + 1.0f, 0), color);
                triangleCount += 2;
            }
            else
            {
                data[pointCount++] = VertexPositionColor(Vector3(position.X, position.Y, 0), color);
                data[pointCount++] = VertexPositionColor(Vector3(position.X - 0.5f, position.Y, 0), color);
                data[pointCount++] = VertexPositionColor(Vector3(position.X, position.Y + 0.5f, 0), color);
                ++triangleCount;
            }
        }
        buffer_->SetData(data.data(), static_cast<int>(data.size()));
    }
}
