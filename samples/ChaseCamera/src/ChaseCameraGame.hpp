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

#include "ChaseCamera.hpp"
#include "Ship.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"

namespace ChaseCameraSample
{
    /** @brief Sample showing how to implement a simple chase camera. */
    class ChaseCameraGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game and configures its camera and display. */
        ChaseCameraGame();

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `ChaseCameraSample.ChaseCameraGame`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Creates the ship and resets the initialized chase camera. */
        void Initialize() override;

        /** @brief Loads the font, ship model, and ground model. */
        void LoadContent() override;

        /**
         * @brief Handles input and advances ship and chase-camera simulation.
         * @param gameTime Timing information for the current frame.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Draws the ship, ground, and control overlay.
         * @param gameTime Timing information for the current frame.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        Microsoft::Xna::Framework::GraphicsDeviceManager graphics;

        std::optional<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> spriteFont;

        Microsoft::Xna::Framework::Input::KeyboardState lastKeyboardState;
        Microsoft::Xna::Framework::Input::GamePadState lastGamePadState;
        Microsoft::Xna::Framework::Input::MouseState lastMousState;
        Microsoft::Xna::Framework::Input::KeyboardState currentKeyboardState;
        Microsoft::Xna::Framework::Input::GamePadState currentGamePadState;
        Microsoft::Xna::Framework::Input::MouseState currentMouseState;

        std::unique_ptr<Ship> ship;
        std::unique_ptr<ChaseCamera> camera;

        std::optional<Microsoft::Xna::Framework::Graphics::Model> shipModel;
        std::optional<Microsoft::Xna::Framework::Graphics::Model> groundModel;

        bool cameraSpringEnabled = true;

        void UpdateCameraChaseTarget();
        void DrawModel(
            Microsoft::Xna::Framework::Graphics::Model& model,
            const Microsoft::Xna::Framework::Matrix& world);
        void DrawOverlayText();
    };
}
