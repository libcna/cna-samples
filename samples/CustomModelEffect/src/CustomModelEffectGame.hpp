// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CustomModelEffect.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"

namespace CustomModelEffect
{
    /** @brief Sample showing how to render a model using a custom effect. */
    class CustomModelEffectGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game and configures its content root. */
        CustomModelEffectGame();

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `CustomModelEffect.CustomModelEffectGame`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads the environment-mapped saucer model. */
        void LoadContent() override;

        /**
         * @brief Handles input and updates the game.
         * @param gameTime Timing information for the current frame.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Calculates the camera and draws the rotating model.
         * @param gameTime Timing information for the current frame.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        Microsoft::Xna::Framework::GraphicsDeviceManager graphics;
        std::optional<Microsoft::Xna::Framework::Graphics::Model> model;

        void HandleInput();
    };
}
