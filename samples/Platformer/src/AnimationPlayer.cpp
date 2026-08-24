// SPDX-License-Identifier: MS-PL

#include "AnimationPlayer.hpp"

#include <algorithm>
#include <optional>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "System/NotSupportedException.hpp"

namespace Platformer
{
    Animation* AnimationPlayer::getAnimationProperty() const
    {
        return animation_;
    }

    int AnimationPlayer::getFrameIndexProperty() const
    {
        return frameIndex_;
    }

    Microsoft::Xna::Framework::Vector2 AnimationPlayer::getOriginProperty() const
    {
        return {animation_->getFrameWidthProperty() / 2.0f,
                static_cast<float>(animation_->getFrameHeightProperty())};
    }

    void AnimationPlayer::PlayAnimation(Animation* animation)
    {
        if (animation_ == animation)
            return;

        animation_ = animation;
        frameIndex_ = 0;
        time_ = 0.0f;
    }

    void AnimationPlayer::Draw(
        const Microsoft::Xna::Framework::GameTime& gameTime,
        Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch,
        const Microsoft::Xna::Framework::Vector2 position,
        const Microsoft::Xna::Framework::Graphics::SpriteEffects spriteEffects)
    {
        using namespace Microsoft::Xna::Framework;
        using namespace Microsoft::Xna::Framework::Graphics;

        if (animation_ == nullptr)
            throw System::NotSupportedException("No animation is currently playing.");

        time_ += static_cast<float>(gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());
        while (time_ > animation_->getFrameTimeProperty())
        {
            time_ -= animation_->getFrameTimeProperty();
            if (animation_->getIsLoopingProperty())
                frameIndex_ = (frameIndex_ + 1) % animation_->getFrameCountProperty();
            else
                frameIndex_ = std::min(frameIndex_ + 1, animation_->getFrameCountProperty() - 1);
        }

        const int frameSize = animation_->getTextureProperty().getHeightProperty();
        const Rectangle source(frameIndex_ * frameSize, 0, frameSize, frameSize);
        spriteBatch.Draw(animation_->getTextureProperty(), position, std::make_optional(source),
                         Color::White, 0.0f, getOriginProperty(), 1.0f, spriteEffects, 0.0f);
    }
}
