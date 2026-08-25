// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// TransformedCollisionTestGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "TransformedSprite.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"

namespace TransformedCollisionTest
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    /**
     * @brief This is the main type for your game.
     */
    class TransformedCollisionTestGame : public Microsoft::Xna::Framework::Game
    {
        GraphicsDeviceManager graphics;

        std::unique_ptr<SpriteBatch> spriteBatch;

        // Sprites
        TransformedSprite spriteF;
        TransformedSprite spriteR;

        // Helper for drawing points
        Texture2D pointTexture;
        Vector2 pointOrigin;

        // Screen origin of the collision space visualization
        Vector2 collisionSpaceOrigin;

        // Remember collisions to control background color
        bool collision = false;

#if !defined(XBOX360) // Mouse is not supported on the 360
        // Retain the previous scroll wheel value in order to calculate a delta
        int lastScrollWheelValue = 0;
#endif

    public:
        /**
         * @brief Constructs the game, shows the mouse and allows the window to be resized.
         */
        TransformedCollisionTestGame()
            : graphics(this)
        {
            getContentProperty().setRootDirectoryProperty("Content");

            setIsMouseVisibleProperty(true);
            getWindowProperty().setAllowUserResizingProperty(true);
        }

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "TransformedCollisionTest.TransformedCollisionTestGame".
         */
        [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name =
                "TransformedCollisionTest.TransformedCollisionTestGame";
            return name;
        }

    protected:
        /**
         * @brief Initializes the game and separates the two sprites.
         */
        void Initialize() override
        {
            Game::Initialize();

            // Start with the sprites not overlapping
            spriteR.Position.X = (float)spriteF.getTextureProperty().getWidthProperty();
        }

        /**
         * @brief Load your graphics content.
         */
        void LoadContent() override
        {
            pointTexture = getContentProperty().Load<Texture2D>("Point");
            // The point should be drawn centered
            pointOrigin =
                Vector2((float)(pointTexture.getWidthProperty() / 2),
                        (float)(pointTexture.getHeightProperty() / 2));

            spriteF.setTextureProperty(getContentProperty().Load<Texture2D>("F"));
            spriteR.setTextureProperty(getContentProperty().Load<Texture2D>("R"));

            spriteBatch = std::make_unique<SpriteBatch>(*graphics.getGraphicsDeviceProperty());

            // The gray collision space visualization should be drawn centered
            // vertically and slighty away from the left edge of the window.
            Viewport viewport = graphics.getGraphicsDeviceProperty()->getViewportProperty();
            collisionSpaceOrigin =
                Vector2(viewport.getWidthProperty() * .15f,
                        (float)(viewport.getHeightProperty() / 2));
        }

        /**
         * @brief Allows the game to run logic such as updating the world,
         *        checking for collisions, gathering input and playing audio.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override
        {
            HandleInput();

            // Rebuild the sprite world transforms
            spriteF.UpdateTransform();
            spriteR.UpdateTransform();

            // Invoke the collision test
            collision = spriteF.IntersectPixels(spriteR);


            Game::Update(gameTime);
        }

    private:
        void HandleInput()
        {
            GamePadState gamePadState = GamePad::GetState(PlayerIndex::One);

#if !defined(XBOX360) // Mouse is not supported on the 360, so don't handle mouse/keyboard

            MouseState mouseState = Mouse::GetState();

            // We don't want mouse movements and such when not active
            if (getIsActiveProperty())
            {
                KeyboardState keyboardState = Keyboard::GetState();

                // Allow exiting with keyboard
                if (keyboardState.IsKeyDown(Keys::Escape))
                {
                    this->Exit();
                }
                // Left mouse button moves the F object
                else if (mouseState.getLeftButtonProperty() == ButtonState::Pressed)
                {
                    HandleInput(spriteF, keyboardState, mouseState);
                }
                // Right mouse button moves the R object
                else if (mouseState.getRightButtonProperty() == ButtonState::Pressed)
                {
                    HandleInput(spriteR, keyboardState, mouseState);
                }
            }

            lastScrollWheelValue = mouseState.getScrollWheelValueProperty();

#endif
            // Allow exiting with game pad
            if (gamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
                Exit();

            // Left trigger/shoulder controls sprite F
            HandleInput(gamePadState.getTriggersProperty().getLeftProperty(),
                gamePadState.getButtonsProperty().getLeftShoulderProperty(),
                spriteF, gamePadState);
            // Right trigger/shoulder controls sprite R
            HandleInput(gamePadState.getTriggersProperty().getRightProperty(),
                gamePadState.getButtonsProperty().getRightShoulderProperty(),
                spriteR, gamePadState);
        }

#if !defined(XBOX360)
        void HandleInput(TransformedSprite& sprite,
            const KeyboardState& keyboardState, const MouseState& mouseState)
        {
            const float rotateAndScaleSpeed = 0.02f;
            Vector2 mousePosition((float)mouseState.getXProperty(),
                                  (float)mouseState.getYProperty());
            float rotationDelta =
                (mouseState.getScrollWheelValueProperty() - lastScrollWheelValue) * 0.005f;

            // Click and drag to move sprite
            // Hold left control when dragging to move sprite's origin
            if (keyboardState.IsKeyDown(Keys::LeftControl))
                sprite.Origin = sprite.Position - mousePosition;
            else
                sprite.Position = mousePosition;

            // Hold the left mouse button and spin the mouse wheel to rotate
            // Do it while holding alt to scale
            if (keyboardState.IsKeyDown(Keys::LeftAlt))
                sprite.Scale += rotationDelta;
            else
                sprite.Rotation += rotationDelta;

            // Also allow the left and right arrow keys to rotate
            if (keyboardState.IsKeyDown(Keys::Left))
                sprite.Rotation -= rotateAndScaleSpeed;
            else if (keyboardState.IsKeyDown(Keys::Right))
                sprite.Rotation += rotateAndScaleSpeed;
            // And the up and down arrow keys to scale
            if (keyboardState.IsKeyDown(Keys::Up))
                sprite.Scale += rotateAndScaleSpeed;
            else if (keyboardState.IsKeyDown(Keys::Down))
                sprite.Scale -= rotateAndScaleSpeed;
        }
#endif

        static void HandleInput(float trigger, ButtonState shoulder,
            TransformedSprite& spriteF, const GamePadState& gamePadState)
        {
            const float rotateAndScaleSpeed = 0.04f;
            Vector2 gamePadMovement =
                gamePadState.getThumbSticksProperty().getLeftProperty() * 3.0f;
            gamePadMovement.Y = -gamePadMovement.Y;

            // Hold the trigger to transform
            if (trigger > 0)
            {
                // Left stick moves
                spriteF.Position += gamePadMovement;
                // Right stick rotates and scales
                spriteF.Rotation +=
                    gamePadState.getThumbSticksProperty().getRightProperty().X * rotateAndScaleSpeed;
                spriteF.Scale +=
                    gamePadState.getThumbSticksProperty().getRightProperty().Y * rotateAndScaleSpeed;
            }
            // Hold the shoulder to move the pivot point
            else if (shoulder == ButtonState::Pressed)
            {
                // with the left thumbstick
                spriteF.Origin += gamePadMovement;
            }
        }

    protected:
        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override
        {
            // Make the background red when the objects are colliding
            if (collision)
                graphics.getGraphicsDeviceProperty()->Clear(Color::Red);
            else
                graphics.getGraphicsDeviceProperty()->Clear(Color::CornflowerBlue);

            spriteBatch->Begin();


            // Calculate R's transform parameters in F's local space
            Matrix transformRtoF = spriteR.getTransformProperty() *
                Matrix::Invert(spriteF.getTransformProperty());
            Vector2 position(transformRtoF.getTranslationProperty().X,
                             transformRtoF.getTranslationProperty().Y);
            float rotation = spriteR.Rotation - spriteF.Rotation;
            float scale = spriteR.Scale / spriteF.Scale;

            // Draw light gray F and R in F's local space
            spriteBatch->Draw(spriteF.getTextureProperty(), collisionSpaceOrigin, std::nullopt,
                Color::Gray, 0, Vector2::Zero, 1, SpriteEffects::None, 0.2f);
            spriteBatch->Draw(spriteR.getTextureProperty(), collisionSpaceOrigin + position,
                std::nullopt, Color::Gray, rotation, Vector2::Zero, scale, SpriteEffects::None, 0.2f);

            // Draw origin's for F and R in F's local space
            DrawPoint(collisionSpaceOrigin, Color::Green);
            DrawPoint(collisionSpaceOrigin + position, Color::Brown);


            // Draw F and R in world space
            spriteF.Draw(*spriteBatch);
            spriteR.Draw(*spriteBatch);

            // Draw origins for F and R in world space
            DrawPoint(spriteF.Position, Color::Yellow);
            DrawPoint(spriteR.Position, Color::Yellow);


            spriteBatch->End();

            Game::Draw(gameTime);
        }

    private:
        void DrawPoint(Vector2 position, Color color)
        {
            spriteBatch->Draw(pointTexture, position, std::nullopt, color, 0, pointOrigin, 1,
                SpriteEffects::None, 0.1f);
        }
    };
}
