// SPDX-License-Identifier: MS-PL

#include "Animation.hpp"

#include <utility>

namespace Platformer
{
    Animation::Animation(Microsoft::Xna::Framework::Graphics::Texture2D texture,
                         const float frameTime, const bool isLooping)
        : texture_(std::move(texture)), frameTime_(frameTime), isLooping_(isLooping)
    {
    }

    const Microsoft::Xna::Framework::Graphics::Texture2D& Animation::getTextureProperty() const
    {
        return *texture_;
    }

    float Animation::getFrameTimeProperty() const
    {
        return frameTime_;
    }

    bool Animation::getIsLoopingProperty() const
    {
        return isLooping_;
    }

    int Animation::getFrameCountProperty() const
    {
        return getTextureProperty().getWidthProperty() / getFrameWidthProperty();
    }

    int Animation::getFrameWidthProperty() const
    {
        return getTextureProperty().getHeightProperty();
    }

    int Animation::getFrameHeightProperty() const
    {
        return getTextureProperty().getHeightProperty();
    }
}
