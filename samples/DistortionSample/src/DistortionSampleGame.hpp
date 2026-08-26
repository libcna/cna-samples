// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include <memory>
#include <optional>
#include <string>

#include "Distorter.hpp"
#include "DistortionComponent.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"

namespace DistortionSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief This sample demonstrates a variety of image-distorting post-processing
     *        techniques.
     */
    class DistortionSampleGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game and registers the distortion component. */
        DistortionSampleGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "DistortionSample.DistortionSampleGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Creates the distorter table once the graphics device exists. */
        void Initialize() override;

        /** @brief Load your graphics content. */
        void LoadContent() override;

        /**
         * @brief Allows the game to run logic.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        /**
         * Displays an overlay showing what the controls are, and which settings are
         * currently selected.
         */
        void DrawOverlayText();

        /** Handles input for quitting or changing the sample settings. */
        void HandleInput();

        GraphicsDeviceManager graphics;

        static constexpr float initialViewAngle = MathHelper::Pi / 2.0f;
        float viewAngle = initialViewAngle;
        static constexpr float CameraRotationSpeed = 0.1f;
        static constexpr float ViewDistance = 750.0f;

        // XNA's components are reference types the Game.Components collection keeps alive.
        // The sample holds the only owning handle here and registers a raw pointer.
        std::unique_ptr<DistortionComponent> distortionComponent;
        std::array<Distorter, 3> distorters;
        int currentDistorter = 0;

        std::unique_ptr<SpriteBatch> spriteBatch;
        // XNA's SpriteFont and Texture2D are reference types, so the original's fields are
        // null until LoadContent runs.
        std::optional<SpriteFont> spriteFont;
        std::optional<Texture2D> background;
        Vector2 overlayTextLocation;

        Input::KeyboardState lastKeyboardState;
        Input::GamePadState lastGamePadState;
        Input::KeyboardState currentKeyboardState;
        Input::GamePadState currentGamePadState;
    };
}
