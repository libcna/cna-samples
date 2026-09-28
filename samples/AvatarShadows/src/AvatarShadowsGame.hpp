// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AvatarShadowGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Avatar.hpp"
#include "CNA/CNAHelper.hpp"
#include "GroundEffect.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace Microsoft::Xna::Framework::GamerServices
{
    class GamerServicesComponent;
}

namespace AvatarShadows
{
    /**
     * @brief This is the main type for your game.
     */
    class AvatarShadowsGame final : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Creates the graphics device manager and adds gamer services for avatars. */
        AvatarShadowsGame();

        /** @brief Releases the avatars, render target and components. */
        ~AvatarShadowsGame() override;

        /**
         * @brief Returns the fully qualified managed type name.
         *
         * @return The managed type name used by SharpRuntime type identity.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads content, builds the ground, creates the avatars and the shadow target. */
        void LoadContent() override;

        /**
         * @brief Reads input, rotates camera and light, and advances every animation.
         *
         * @param gameTime Timing information for the current update.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Draws the shadows, the ground, the avatars and the instructions.
         *
         * @param gameTime Timing information for the current frame.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        std::unique_ptr<Microsoft::Xna::Framework::GraphicsDeviceManager> graphics;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> font;

        // The ground beneath the avatar and the effect we use to render it
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> groundVertices;
        std::unique_ptr<GroundEffect> groundEffect;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> groundTexture;

        // Our list of avatars
        std::vector<std::unique_ptr<Avatar>> avatars;

        // The rotation of the camera
        float cameraRotation = 0.0f;

        // The rotation of the light
        float lightRotation;

        // States used for our input
        Microsoft::Xna::Framework::Input::GamePadState gamePad;
        Microsoft::Xna::Framework::Input::GamePadState gamePadPrev;

        // Our render target that will hold the avatar shadows
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::RenderTarget2D> shadowTarget;

        std::unique_ptr<Microsoft::Xna::Framework::GamerServices::GamerServicesComponent> gamerServices;

        void DrawAvatarShadows(
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection,
            const Microsoft::Xna::Framework::Vector3& lightDirection);
        void DrawGround(
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection);
        void DrawInstructions();
    };
}
