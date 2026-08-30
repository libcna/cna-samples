// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "AudioManager.hpp"
#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"

namespace Audio3D
{
    class QuadDrawer;
    class SpriteEntity;

    /** @brief Sample showing how to implement 3D audio. */
    class Audio3DGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game, graphics manager, audio manager, and entities. */
        Audio3DGame();

        /** @brief Destroys the game after its forward-declared owned helpers are complete. */
        ~Audio3DGame() override;

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `Audio3D.Audio3DGame`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads the cat, dog, and checker textures and creates the quad drawer. */
        void LoadContent() override;

        /**
         * @brief Handles input, updates the camera listener, and advances both entities.
         * @param gameTime Timing information for the current frame.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Draws the ground and both billboard entities.
         * @param gameTime Timing information for the current frame.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        Microsoft::Xna::Framework::GraphicsDeviceManager graphics;
        std::unique_ptr<AudioManager> audioManager;
        std::unique_ptr<SpriteEntity> cat;
        std::unique_ptr<SpriteEntity> dog;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> catTexture;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> dogTexture;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> checkerTexture;
        std::unique_ptr<QuadDrawer> quadDrawer;

        Microsoft::Xna::Framework::Vector3 cameraPosition =
            Microsoft::Xna::Framework::Vector3(0.0f, 512.0f, 0.0f);
        Microsoft::Xna::Framework::Vector3 cameraForward =
            Microsoft::Xna::Framework::Vector3::Forward;
        Microsoft::Xna::Framework::Vector3 cameraUp =
            Microsoft::Xna::Framework::Vector3::Up;
        Microsoft::Xna::Framework::Vector3 cameraVelocity =
            Microsoft::Xna::Framework::Vector3::Zero;

        Microsoft::Xna::Framework::Input::KeyboardState currentKeyboardState;
        Microsoft::Xna::Framework::Input::GamePadState currentGamePadState;

        void HandleInput();
        void UpdateCamera();
    };
}
