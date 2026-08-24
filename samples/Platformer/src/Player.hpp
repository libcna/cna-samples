// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>

#include "Accelerometer.hpp"
#include "Animation.hpp"
#include "AnimationPlayer.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"
#include "Microsoft/Xna/Framework/DisplayOrientation.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Platformer
{
    class Enemy;
    class Level;

    class Player
    {
    public:
        Player(Level* level, Microsoft::Xna::Framework::Vector2 position);

        [[nodiscard]] Level* getLevelProperty() const;
        [[nodiscard]] bool getIsAliveProperty() const;
        [[nodiscard]] Microsoft::Xna::Framework::Vector2 getPositionProperty() const;
        void setPositionProperty(Microsoft::Xna::Framework::Vector2 value);
        [[nodiscard]] Microsoft::Xna::Framework::Vector2 getVelocityProperty() const;
        void setVelocityProperty(Microsoft::Xna::Framework::Vector2 value);
        [[nodiscard]] bool getIsOnGroundProperty() const;
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle getBoundingRectangleProperty() const;

        void LoadContent();
        void Reset(Microsoft::Xna::Framework::Vector2 position);
        void Update(const Microsoft::Xna::Framework::GameTime& gameTime,
                    const Microsoft::Xna::Framework::Input::KeyboardState& keyboardState,
                    const Microsoft::Xna::Framework::Input::GamePadState& gamePadState,
                    const Microsoft::Xna::Framework::Input::Touch::TouchCollection& touchState,
                    const AccelerometerState& accelState,
                    Microsoft::Xna::Framework::DisplayOrientation orientation);
        void ApplyPhysics(const Microsoft::Xna::Framework::GameTime& gameTime);
        void OnKilled(Enemy* killedBy);
        void OnReachedExit();
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime,
                  Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch);

    private:
        void GetInput(const Microsoft::Xna::Framework::Input::KeyboardState& keyboardState,
                      const Microsoft::Xna::Framework::Input::GamePadState& gamePadState,
                      const Microsoft::Xna::Framework::Input::Touch::TouchCollection& touchState,
                      const AccelerometerState& accelState,
                      Microsoft::Xna::Framework::DisplayOrientation orientation);
        [[nodiscard]] float DoJump(float velocityY,
                                   const Microsoft::Xna::Framework::GameTime& gameTime);
        void HandleCollisions();

        std::optional<Animation> idleAnimation_;
        std::optional<Animation> runAnimation_;
        std::optional<Animation> jumpAnimation_;
        std::optional<Animation> celebrateAnimation_;
        std::optional<Animation> dieAnimation_;
        Microsoft::Xna::Framework::Graphics::SpriteEffects flip_ =
            Microsoft::Xna::Framework::Graphics::SpriteEffects::None;
        AnimationPlayer sprite_;

        std::optional<Microsoft::Xna::Framework::Audio::SoundEffect> killedSound_;
        std::optional<Microsoft::Xna::Framework::Audio::SoundEffect> jumpSound_;
        std::optional<Microsoft::Xna::Framework::Audio::SoundEffect> fallSound_;

        Level* level_;
        bool isAlive_ = false;
        Microsoft::Xna::Framework::Vector2 position_;
        float previousBottom_ = 0.0f;
        Microsoft::Xna::Framework::Vector2 velocity_;

        static constexpr float MoveAcceleration = 13000.0f;
        static constexpr float MaxMoveSpeed = 1750.0f;
        static constexpr float GroundDragFactor = 0.48f;
        static constexpr float AirDragFactor = 0.58f;
        static constexpr float MaxJumpTime = 0.35f;
        static constexpr float JumpLaunchVelocity = -3500.0f;
        static constexpr float GravityAcceleration = 3400.0f;
        static constexpr float MaxFallSpeed = 550.0f;
        static constexpr float JumpControlPower = 0.14f;
        static constexpr float MoveStickScale = 1.0f;
        static constexpr float AccelerometerScale = 1.5f;
        static constexpr Microsoft::Xna::Framework::Input::Buttons JumpButton =
            Microsoft::Xna::Framework::Input::Buttons::A;

        bool isOnGround_ = false;
        float movement_ = 0.0f;
        bool isJumping_ = false;
        bool wasJumping_ = false;
        float jumpTime_ = 0.0f;
        Microsoft::Xna::Framework::Rectangle localBounds_;
    };
}
