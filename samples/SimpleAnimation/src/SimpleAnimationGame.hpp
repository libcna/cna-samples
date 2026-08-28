// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SimpleAnimation.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <string>

#include "Tank.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"

namespace SimpleAnimation
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::GraphicsDeviceManager;

    /**
     * @brief Sample showing how to apply simple animation to a rigid body tank model.
     */
    class SimpleAnimationGame : public Microsoft::Xna::Framework::Game
    {
    private:
        GraphicsDeviceManager graphics;

        Tank tank;

    public:
        /** @brief Creates the graphics device manager and the tank. */
        SimpleAnimationGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "SimpleAnimation.SimpleAnimationGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads the graphics content. */
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
        /** @brief Handles input for quitting the game. */
        void HandleInput();
    };
}
