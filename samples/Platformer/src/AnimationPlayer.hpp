// SPDX-License-Identifier: MS-PL
#pragma once

#include "Animation.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Platformer
{
    struct AnimationPlayer
    {
        [[nodiscard]] Animation* getAnimationProperty() const;
        [[nodiscard]] int getFrameIndexProperty() const;
        [[nodiscard]] Microsoft::Xna::Framework::Vector2 getOriginProperty() const;

        void PlayAnimation(Animation* animation);
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime,
                  Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch,
                  Microsoft::Xna::Framework::Vector2 position,
                  Microsoft::Xna::Framework::Graphics::SpriteEffects spriteEffects);

    private:
        Animation* animation_ = nullptr;
        int frameIndex_ = 0;
        float time_ = 0.0f;
    };
}
