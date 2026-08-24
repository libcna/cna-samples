// SPDX-License-Identifier: MS-PL

#include "Enemy.hpp"

#include <algorithm>
#include <cmath>

#include "Level.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/TimeSpan.hpp"

namespace Platformer
{
    Enemy::Enemy(Level* level, const Microsoft::Xna::Framework::Vector2 position,
                 const std::string& spriteSet)
        : level_(level), position_(position)
    {
        LoadContent(spriteSet);
    }

    Level* Enemy::getLevelProperty() const
    {
        return level_;
    }

    Microsoft::Xna::Framework::Vector2 Enemy::getPositionProperty() const
    {
        return position_;
    }

    Microsoft::Xna::Framework::Rectangle Enemy::getBoundingRectangleProperty() const
    {
        const int left = static_cast<int>(std::round(position_.X - sprite_.getOriginProperty().X)) +
                         localBounds_.X;
        const int top = static_cast<int>(std::round(position_.Y - sprite_.getOriginProperty().Y)) +
                        localBounds_.Y;
        return {left, top, localBounds_.Width, localBounds_.Height};
    }

    void Enemy::LoadContent(const std::string& spriteSet)
    {
        using Microsoft::Xna::Framework::Graphics::Texture2D;

        const std::string path = "Sprites/" + spriteSet + "/";
        runAnimation_.emplace(
            level_->getContentProperty().Load<Texture2D>(path + "Run"), 0.1f, true);
        idleAnimation_.emplace(
            level_->getContentProperty().Load<Texture2D>(path + "Idle"), 0.15f, true);
        sprite_.PlayAnimation(&*idleAnimation_);

        const int width = static_cast<int>(idleAnimation_->getFrameWidthProperty() * 0.35f);
        const int left = (idleAnimation_->getFrameWidthProperty() - width) / 2;
        const int height = static_cast<int>(idleAnimation_->getFrameWidthProperty() * 0.7f);
        const int top = idleAnimation_->getFrameHeightProperty() - height;
        localBounds_ = {left, top, width, height};
    }

    void Enemy::Update(const Microsoft::Xna::Framework::GameTime& gameTime)
    {
        const float elapsed = static_cast<float>(
            gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());
        const float posX = position_.X + localBounds_.Width / 2.0f * static_cast<int>(direction_);
        const int tileX = static_cast<int>(std::floor(posX / Tile::Width)) -
                          static_cast<int>(direction_);
        const int tileY = static_cast<int>(std::floor(position_.Y / Tile::Height));

        if (waitTime_ > 0.0f)
        {
            waitTime_ = std::max(0.0f, waitTime_ - elapsed);
            if (waitTime_ <= 0.0f)
                direction_ = static_cast<FaceDirection>(-static_cast<int>(direction_));
        }
        else if (level_->GetCollision(tileX + static_cast<int>(direction_), tileY - 1) ==
                     TileCollision::Impassable ||
                 level_->GetCollision(tileX + static_cast<int>(direction_), tileY) ==
                     TileCollision::Passable)
        {
            waitTime_ = MaxWaitTime;
        }
        else
        {
            const Microsoft::Xna::Framework::Vector2 velocity(
                static_cast<int>(direction_) * MoveSpeed * elapsed, 0.0f);
            position_ = position_ + velocity;
        }
    }

    void Enemy::Draw(const Microsoft::Xna::Framework::GameTime& gameTime,
                     Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch)
    {
        using Microsoft::Xna::Framework::Graphics::SpriteEffects;

        if (!level_->getPlayerProperty()->getIsAliveProperty() ||
            level_->getReachedExitProperty() ||
            level_->getTimeRemainingProperty() == System::TimeSpan::Zero || waitTime_ > 0.0f)
            sprite_.PlayAnimation(&*idleAnimation_);
        else
            sprite_.PlayAnimation(&*runAnimation_);

        const SpriteEffects flip = static_cast<int>(direction_) > 0
                                       ? SpriteEffects::FlipHorizontally
                                       : SpriteEffects::None;
        sprite_.Draw(gameTime, spriteBatch, position_, flip);
    }
}
