// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"

namespace ObjImporterSample
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GraphicsDeviceManager;
    using Microsoft::Xna::Framework::Graphics::Model;

    /**
     * @brief Displays the imported model and animates it rotating.
     */
    class ObjImporterGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Creates the graphics device manager and configures the content root. */
        ObjImporterGame();

        /**
         * @brief Returns the fully qualified logical type name.
         *
         * @return The XNA type name of this game.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads the imported tank model. */
        void LoadContent() override;

        /**
         * @brief Runs the base game logic and checks for exit input.
         *
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief Draws the lit tank model with time-based rotation.
         *
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        GraphicsDeviceManager graphics;
        std::optional<Model> model;
    };
}
