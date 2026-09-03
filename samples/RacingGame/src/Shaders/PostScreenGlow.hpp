// SPDX-License-Identifier: MS-PL

#pragma once

#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Shaders/PostScreenMenu.hpp"

namespace RacingGame::Shaders
{
    /** @brief Runs the original five-pass Racing in-game glow effect. */
    class PostScreenGlow final : public PostScreenMenu
    {
    public:
        /**
         * @brief Loads the authentic PostScreenGlow effect and render targets.
         * @param device Graphics device that owns the post-screen resources.
         * @param content Content manager rooted at authentic XNA content.
         */
        PostScreenGlow(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
            Microsoft::Xna::Framework::Content::ContentManager& content);

        /** @brief Gets the last radial-blur scale sent to the effect. */
        [[nodiscard]] float getRadialBlurScaleFactorProperty() const;
        /**
         * @brief Sets the radial-blur scale sent to the effect.
         * @param value New radial-blur scale.
         */
        void setRadialBlurScaleFactorProperty(float value);
        /**
         * @brief Executes all five original in-game glow passes.
         * @param speed Current player speed in metres per second.
         */
        void Show(float speed) override;

    private:
        std::unique_ptr<RenderToTexture> radialSceneMapTexture;
        Microsoft::Xna::Framework::Graphics::Texture2D
            screenBorderFadeoutMapTexture;
        Microsoft::Xna::Framework::Graphics::BlendState alphaWriteBlend;
        float lastUsedRadialBlurScaleFactor = 0.0f;
    };
}
