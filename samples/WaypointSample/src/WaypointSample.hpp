// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// WaypointSample.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Tank.hpp"

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"

namespace Waypoint
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    /**
     * @brief This is the main type for your game.
     */
    class WaypointSample : public Microsoft::Xna::Framework::Game
    {
#if defined(WINDOWS_PHONE)
        static constexpr int screenWidth = 800;
        static constexpr int screenHeight = 480;
#else
        static constexpr int screenWidth = 853;
        static constexpr int screenHeight = 480;
#endif
        static constexpr float cursorMoveSpeed = 250.0f;

        // the text we display on screen, created here to make our Draw method cleaner
        static const std::string helpText;

        // Graphics data
        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;

        // Cursor data
        Texture2D cursorTexture;
        Vector2 cursorCenter;
        Vector2 cursorLocation;

        // HUD data
        // XNA's SpriteFont is a reference type, so the original's field is null until
        // LoadContent assigns it; std::optional is this port's established stand-in.
        std::optional<SpriteFont> hudFont;
        // Where the HUD draws on the screen
        Vector2 hudLocation;

        // Input data
        KeyboardState previousKeyboardState;
        GamePadState previousGamePadState;
        KeyboardState currentKeyboardState;
        GamePadState currentGamePadState;

        // The waypoint-following tank
        std::unique_ptr<Tank> tank;

#if defined(WINDOWS_PHONE)
        // Menu Bar data
        Texture2D blankTexture;
        int menuBarButton1_Left = 150;
        int menuBarButton2_Left = 500;
        int menuBarButtonTop = 5;
        int menuBarButtonWidth = 150;
        int menuBarButtonHeight = 30;
        static constexpr int menuBar_Height = 40;
        bool isClearRequested = false;
        bool isBehaviorChangeRequested = false;
        RenderTarget2D renderTarget;
#endif

    public:
        /** @brief Constructs the game and adds the tank as a game component. */
        WaypointSample();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "Waypoint.WaypointSample".
         */
        [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Places the HUD, the cursor and the tank. */
        void Initialize() override;

        /** @brief Loads the cursor texture and the HUD font. */
        void LoadContent() override;

        /**
         * @brief Allows the game to run logic such as updating the world.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    private:
#if defined(WINDOWS_PHONE)
        void DrawMenuBar();
#endif
        void HandleInput(float elapsedTime);
    };
}
