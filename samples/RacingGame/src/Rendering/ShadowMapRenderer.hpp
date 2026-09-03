// SPDX-License-Identifier: MS-PL

#pragma once

#include <array>
#include <memory>

#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionTexture.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace Microsoft::Xna::Framework::Content
{
    class ContentManager;
}

namespace Microsoft::Xna::Framework::Graphics
{
    class GraphicsDevice;
}

namespace RacingGame::Rendering
{
    class CarRenderer;
    class StaticTrackScene;

    /** @brief Reproduces the original Racing depth-map, receiver and blur passes. */
    class ShadowMapRenderer
    {
    public:
        /**
         * @brief Loads the authentic effects and creates the original render targets.
         * @param device Graphics device owning all shadow resources.
         * @param content Content manager rooted at the authentic XNA output.
         */
        ShadowMapRenderer(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
            Microsoft::Xna::Framework::Content::ContentManager& content);

        /**
         * @brief Generates the shadow map and horizontally blurred receiver map.
         * @param scene Track, terrain and landscape-object renderer.
         * @param car Authentic car model renderer.
         * @param carMatrix Current car world transform.
         * @param carPosition Current car position.
         * @param carDirection Current car forward direction.
         * @param view Current camera view matrix.
         * @param projection Current camera projection matrix.
         * @param totalTimeSeconds Total game time for animated models.
         */
        void Prepare(
            StaticTrackScene& scene, CarRenderer& car,
            Microsoft::Xna::Framework::Matrix carMatrix,
            Microsoft::Xna::Framework::Vector3 carPosition,
            Microsoft::Xna::Framework::Vector3 carDirection,
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection,
            float totalTimeSeconds);

        /** @brief Vertically blurs and multiplicatively overlays the prepared shadow map. */
        void ShowShadows();

        /** @brief Gets caster submissions from the latest generated shadow map. */
        [[nodiscard]] int getLastCasterSubmissionCountProperty() const;
        /** @brief Gets receiver submissions from the latest shadow comparison pass. */
        [[nodiscard]] int getLastReceiverSubmissionCountProperty() const;
        /** @brief Counts non-white pixels in the generated 16-bit shadow depth map. */
        [[nodiscard]] int getShadowMapNonWhitePixelCountProperty() const;
        /** @brief Counts non-white pixels in the generated 16-bit receiver map. */
        [[nodiscard]] int getReceiverMapNonWhitePixelCountProperty() const;

    private:
        static constexpr int ShadowMapSize = 2048;
        static constexpr float ShadowNearPlane = 1.0f;
        static constexpr float ShadowFarPlane = 218.75f;
        static constexpr float ShadowDistance = 171.6f;
        static constexpr float VirtualVisibleRange = 129.25f;
        static constexpr float DepthBias = 0.00065f;

        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device;
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect>
            shadowEffect;
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect>
            blurEffect;
        Microsoft::Xna::Framework::Graphics::Texture2D fadeTexture;
        Microsoft::Xna::Framework::Graphics::RenderTarget2D shadowMap;
        Microsoft::Xna::Framework::Graphics::RenderTarget2D sceneMap;
        Microsoft::Xna::Framework::Graphics::RenderTarget2D blurMap;
        Microsoft::Xna::Framework::Graphics::VertexBuffer fullscreenQuad;
        Microsoft::Xna::Framework::Graphics::BlendState zeroSourceBlend;
        Microsoft::Xna::Framework::Matrix lightViewProjection;
        Microsoft::Xna::Framework::Matrix textureScaleBias;
        Microsoft::Xna::Framework::Vector3 shadowLightPosition =
            Microsoft::Xna::Framework::Vector3::Zero;
        int lastCasterSubmissions = 0;
        int lastReceiverSubmissions = 0;

        void SetSharedShadowParameters();
        void DrawFullscreenPass(int passIndex);
        [[nodiscard]] static int CountNonWhitePixels(
            const Microsoft::Xna::Framework::Graphics::RenderTarget2D& target);
    };
}
