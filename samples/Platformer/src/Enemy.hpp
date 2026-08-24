// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>
#include <string>

#include "Animation.hpp"
#include "AnimationPlayer.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Platformer
{
    class Level;

    enum class FaceDirection
    {
        Left = -1,
        Right = 1,
    };

    class Enemy
    {
    public:
        Enemy(Level* level, Microsoft::Xna::Framework::Vector2 position,
              const std::string& spriteSet);

        [[nodiscard]] Level* getLevelProperty() const;
        [[nodiscard]] Microsoft::Xna::Framework::Vector2 getPositionProperty() const;
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle getBoundingRectangleProperty() const;

        void LoadContent(const std::string& spriteSet);
        void Update(const Microsoft::Xna::Framework::GameTime& gameTime);
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime,
                  Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch);

    private:
        Level* level_;
        Microsoft::Xna::Framework::Vector2 position_;
        Microsoft::Xna::Framework::Rectangle localBounds_;
        std::optional<Animation> runAnimation_;
        std::optional<Animation> idleAnimation_;
        AnimationPlayer sprite_;
        FaceDirection direction_ = FaceDirection::Left;
        float waitTime_ = 0.0f;

        static constexpr float MaxWaitTime = 0.5f;
        static constexpr float MoveSpeed = 64.0f;
    };
}
