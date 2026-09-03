// SPDX-License-Identifier: MS-PL

#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Shaders/RenderToTexture.hpp"
#include "Shaders/VBScreenHelper.hpp"

namespace Microsoft::Xna::Framework::Content
{
    class ContentManager;
}

namespace RacingGame::Shaders
{
    /** @brief Runs the original four-pass Racing menu post-screen effect. */
    class PostScreenMenu
    {
    public:
        /**
         * @brief Loads the authentic PostScreenMenu effect and render targets.
         * @param device Graphics device that owns the post-screen resources.
         * @param content Content manager rooted at authentic XNA content.
         */
        PostScreenMenu(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
            Microsoft::Xna::Framework::Content::ContentManager& content);
        /** @brief Destroys post-screen resources through their XNA owners. */
        virtual ~PostScreenMenu();

        /** @brief Gets whether scene capture is currently active. */
        [[nodiscard]] bool getStartedProperty() const;
        /** @brief Gets whether new post-screen scene captures are enabled. */
        [[nodiscard]] bool getEnabledProperty() const;
        /**
         * @brief Enables or disables new post-screen scene captures.
         * @param value True to enable the original post-screen path.
         */
        void setEnabledProperty(bool value);
        /** @brief Gets the number of effect passes executed by the latest Show call. */
        [[nodiscard]] int getLastPassCountProperty() const;
        /** @brief Redirects subsequent scene rendering to the full-screen scene map. */
        void Start();
        /**
         * @brief Executes the four original menu post-screen passes.
         * @param totalTimeSeconds Total game time used by film/noise animation.
         */
        virtual void Show(float totalTimeSeconds);

    protected:
        PostScreenMenu(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
            Microsoft::Xna::Framework::Content::ContentManager& content,
            const std::string& effectAsset,
            bool loadNoiseTexture);

        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device;
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> effect;
        std::unique_ptr<RenderToTexture> sceneMapTexture;
        std::unique_ptr<RenderToTexture> downsampleMapTexture;
        std::unique_ptr<RenderToTexture> blurMap1Texture;
        std::unique_ptr<RenderToTexture> blurMap2Texture;
        VBScreenHelper screenHelper;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>
            noiseMapTexture;
        bool started = false;
        bool enabled = true;
        int lastPassCount = 0;

        void SetWindowAndSceneParameters();
        void DrawPass(int passIndex, bool useGrid);
        void RestoreAfterShow();
    };
}
