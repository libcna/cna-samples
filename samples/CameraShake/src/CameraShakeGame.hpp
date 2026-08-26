// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CameraShakeGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Camera.hpp"

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"

namespace CameraShake
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    class VibrationManager;

    /**
     * @brief This is the main type for your game.
     */
    class CameraShakeGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game and registers its VibrationManager. */
        CameraShakeGame();

        /** @brief Releases the game. */
        CNAEXT ~CameraShakeGame() override;

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "CameraShake.CameraShakeGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /**
         * @brief LoadContent will be called once per game and is the place to load all of
         *        your content.
         */
        void LoadContent() override;

        /**
         * @brief Allows the game to run logic such as updating the world, checking for
         *        collisions, gathering input, and playing audio.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        void DrawInstructions();

        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;
        // XNA's SpriteFont and Model are reference types, so the original's fields are null
        // until LoadContent runs.
        std::optional<SpriteFont> font;

        // Two models that make up our scene
        std::optional<Model> ground;
        std::optional<Model> tank;

        // Our camera for viewing the scene
        Camera camera;

        // The manager is owned by Components once added, but the game creates it.
        std::unique_ptr<VibrationManager> vibration;

        // Input states for GamePad and Keyboard
        GamePadState gamePad, gamePadPrev;
        KeyboardState keyboard, keyboardPrev;
    };
}
