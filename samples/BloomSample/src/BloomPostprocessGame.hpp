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

#include "BloomComponent.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"

namespace BloomPostprocess
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief Sample showing how to implement a bloom postprocess, adding a glowing effect
     *        over the top of an existing scene.
     */
    class BloomPostprocessGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game and registers the bloom component. */
        BloomPostprocessGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "BloomPostprocess.BloomPostprocessGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
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
        /** Helper for drawing the spinning 3D model. */
        void DrawModel(const GameTime& gameTime);

        /**
         * Displays an overlay showing what the controls are, and which settings are
         * currently selected.
         */
        void DrawOverlayText();

        /** Handles input for quitting or changing the bloom settings. */
        void HandleInput();

        /**
         * C# gets the name of an enum value from reflection, which C++ does not have, so
         * the four names are spelled out here.
         */
        [[nodiscard]] static const std::string& ToString(BloomComponent::IntermediateBuffer value);

        GraphicsDeviceManager graphics;

        // XNA's components are reference types the Game.Components collection keeps alive.
        // The sample holds the only owning handle here and registers a raw pointer, exactly
        // as ParticleSample does.
        std::unique_ptr<BloomComponent> bloom;

        int bloomSettingsIndex = 0;

        std::unique_ptr<SpriteBatch> spriteBatch;
        // XNA's SpriteFont, Texture2D and Model are reference types, so the original's
        // fields are null until LoadContent runs.
        std::optional<SpriteFont> spriteFont;
        std::optional<Texture2D> background;
        std::optional<Model> model;

        Input::KeyboardState lastKeyboardState;
        Input::GamePadState lastGamePadState;
        Input::KeyboardState currentKeyboardState;
        Input::GamePadState currentGamePadState;
    };
}
