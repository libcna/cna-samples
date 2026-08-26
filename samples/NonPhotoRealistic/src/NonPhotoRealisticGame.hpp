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
#include <vector>

#include "NonPhotoRealisticSettings.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "System/Random.hpp"
#include "System/TimeSpan.hpp"

namespace NonPhotoRealistic
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief Sample showing how to implement non-photorealistic rendering techniques,
     *        providing a cartoon shader, edge detection, and pencil sketch rendering effect.
     */
    class NonPhotoRealisticGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game. */
        NonPhotoRealisticGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "NonPhotoRealistic.NonPhotoRealisticGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Load your graphics content. */
        void LoadContent() override;

        /** @brief Unload your graphics content. */
        void UnloadContent() override;

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
        /** Choose what display settings to use. */
        [[nodiscard]] const NonPhotoRealisticSettings& getSettingsProperty() const;

        /**
         * Alters a model so it will draw using a custom effect, while preserving whatever
         * textures were set on it as part of the original effects.
         *
         * Not static as in the original: XNA's Dictionary keeps the clones alive for as long
         * as the model references them, and CNA's ModelMeshPart holds a non-owning Effect*
         * while Clone() hands back an owning raw pointer, so the game owns them instead.
         */
        void ChangeEffectUsedByModel(Model& model, Effect& replacementEffect);

        /** Helper for drawing the spinning model using the specified effect technique. */
        void DrawModel(const Matrix& world, const Matrix& view, const Matrix& projection,
                       const String& effectTechniqueName);

        /** Helper applies the edge detection and pencil sketch postprocess effect. */
        void ApplyPostprocess();

        /**
         * Displays an overlay showing what the controls are, and which settings are
         * currently selected.
         */
        void DrawOverlayText();

        /** Handles input for quitting or changing the display settings. */
        void HandleInput();

        GraphicsDeviceManager graphics;

        std::unique_ptr<SpriteBatch> spriteBatch;
        // XNA's SpriteFont, Model, Effect and Texture2D are reference types, so the
        // original's fields are null until LoadContent runs.
        std::optional<SpriteFont> spriteFont;
        std::optional<Model> model;

        System::Random random;

        // Effect used to apply the edge detection and pencil sketch postprocessing.
        std::shared_ptr<Effect> postprocessEffect;

        // Overlay texture containing the pencil sketch stroke pattern.
        std::optional<Texture2D> sketchTexture;

        // Randomly offsets the sketch pattern to create a hand-drawn animation effect.
        Vector2 sketchJitter;
        System::TimeSpan timeToNextJitter;

        // Custom rendertargets.
        std::unique_ptr<RenderTarget2D> sceneRenderTarget;
        std::unique_ptr<RenderTarget2D> normalDepthRenderTarget;

        // The replacement effects the model's mesh parts point at. XNA's local Dictionary
        // owns these for the model's lifetime; here the game does.
        std::vector<std::unique_ptr<Effect>> replacementEffects;

        int settingsIndex = 0;

        // Current and previous input states.
        Input::KeyboardState lastKeyboardState;
        Input::GamePadState lastGamePadState;
        Input::KeyboardState currentKeyboardState;
        Input::GamePadState currentGamePadState;
    };
}
