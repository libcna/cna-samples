// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Platformer
{
    class Animation
    {
    public:
        Animation() = default;
        Animation(Microsoft::Xna::Framework::Graphics::Texture2D texture,
                  float frameTime, bool isLooping);

        [[nodiscard]] const Microsoft::Xna::Framework::Graphics::Texture2D& getTextureProperty() const;
        [[nodiscard]] float getFrameTimeProperty() const;
        [[nodiscard]] bool getIsLoopingProperty() const;
        [[nodiscard]] int getFrameCountProperty() const;
        [[nodiscard]] int getFrameWidthProperty() const;
        [[nodiscard]] int getFrameHeightProperty() const;

    private:
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> texture_;
        float frameTime_ = 0.0f;
        bool isLooping_ = false;
    };
}
