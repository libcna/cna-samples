// SPDX-License-Identifier: MS-PL

#include "Player.hpp"

#include <cmath>

#include "Enemy.hpp"
#include "Level.hpp"
#include "RectangleExtensions.hpp"
#include "Tile.hpp"
#include "TouchCollectionExtensions.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace Platformer
{
    Player::Player(Level* level, const Microsoft::Xna::Framework::Vector2 position)
        : level_(level)
    {
        LoadContent();
        Reset(position);
    }

    Level* Player::getLevelProperty() const
    {
        return level_;
    }

    bool Player::getIsAliveProperty() const
    {
        return isAlive_;
    }

    Microsoft::Xna::Framework::Vector2 Player::getPositionProperty() const
    {
        return position_;
    }

    void Player::setPositionProperty(const Microsoft::Xna::Framework::Vector2 value)
    {
        position_ = value;
    }

    Microsoft::Xna::Framework::Vector2 Player::getVelocityProperty() const
    {
        return velocity_;
    }

    void Player::setVelocityProperty(const Microsoft::Xna::Framework::Vector2 value)
    {
        velocity_ = value;
    }

    bool Player::getIsOnGroundProperty() const
    {
        return isOnGround_;
    }

    Microsoft::Xna::Framework::Rectangle Player::getBoundingRectangleProperty() const
    {
        const int left = static_cast<int>(std::round(position_.X - sprite_.getOriginProperty().X)) +
                         localBounds_.X;
        const int top = static_cast<int>(std::round(position_.Y - sprite_.getOriginProperty().Y)) +
                        localBounds_.Y;
        return {left, top, localBounds_.Width, localBounds_.Height};
    }

    void Player::LoadContent()
    {
        using Microsoft::Xna::Framework::Audio::SoundEffect;
        using Microsoft::Xna::Framework::Graphics::Texture2D;

        auto& content = level_->getContentProperty();
        idleAnimation_.emplace(
            content.Load<Texture2D>("Sprites/Player/Idle"), 0.1f, true);
        runAnimation_.emplace(
            content.Load<Texture2D>("Sprites/Player/Run"), 0.1f, true);
        jumpAnimation_.emplace(
            content.Load<Texture2D>("Sprites/Player/Jump"), 0.1f, false);
        celebrateAnimation_.emplace(
            content.Load<Texture2D>("Sprites/Player/Celebrate"), 0.1f, false);
        dieAnimation_.emplace(
            content.Load<Texture2D>("Sprites/Player/Die"), 0.1f, false);

        const int width = static_cast<int>(idleAnimation_->getFrameWidthProperty() * 0.4f);
        const int left = (idleAnimation_->getFrameWidthProperty() - width) / 2;
        const int height = static_cast<int>(idleAnimation_->getFrameWidthProperty() * 0.8f);
        const int top = idleAnimation_->getFrameHeightProperty() - height;
        localBounds_ = {left, top, width, height};

        killedSound_.emplace(content.Load<SoundEffect>("Sounds/PlayerKilled"));
        jumpSound_.emplace(content.Load<SoundEffect>("Sounds/PlayerJump"));
        fallSound_.emplace(content.Load<SoundEffect>("Sounds/PlayerFall"));
    }

    void Player::Reset(const Microsoft::Xna::Framework::Vector2 position)
    {
        position_ = position;
        velocity_ = Microsoft::Xna::Framework::Vector2::Zero;
        isAlive_ = true;
        sprite_.PlayAnimation(&*idleAnimation_);
    }

    void Player::Update(
        const Microsoft::Xna::Framework::GameTime& gameTime,
        const Microsoft::Xna::Framework::Input::KeyboardState& keyboardState,
        const Microsoft::Xna::Framework::Input::GamePadState& gamePadState,
        const Microsoft::Xna::Framework::Input::Touch::TouchCollection& touchState,
        const AccelerometerState& accelState,
        const Microsoft::Xna::Framework::DisplayOrientation orientation)
    {
        GetInput(keyboardState, gamePadState, touchState, accelState, orientation);
        ApplyPhysics(gameTime);

        if (isAlive_ && isOnGround_)
        {
            if (std::abs(velocity_.X) - 0.02f > 0.0f)
                sprite_.PlayAnimation(&*runAnimation_);
            else
                sprite_.PlayAnimation(&*idleAnimation_);
        }

        movement_ = 0.0f;
        isJumping_ = false;
    }

    void Player::GetInput(
        const Microsoft::Xna::Framework::Input::KeyboardState& keyboardState,
        const Microsoft::Xna::Framework::Input::GamePadState& gamePadState,
        const Microsoft::Xna::Framework::Input::Touch::TouchCollection& touchState,
        const AccelerometerState& accelState,
        const Microsoft::Xna::Framework::DisplayOrientation orientation)
    {
        using namespace Microsoft::Xna::Framework;
        using namespace Microsoft::Xna::Framework::Input;

        movement_ = gamePadState.getThumbSticksProperty().getLeftProperty().X * MoveStickScale;
        if (std::abs(movement_) < 0.5f)
            movement_ = 0.0f;

        if (std::abs(accelState.getAccelerationProperty().Y) > 0.10f)
        {
            movement_ = MathHelper::Clamp(
                -accelState.getAccelerationProperty().Y * AccelerometerScale, -1.0f, 1.0f);
            if (orientation == DisplayOrientation::LandscapeRight)
                movement_ = -movement_;
        }

        if (gamePadState.IsButtonDown(Buttons::DPadLeft) ||
            keyboardState.IsKeyDown(Keys::Left) || keyboardState.IsKeyDown(Keys::A))
            movement_ = -1.0f;
        else if (gamePadState.IsButtonDown(Buttons::DPadRight) ||
                 keyboardState.IsKeyDown(Keys::Right) || keyboardState.IsKeyDown(Keys::D))
            movement_ = 1.0f;

        isJumping_ = gamePadState.IsButtonDown(JumpButton) ||
                     keyboardState.IsKeyDown(Keys::Space) ||
                     keyboardState.IsKeyDown(Keys::Up) || keyboardState.IsKeyDown(Keys::W) ||
                     TouchCollectionExtensions::AnyTouch(touchState);
    }

    void Player::ApplyPhysics(const Microsoft::Xna::Framework::GameTime& gameTime)
    {
        using namespace Microsoft::Xna::Framework;

        const float elapsed = static_cast<float>(
            gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());
        const Vector2 previousPosition = position_;

        velocity_.X += movement_ * MoveAcceleration * elapsed;
        velocity_.Y = MathHelper::Clamp(velocity_.Y + GravityAcceleration * elapsed,
                                        -MaxFallSpeed, MaxFallSpeed);
        velocity_.Y = DoJump(velocity_.Y, gameTime);

        if (isOnGround_)
            velocity_.X *= GroundDragFactor;
        else
            velocity_.X *= AirDragFactor;

        velocity_.X = MathHelper::Clamp(velocity_.X, -MaxMoveSpeed, MaxMoveSpeed);
        position_ = position_ + velocity_ * elapsed;
        position_ = {static_cast<float>(std::round(position_.X)),
                     static_cast<float>(std::round(position_.Y))};

        HandleCollisions();

        if (position_.X == previousPosition.X)
            velocity_.X = 0.0f;
        if (position_.Y == previousPosition.Y)
            velocity_.Y = 0.0f;
    }

    float Player::DoJump(float velocityY, const Microsoft::Xna::Framework::GameTime& gameTime)
    {
        if (isJumping_)
        {
            if ((!wasJumping_ && isOnGround_) || jumpTime_ > 0.0f)
            {
                if (jumpTime_ == 0.0f)
                    jumpSound_->Play();
                jumpTime_ += static_cast<float>(
                    gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());
                sprite_.PlayAnimation(&*jumpAnimation_);
            }

            if (0.0f < jumpTime_ && jumpTime_ <= MaxJumpTime)
                velocityY = JumpLaunchVelocity *
                            (1.0f - static_cast<float>(
                                        std::pow(jumpTime_ / MaxJumpTime, JumpControlPower)));
            else
                jumpTime_ = 0.0f;
        }
        else
        {
            jumpTime_ = 0.0f;
        }

        wasJumping_ = isJumping_;
        return velocityY;
    }

    void Player::HandleCollisions()
    {
        using Microsoft::Xna::Framework::Rectangle;
        using Microsoft::Xna::Framework::Vector2;

        Rectangle bounds = getBoundingRectangleProperty();
        const int leftTile = static_cast<int>(
            std::floor(static_cast<float>(bounds.getLeftProperty()) / Tile::Width));
        const int rightTile = static_cast<int>(
                                  std::ceil(static_cast<float>(bounds.getRightProperty()) /
                                            Tile::Width)) -
                              1;
        const int topTile = static_cast<int>(
            std::floor(static_cast<float>(bounds.getTopProperty()) / Tile::Height));
        const int bottomTile = static_cast<int>(
                                   std::ceil(static_cast<float>(bounds.getBottomProperty()) /
                                             Tile::Height)) -
                               1;

        isOnGround_ = false;
        for (int y = topTile; y <= bottomTile; ++y)
        {
            for (int x = leftTile; x <= rightTile; ++x)
            {
                const TileCollision collision = level_->GetCollision(x, y);
                if (collision == TileCollision::Passable)
                    continue;

                const Rectangle tileBounds = level_->GetBounds(x, y);
                const Vector2 depth = RectangleExtensions::GetIntersectionDepth(bounds, tileBounds);
                if (depth == Vector2::Zero)
                    continue;

                const float absDepthX = std::abs(depth.X);
                const float absDepthY = std::abs(depth.Y);
                if (absDepthY < absDepthX || collision == TileCollision::Platform)
                {
                    if (previousBottom_ <= tileBounds.getTopProperty())
                        isOnGround_ = true;

                    if (collision == TileCollision::Impassable || isOnGround_)
                    {
                        position_ = {position_.X, position_.Y + depth.Y};
                        bounds = getBoundingRectangleProperty();
                    }
                }
                else if (collision == TileCollision::Impassable)
                {
                    position_ = {position_.X + depth.X, position_.Y};
                    bounds = getBoundingRectangleProperty();
                }
            }
        }

        previousBottom_ = static_cast<float>(bounds.getBottomProperty());
    }

    void Player::OnKilled(Enemy* killedBy)
    {
        isAlive_ = false;
        if (killedBy != nullptr)
            killedSound_->Play();
        else
            fallSound_->Play();
        sprite_.PlayAnimation(&*dieAnimation_);
    }

    void Player::OnReachedExit()
    {
        sprite_.PlayAnimation(&*celebrateAnimation_);
    }

    void Player::Draw(const Microsoft::Xna::Framework::GameTime& gameTime,
                      Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch)
    {
        if (velocity_.X > 0.0f)
            flip_ = Microsoft::Xna::Framework::Graphics::SpriteEffects::FlipHorizontally;
        else if (velocity_.X < 0.0f)
            flip_ = Microsoft::Xna::Framework::Graphics::SpriteEffects::None;

        sprite_.Draw(gameTime, spriteBatch, position_, flip_);
    }
}
