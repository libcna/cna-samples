// SPDX-License-Identifier: MS-PL

#pragma once

#include <memory>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class GraphicsDevice;
}

namespace RacingGame::Shaders
{
    /** @brief Owns an original Racing render-to-texture surface. */
    class RenderToTexture
    {
    public:
        /** @brief Selects an original render-target size policy. */
        enum class SizeType
        {
            /** @brief Uses the complete viewport size. */
            FullScreen,
            /** @brief Uses half of the viewport size. */
            HalfScreen,
            /** @brief Uses one quarter of the viewport size. */
            QuarterScreen,
            /** @brief Uses the original high-detail shadow-map size. */
            ShadowMap,
        };

        /**
         * @brief Creates a render target using the selected original size policy.
         * @param device Graphics device that owns the target.
         * @param sizeType Size policy to apply.
         */
        RenderToTexture(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
            SizeType sizeType);

        /** @brief Gets the owned XNA render target. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::RenderTarget2D&
        getRenderTargetProperty() const;
        /** @brief Gets the render target through its Texture2D surface. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::Texture2D&
        getXnaTextureProperty() const;
        /** @brief Gets whether the selected target format is more precise than Color. */
        [[nodiscard]] bool getUsesHighPrecisionFormatProperty() const;
        /** @brief Gets the current render-target width. */
        [[nodiscard]] int getWidthProperty() const;
        /** @brief Gets the current render-target height. */
        [[nodiscard]] int getHeightProperty() const;

        /** @brief Recreates the render target after a viewport/device reset. */
        void HandleDeviceReset();
        /**
         * @brief Clears the currently bound target.
         * @param clearColor Color used to clear target and depth.
         */
        void Clear(Microsoft::Xna::Framework::Color clearColor);
        /**
         * @brief Binds this render target.
         * @return True when the target was bound.
         */
        bool SetRenderTarget();
        /** @brief Resolves this currently bound target and restores the backbuffer. */
        void Resolve();

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device;
        SizeType sizeType;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::RenderTarget2D>
            renderTarget;
        bool usesHighPrecisionFormat = false;

        void Create();
    };
}
