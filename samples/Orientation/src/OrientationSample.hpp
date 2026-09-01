// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace OrientationSample
{
    /**
     * @brief Demonstrates the Windows Phone orientation configuration scenarios.
     */
    class OrientationSample final : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Creates the orientation sample in its shipped landscape-only configuration. */
        OrientationSample();

        /**
         * @brief Returns the fully qualified managed type name.
         *
         * @return The managed type name used by SharpRuntime reflection-free type identity.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Enables the tap gesture used by the optional fourth tutorial scenario. */
        void Initialize() override;

        /** @brief Loads the original directions texture and sprite font. */
        void LoadContent() override;

        /** @brief Completes the sample's content-unload phase. */
        void UnloadContent() override;

        /**
         * @brief Updates exit handling and the optional orientation-lock scenario.
         *
         * @param gameTime Timing information for the current update.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Draws the centered orientation guide and optional lock instructions.
         *
         * @param gameTime Timing information for the current frame.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        Microsoft::Xna::Framework::GraphicsDeviceManager graphics_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch_;

        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> directions_;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> font_;

        bool orientationLocked_ = false;
        bool enableOrientationLocking_ = false;
    };
}
