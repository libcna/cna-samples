// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Billboard.cs
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
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"

namespace Billboard
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief Sample showing how to efficiently render billboard sprites.
     */
    class BillboardGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game. */
        BillboardGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "Billboard.BillboardGame".
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
        /** Handles input for quitting the game. */
        void HandleInput();

        /** Handles camera input. */
        void UpdateCamera(const GameTime& gameTime);

        GraphicsDeviceManager graphics;

        Input::KeyboardState currentKeyboardState;
        Input::GamePadState currentGamePadState;

        Vector3 cameraPosition{0, 50, 50};
        Vector3 cameraFront{0, 0, -1};

        // XNA's Model is a reference type, so the original's field is null until LoadContent runs.
        std::optional<Model> landscape;
    };
}
