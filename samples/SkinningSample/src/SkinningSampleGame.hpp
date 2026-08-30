// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SkinningSample.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "SkinnedModel/AnimationPlayer.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"

namespace SkinningSample
{
    /** @brief Sample game showing how to display skinned character animation. */
    class SkinningSampleGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game and configures its content root. */
        SkinningSampleGame();

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `SkinningSample.SkinningSampleGame`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads the processed skinned model and starts its `Take 001` clip. */
        void LoadContent() override;

        /**
         * @brief Handles input, updates the camera and advances animation.
         * @param gameTime Timing information for the current frame.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Draws the animated model using its SkinnedEffect instances.
         * @param gameTime Timing information for the current frame.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        Microsoft::Xna::Framework::GraphicsDeviceManager graphics;

        Microsoft::Xna::Framework::Input::KeyboardState currentKeyboardState;
        Microsoft::Xna::Framework::Input::GamePadState currentGamePadState;

        std::optional<Microsoft::Xna::Framework::Graphics::Model> currentModel;
        std::unique_ptr<SkinnedModel::AnimationPlayer> animationPlayer;

        float cameraArc = 0.0f;
        float cameraRotation = 0.0f;
        float cameraDistance = 100.0f;

        void HandleInput();
        void UpdateCamera(const Microsoft::Xna::Framework::GameTime& gameTime);
    };
}
