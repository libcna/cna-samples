// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/Collections/Generic/List.hpp"
#include "System/Random.hpp"

namespace RectangleCollision
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;
    using System::Collections::Generic::List;

    /**
     * @brief This is the main type for your game.
     */
    class RectangleCollisionGame : public Microsoft::Xna::Framework::Game
    {
        GraphicsDeviceManager graphics;

        // The images we will draw
        Texture2D personTexture;
        Texture2D blockTexture;

        // The images will be drawn with this SpriteBatch
        std::unique_ptr<SpriteBatch> spriteBatch;

        // Person
        Vector2 personPosition;
        static constexpr int PersonMoveSpeed = 5;

        // Blocks
        List<Vector2> blockPositions;
        float BlockSpawnProbability = 0.01f;
        static constexpr int BlockFallSpeed = 2;

        System::Random random;

        // For when a collision is detected
        bool personHit = false;

        // The sub-rectangle of the drawable area which should be visible on all TVs
        Rectangle safeBounds;
        // Percentage of the screen on every side is the safe area
        static constexpr float SafeAreaPortion = 0.05f;

    public:
        /**
         * @brief Constructs the game and selects its content root directory.
         */
        RectangleCollisionGame()
            : graphics(this)
        {
            getContentProperty().setRootDirectoryProperty("Content");
        }

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "RectangleCollision.RectangleCollisionGame".
         */
        [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name = "RectangleCollision.RectangleCollisionGame";
            return name;
        }

    protected:
        /**
         * @brief Allows the game to perform any initialization it needs to before starting to
         *        run. This is where it can query for any required services and load any
         *        non-graphic related content. Calling base.Initialize will enumerate through
         *        any components and initialize them as well.
         */
        void Initialize() override
        {
            Game::Initialize();

            // Calculate safe bounds based on current resolution
            Viewport viewport = graphics.getGraphicsDeviceProperty()->getViewportProperty();
            // C# allows a floating-point expression to be evaluated with more precision than its
            // type, and the 32-bit reference build of this sample does exactly that: it keeps
            // `viewport.Width * (1 - 2 * SafeAreaPortion)` in an extended-precision register and
            // truncates that value, so the original's safe area is 719x431 and not the 720x432 a
            // float-rounded product yields. The widening casts below reproduce the reference
            // build's own arithmetic, and with it the original person rest position and the exact
            // horizontal clamp range the player can reach.
            safeBounds = Rectangle(
                (int)((double)viewport.getWidthProperty() * SafeAreaPortion),
                (int)((double)viewport.getHeightProperty() * SafeAreaPortion),
                (int)((double)viewport.getWidthProperty() * (1 - 2 * SafeAreaPortion)),
                (int)((double)viewport.getHeightProperty() * (1 - 2 * SafeAreaPortion)));

            // Start the player in the center along the bottom of the screen
            personPosition.X = (float)((safeBounds.Width - personTexture.getWidthProperty()) / 2);
            personPosition.Y = (float)(safeBounds.Height - personTexture.getHeightProperty());
        }

        /**
         * @brief Load your graphics content.
         */
        void LoadContent() override
        {
            // Load textures
            blockTexture = getContentProperty().Load<Texture2D>("Block");
            personTexture = getContentProperty().Load<Texture2D>("Person");

            // Create a sprite batch to draw those textures
            spriteBatch = std::make_unique<SpriteBatch>(*graphics.getGraphicsDeviceProperty());
        }

        /**
         * @brief Allows the game to run logic such as updating the world,
         *        checking for collisions, gathering input and playing audio.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override
        {
            // Get input
            KeyboardState keyboard = Keyboard::GetState();
            GamePadState gamePad = GamePad::GetState(PlayerIndex::One);

            // Allows the game to exit
            if (gamePad.getButtonsProperty().getBackProperty() == ButtonState::Pressed ||
                keyboard.IsKeyDown(Keys::Escape))
            {
                this->Exit();
            }

            // Move the player left and right with arrow keys or d-pad
            if (keyboard.IsKeyDown(Keys::Left) ||
                gamePad.getDPadProperty().getLeftProperty() == ButtonState::Pressed)
            {
                personPosition.X -= PersonMoveSpeed;
            }
            if (keyboard.IsKeyDown(Keys::Right) ||
                gamePad.getDPadProperty().getRightProperty() == ButtonState::Pressed)
            {
                personPosition.X += PersonMoveSpeed;
            }

            // Prevent the person from moving off of the screen
            personPosition.X = MathHelper::Clamp(personPosition.X,
                (float)safeBounds.getLeftProperty(),
                (float)(safeBounds.getRightProperty() - personTexture.getWidthProperty()));

            // Spawn new falling blocks
            if (random.NextDouble() < BlockSpawnProbability)
            {
                float x = (float)random.NextDouble() *
                    (getWindowProperty().getClientBoundsProperty().Width - blockTexture.getWidthProperty());
                blockPositions.Add(Vector2(x, (float)(-blockTexture.getHeightProperty())));
            }

            // Get the bounding rectangle of the person
            Rectangle personRectangle =
                Rectangle((int)personPosition.X, (int)personPosition.Y,
                personTexture.getWidthProperty(), personTexture.getHeightProperty());

            // Update each block
            personHit = false;
            for (int i = 0; i < blockPositions.getCountProperty(); i++)
            {
                // Animate this block falling
                blockPositions.setItem(i,
                    Vector2(blockPositions.getItem(i).X,
                            blockPositions.getItem(i).Y + BlockFallSpeed));

                // Get the bounding rectangle of this block
                Rectangle blockRectangle =
                    Rectangle((int)blockPositions.getItem(i).X, (int)blockPositions.getItem(i).Y,
                    blockTexture.getWidthProperty(), blockTexture.getHeightProperty());

                // Check collision with person
                if (personRectangle.Intersects(blockRectangle))
                    personHit = true;

                // Remove this block if it have fallen off the screen
                if (blockPositions.getItem(i).Y > getWindowProperty().getClientBoundsProperty().Height)
                {
                    blockPositions.RemoveAt(i);

                    // When removing a block, the next block will have the same index
                    // as the current block. Decrement i to prevent skipping a block.
                    i--;
                }
            }

            Game::Update(gameTime);
        }

        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override
        {
            GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

            // Change the background to red when the person was hit by a block
            if (personHit)
            {
                device.Clear(Color::Red);
            }
            else
            {
                device.Clear(Color::CornflowerBlue);
            }


            spriteBatch->Begin();

            // Draw person
            spriteBatch->Draw(personTexture, personPosition, Color::White);

            // Draw blocks
            for (const Vector2& blockPosition : blockPositions)
                spriteBatch->Draw(blockTexture, blockPosition, Color::White);

            spriteBatch->End();


            Game::Draw(gameTime);
        }
    };
}
