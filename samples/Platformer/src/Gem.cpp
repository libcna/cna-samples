// SPDX-License-Identifier: MS-PL

#include "Gem.hpp"

#include <cmath>

#include "Level.hpp"
#include "Player.hpp"
#include "Tile.hpp"

namespace Platformer
{
    Gem::Gem(Level* level, const Microsoft::Xna::Framework::Vector2 position)
        : basePosition_(position), level_(level)
    {
        LoadContent();
    }

    Level* Gem::getLevelProperty() const
    {
        return level_;
    }

    Microsoft::Xna::Framework::Vector2 Gem::getPositionProperty() const
    {
        return basePosition_ + Microsoft::Xna::Framework::Vector2(0.0f, bounce_);
    }

    Circle Gem::getBoundingCircleProperty() const
    {
        return {getPositionProperty(), Tile::Width / 3.0f};
    }

    void Gem::LoadContent()
    {
        using Microsoft::Xna::Framework::Audio::SoundEffect;
        using Microsoft::Xna::Framework::Graphics::Texture2D;

        texture_.emplace(level_->getContentProperty().Load<Texture2D>("Sprites/Gem"));
        origin_ = {texture_->getWidthProperty() / 2.0f, texture_->getHeightProperty() / 2.0f};
        collectedSound_.emplace(
            level_->getContentProperty().Load<SoundEffect>("Sounds/GemCollected"));
    }

    void Gem::Update(const Microsoft::Xna::Framework::GameTime& gameTime)
    {
        constexpr float BounceHeight = 0.18f;
        constexpr float BounceRate = 3.0f;
        constexpr float BounceSync = -0.75f;

        const double t = gameTime.getTotalGameTimeProperty().getTotalSecondsProperty() * BounceRate +
                         getPositionProperty().X * BounceSync;
        bounce_ = static_cast<float>(std::sin(t)) * BounceHeight * texture_->getHeightProperty();
    }

    void Gem::OnCollected(Player*)
    {
        collectedSound_->Play();
    }

    void Gem::Draw(const Microsoft::Xna::Framework::GameTime&,
                   Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch)
    {
        spriteBatch.Draw(*texture_, getPositionProperty(), std::nullopt, Color, 0.0f, origin_, 1.0f,
                         Microsoft::Xna::Framework::Graphics::SpriteEffects::None, 0.0f);
    }
}
