// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CustomModelAnimationSample.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "ModelAnimationClip.hpp"
#include "RigidAnimationPlayer.hpp"
#include "RootAnimationPlayer.hpp"
#include "SkinnedAnimationPlayer.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace CustomAvatarAnimationSample
{
    using CustomModelAnimation::ModelAnimationClip;
    using CustomModelAnimation::RigidAnimationPlayer;
    using CustomModelAnimation::RootAnimationPlayer;
    using CustomModelAnimation::SkinnedAnimationPlayer;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GraphicsDeviceManager;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Graphics::Model;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteFont;
    using Microsoft::Xna::Framework::Input::Buttons;
    using Microsoft::Xna::Framework::Input::GamePadState;
    using Microsoft::Xna::Framework::Input::KeyboardState;
    using Microsoft::Xna::Framework::Input::Keys;

    /**
     * @brief Shows both rigid, per-part animation and true skinned animation, played from clips
     *        the sample's own content processors baked into each model's `Tag`.
     */
    class CustomAvatarAnimationSampleGame : public Microsoft::Xna::Framework::Game
    {
    private:
        GraphicsDeviceManager graphics;

        // Rigid model, animation players, clips.
        // XNA's Model is a reference type, so the originals are null until LoadContent assigns
        // them; std::optional is this port's established stand-in.
        std::optional<Model> rigidModel;
        Matrix rigidWorld;
        bool playingRigid = false;
        std::unique_ptr<RootAnimationPlayer> rigidRootPlayer;
        std::shared_ptr<ModelAnimationClip> rigidRootClip;
        std::unique_ptr<RigidAnimationPlayer> rigidPlayer;
        std::shared_ptr<ModelAnimationClip> rigidClip;

        // Skinned model, animation players, clips
        std::optional<Model> skinnedModel;
        Matrix skinnedWorld;
        bool playingSkinned = false;
        std::unique_ptr<RootAnimationPlayer> skinnedRootPlayer;
        std::shared_ptr<ModelAnimationClip> skinnedRootClip;
        std::unique_ptr<SkinnedAnimationPlayer> skinnedPlayer;
        std::shared_ptr<ModelAnimationClip> skinnedClip;

        // View and Projection matrices used for rendering
        Matrix view;
        Matrix projection;

        std::unique_ptr<SpriteBatch> spriteBatch;
        std::optional<SpriteFont> font;

        // Store the current and last gamepad state
        GamePadState currentGamePadState;
        GamePadState lastGamePadState;

        // Store the current and last keyboard state
        KeyboardState currentKeyboardState;
        KeyboardState lastKeyboardState;

    public:
        /** @brief Creates a new CustomAvatarAnimationSampleGame object. */
        CustomAvatarAnimationSampleGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "CustomAvatarAnimationSample.CustomAvatarAnimationSampleGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads all graphical content. */
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
        /** @brief Callback function when a skinned animation player is completed. */
        void skinnedPlayer_Completed();

        /** @brief Callback function when a rigid animation player is completed. */
        void rigidPlayer_Completed();

        /**
         * @brief Helper method to tell if a button was just pressed.
         * @param button The button to test.
         * @return True on the frame the button goes down.
         */
        [[nodiscard]] bool IsNewButtonPress(Buttons button) const;

        /**
         * @brief Helper method to tell if a key was just pressed.
         * @param key The key to test.
         * @return True on the frame the key goes down.
         */
        [[nodiscard]] bool IsNewKeyPress(Keys key) const;

        /** @brief Draws the two lines of HUD text. */
        void DrawHUD();

        /**
         * @brief Draws the skinned model with the skinning transforms the player produced.
         *
         * @param model The model to draw.
         * @param skinnedAnimationPlayer The player holding the skin transforms, or null.
         * @param rootAnimationPlayer The player holding the root motion, or null.
         */
        void DrawSkinnedModel(Model& model, SkinnedAnimationPlayer* skinnedAnimationPlayer,
                              RootAnimationPlayer* rootAnimationPlayer);

        /**
         * @brief Draws the rigid model, one bone transform per mesh.
         *
         * @param model The model to draw.
         * @param rigidAnimationPlayer The player holding the per-part transforms, or null.
         * @param rootAnimationPlayer The player holding the root motion, or null.
         */
        void DrawRigidModel(Model& model, RigidAnimationPlayer* rigidAnimationPlayer,
                            RootAnimationPlayer* rootAnimationPlayer);
    };
}
