// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game1.cs
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
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"

namespace SampleGame
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /** @brief Main game type for the content-manifest sample. */
    class Game1 : public Game
    {
        GraphicsDeviceManager graphics;
        std::optional<SpriteBatch> spriteBatch;
        std::optional<SpriteFont> font;

        // We're building a string of the files to display on screen.
        std::string displayedText;

    public:
        /** @brief Constructs the game and applies its original timing and content settings. */
        Game1();

        /**
         * @brief Returns the fully qualified logical runtime type name.
         * @return `SampleGame.Game1`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Performs the sample's original initialization pass. */
        void Initialize() override;

        /** @brief Loads the font and generated content manifest. */
        void LoadContent() override;

        /** @brief Releases non-content-manager resources; this sample owns none. */
        void UnloadContent() override;

        /**
         * @brief Handles exit input and updates the game.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief Draws the generated manifest categories.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;
    };
}
