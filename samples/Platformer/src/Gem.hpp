// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>

#include "Circle.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Platformer
{
    class Level;
    class Player;

    class Gem
    {
    public:
        static constexpr int PointValue = 30;
        const Microsoft::Xna::Framework::Color Color = Microsoft::Xna::Framework::Color::Yellow;

        Gem(Level* level, Microsoft::Xna::Framework::Vector2 position);

        [[nodiscard]] Level* getLevelProperty() const;
        [[nodiscard]] Microsoft::Xna::Framework::Vector2 getPositionProperty() const;
        [[nodiscard]] Circle getBoundingCircleProperty() const;

        void LoadContent();
        void Update(const Microsoft::Xna::Framework::GameTime& gameTime);
        void OnCollected(Player* collectedBy);
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime,
                  Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch);

    private:
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> texture_;
        Microsoft::Xna::Framework::Vector2 origin_;
        std::optional<Microsoft::Xna::Framework::Audio::SoundEffect> collectedSound_;
        Microsoft::Xna::Framework::Vector2 basePosition_;
        float bounce_ = 0.0f;
        Level* level_;
    };
}
