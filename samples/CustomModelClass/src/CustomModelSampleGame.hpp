// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CustomModelSampleGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

namespace CustomModelSample
{
    class CustomModel;

    /** @brief Sample showing how to replace the built-in XNA Framework Model type. */
    class CustomModelSampleGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game and configures its content root. */
        CustomModelSampleGame();

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `CustomModelSample.CustomModelSampleGame`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads the custom model and calculates the camera matrices. */
        void LoadContent() override;

        /**
         * @brief Handles input and updates the rotating world transform.
         * @param gameTime Timing information for the current frame.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Clears the frame and draws the custom model.
         * @param gameTime Timing information for the current frame.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        Microsoft::Xna::Framework::GraphicsDeviceManager graphics;
        std::shared_ptr<CustomModel> model;
        Microsoft::Xna::Framework::Matrix world;
        Microsoft::Xna::Framework::Matrix view;
        Microsoft::Xna::Framework::Matrix projection;

        void HandleInput();
    };
}
