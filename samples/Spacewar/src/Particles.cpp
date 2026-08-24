// SPDX-License-Identifier: MS-PL

#include "Particles.hpp"

#include "Particle.hpp"
#include "SpacewarGame.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    System::Random Particles::random_;
    std::unique_ptr<SpriteBatch> Particles::batch_;

    Particles::Particles(Game* game)
        : SceneItem(game)
    {
        if (game)
            batch_ = std::make_unique<SpriteBatch>(game->getGraphicsDeviceProperty());
    }

    void Particles::Dispose()
    {
        if (batch_)
            batch_->Dispose();
        batch_.reset();
    }

    void Particles::Render()
    {
        if (getCountProperty() == 0)
            return;
        batch_->Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend, nullptr, nullptr, nullptr);
        auto texture = SpacewarGame::getContentManagerProperty().Load<Texture2D>(
            SpacewarGame::getSettingsProperty().MediaPath + particleTexture_);
        for (const auto& item : getChildrenProperty())
        {
            if (const auto* particle = dynamic_cast<const Particle*>(item.get()))
            {
                batch_->Draw(texture,
                             Vector2(item->getPositionProperty().X, item->getPositionProperty().Y),
                             std::nullopt, Color(particle->getColorProperty()), 0.0f,
                             Vector2(16.0f, 16.0f), 0.2f, SpriteEffects::None,
                             item->getPositionProperty().Z);
            }
        }
        batch_->End();
    }

    void Particles::AddShipTrail(Matrix world, Vector2 direction)
    {
        const Vector4 source = Vector4::Transform(
            Vector4(0, 0, 130000, 1), world * SpacewarGame::getCameraProperty().getViewProperty() *
            SpacewarGame::getCameraProperty().getProjectionProperty());
        const Vector2 source2D(
            static_cast<float>(static_cast<int>((source.X / source.W + 1.0f) / 2.0f * 1280.0f)),
            static_cast<float>(static_cast<int>((-source.Y / source.W + 1.0f) / 2.0f * 720.0f)));
        direction = Vector2::Normalize(direction);

        for (int i = 0; i < 70; ++i)
        {
            const float trailDistance = static_cast<float>(random_.Next(50));
            const float trailOffset = static_cast<float>(random_.Next(21) - 10);
            SceneItem::Add(std::make_unique<Particle>(
                getGameInstanceProperty(),
                Vector2(source2D.X + trailDistance * direction.X + trailOffset * direction.Y,
                        source2D.Y + trailDistance * direction.Y + trailOffset * direction.X),
                Vector2(trailDistance * trailOffset * direction.Y / 5.0f,
                        trailDistance * trailOffset * direction.X / 5.0f),
                Vector4(1.0f, 1.0f, 0.5f, 0.5f), Vector4(0.2f, 0.2f, 0.0f, 0.2f),
                System::TimeSpan(0, 0, 2)));
        }
    }

    void Particles::AddRocketTrail(Matrix world, Vector2 direction)
    {
        const Vector4 source = Vector4::Transform(
            Vector4(0, 0, 291, 1), world * SpacewarGame::getCameraProperty().getViewProperty() *
            SpacewarGame::getCameraProperty().getProjectionProperty());
        const Vector2 source2D(
            static_cast<float>(static_cast<int>((source.X / source.W + 1.0f) / 2.0f * 1280.0f)),
            static_cast<float>(static_cast<int>((-source.Y / source.W + 1.0f) / 2.0f * 720.0f)));
        direction = Vector2::Normalize(direction);
        for (int i = 0; i < 20; ++i)
        {
            const float trailDistance = static_cast<float>(random_.Next(50));
            SceneItem::Add(std::make_unique<Particle>(
                getGameInstanceProperty(), source2D + trailDistance * -direction, -direction,
                Vector4(1.0f, 1.0f, 0.5f, 0.5f), Vector4(0.2f, 0.2f, 0.0f, 0.2f),
                System::TimeSpan(0, 0, 1)));
        }
    }

    void Particles::AddExplosion(Vector3 position)
    {
        const Vector4 source = Vector4::Transform(
            position, SpacewarGame::getCameraProperty().getViewProperty() *
            SpacewarGame::getCameraProperty().getProjectionProperty());
        const Vector2 source2D(
            static_cast<float>(static_cast<int>((source.X / source.W + 1.0f) / 2.0f * 1280.0f)),
            static_cast<float>(static_cast<int>((-source.Y / source.W + 1.0f) / 2.0f * 720.0f)));
        for (int i = 0; i < 300; ++i)
        {
            const Vector2 velocity = static_cast<float>(random_.Next(100)) * Vector2::Normalize(
                Vector2(static_cast<float>(random_.NextDouble() - 0.5),
                        static_cast<float>(random_.NextDouble() - 0.5)));
            SceneItem::Add(std::make_unique<Particle>(
                getGameInstanceProperty(), source2D, velocity,
                i > 70 ? Vector4(1.0f, 0.0f, 0.0f, 1.0f) : Vector4(0.941f, 0.845f, 0.0f, 1.0f),
                Vector4(0.2f, 0.2f, 0.2f, 0.0f),
                System::TimeSpan(0, 0, 0, 0, random_.Next(1000) + 500)));
        }
    }

    void Particles::OnCreateDevice()
    {
        batch_ = std::make_unique<SpriteBatch>(getGameInstanceProperty()->getGraphicsDeviceProperty());
    }
}
