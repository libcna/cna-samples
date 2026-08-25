// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// FuzzyLogicGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Entities/Mouse.hpp"
#include "Entities/Tank.hpp"

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/Collections/Generic/List.hpp"

namespace FuzzyLogic
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;
    using System::Collections::Generic::List;

    /**
     * @brief This sample shows how an AI can use fuzzy logic to make decisions.
     *
     * It also demonstrates a method for organizing different AI behaviors, similar to
     * a state machine.
     */
    class FuzzyLogicGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game and sets the back buffer size. */
        FuzzyLogicGame();

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "FuzzyLogic.FuzzyLogicGame".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Works out the level boundary and creates the tank. */
        void Initialize() override;

        /**
         * @brief LoadContent will be called once per game and is the place to load
         *        all of your content.
         */
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
        /**
         * DrawBar is a helper function used by Draw. It is used to draw the three
         * bars which display the tank's fuzzy weights.
         */
        void DrawBar(Rectangle bar, float barWidthNormalized, const String& label,
                     const GameTime& gameTime, bool highlighted);

        bool IsPressed(Keys key) const;
        bool IsPressed(Buttons button) const;

        /** Handles input for quitting the game. */
        void HandleInput();

        // This value controls the number of mice that will be in the game. Try
        // increasing this value! Lots of mice can be fun to watch.
        static constexpr int NumberOfMice = 15;

        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;
        // XNA's SpriteFont is a reference type, so the original's field is null until
        // LoadContent runs.
        std::optional<SpriteFont> font;

        KeyboardState currentKeyboardState;
        GamePadState currentGamePadState;
        KeyboardState lastKeyboardState;
        GamePadState lastGamePadState;

        // The game will keep track of a tank and some mice, which are represented
        // by these two variables.
        std::unique_ptr<Tank> tank;
        List<std::shared_ptr<Mouse>> mice;

        // This texture is a 1x1 white dot, just like the name suggests. by stretching
        // it, we can use it to draw the bar graph that will show the tank's fuzzy
        // weights.
        std::optional<Texture2D> onePixelWhite;

        // Tells us which of the three fuzzy weights the user is currently modifying.
        // the currently selected weight will have a pulsing red tint.
        int currentlySelectedWeight = 0;

        // Definte the dimensions of the fuzzy logic bars
        Rectangle barDistance{105, 45, 85, 40};
        Rectangle barAngle{105, 125, 85, 40};
        Rectangle barTime{105, 205, 85, 40};

        Rectangle levelBoundary;

        Vector2 lastTouchPoint;
        bool isDragging = false;
    };
}
