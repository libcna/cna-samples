// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PathDrawingGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "PrimitiveBatch.hpp"
#include "Tank.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocationState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/TimeSpan.hpp"

namespace PathDrawing
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;
    using namespace Microsoft::Xna::Framework::Input::Touch;

    /**
     * @brief This is the main type for your game.
     */
    class PathDrawingGame : public Microsoft::Xna::Framework::Game
    {
    private:
        GraphicsDeviceManager graphics;

        // We need a SpriteBatch to do our main drawing
        std::unique_ptr<SpriteBatch> spriteBatch;

        // We also use the PrimitiveBatch to draw the lines for our path.
        std::unique_ptr<PrimitiveBatch> primitiveBatch;

        // A font for drawing our instruction text
        // XNA's SpriteFont is a reference type, so the original's field is null until
        // LoadContent assigns it; std::optional is this port's established stand-in.
        std::optional<SpriteFont> font;

        // A texture we draw for the ground
        Texture2D groundTexture;

        // Sets the number of pixels the ground occupies before repeating. Increase to "zoom in" on
        // the ground or decrease to "zoom out".
        static constexpr int groundSize = 300;

        // The tank that will drive around, following our path.
        std::unique_ptr<Tank> tank;

        // Whether or not the user is drawing the path for the Tank to follow
        bool drawingWaypoints = false;

    public:
        /**
         * @brief Constructs the game, selects the phone frame rate and enables the drag gesture.
         */
        PathDrawingGame()
            : graphics(this)
        {
            getContentProperty().setRootDirectoryProperty("Content");

            // Frame rate is 30 fps by default for Windows Phone.
            setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
            graphics.setIsFullScreenProperty(true);

            // We only care about the FreeDrag gesture for this sample
            TouchPanel::setEnabledGesturesProperty(GestureType::FreeDrag);
        }

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "PathDrawing.PathDrawingGame".
         */
        [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name = "PathDrawing.PathDrawingGame";
            return name;
        }

    protected:
        /**
         * @brief LoadContent will be called once per game and is the place to load
         *        all of your content.
         */
        void LoadContent() override
        {
            // Create a new SpriteBatch, which can be used to draw textures.
            spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());

            // Create the PrimitiveBatch for drawing our path
            primitiveBatch = std::make_unique<PrimitiveBatch>(getGraphicsDeviceProperty());

            // Load our font and ground
            font = getContentProperty().Load<SpriteFont>("Font");
            groundTexture = getContentProperty().Load<Texture2D>("ground");

            // Create the tank
            tank = std::make_unique<Tank>(getGraphicsDeviceProperty(), getContentProperty());
            tank->Reset(Vector2(100));
            tank->setMoveSpeedProperty(225.0f);
        }

        /**
         * @brief Allows the game to run logic such as updating the world,
         *        checking for collisions, gathering input, and playing audio.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override
        {
            // Allows the game to exit
            if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getBackProperty() ==
                ButtonState::Pressed)
                this->Exit();

            // Update the tank
            tank->Update(gameTime);

            // Get the current state of touch input
            TouchCollection touches = TouchPanel::GetState();

            // If we have at least one touch...
            if (touches.getCountProperty() > 0)
            {
                // If the primary touch is pressed and in the tank, we start drawing our path
                if (touches[0].getStateProperty() == TouchLocationState::Pressed &&
                    tank->HitTest(touches[0].getPositionProperty()))
                {
                    // Clear the waypoints to start a new path
                    tank->getWaypointsProperty().Clear();

                    // We're now drawing waypoints
                    drawingWaypoints = true;

                    // Use the touch location as the first waypoint
                    tank->getWaypointsProperty().Enqueue(touches[0].getPositionProperty());
                }

                // Otherwise if the primary touch is released, we stop drawing our path
                else if (touches[0].getStateProperty() == TouchLocationState::Released)
                {
                    drawingWaypoints = false;
                }
            }

            // Read all of the gestures
            while (TouchPanel::getIsGestureAvailableProperty())
            {
                GestureSample gesture = TouchPanel::ReadGesture();

                // If we have a FreeDrag gesture...
                if (gesture.getGestureTypeProperty() == GestureType::FreeDrag)
                {
                    // If we're drawing waypoints and the drag gesture has moved from the last
                    // location, enqueue the position of the gesture as the next waypoint.
                    if (drawingWaypoints && gesture.getDeltaProperty() != Vector2::Zero)
                    {
                        tank->getWaypointsProperty().Enqueue(gesture.getPositionProperty());
                    }
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
            getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

            // First draw our ground
            DrawGround();

            // Next draw the path
            DrawPath();

            // Draw our instruction text and tank
            spriteBatch->Begin();
            spriteBatch->DrawString(*font, "Drag a path from the tank to have him drive around.",
                                    Vector2(5), Color::White);
            tank->Draw(*spriteBatch);
            spriteBatch->End();

            Game::Draw(gameTime);
        }

    private:
        /**
         * @brief Helper method to draw our ground texture.
         */
        void DrawGround()
        {
            // Draw the ground using a LinearWrap state so we can repeat the texture across the screen
            SamplerState linearWrap = SamplerState::LinearWrap;
            spriteBatch->Begin(SpriteSortMode::Deferred, nullptr, &linearWrap, nullptr, nullptr);

            // Compute the source rectangle based on our viewport size and ground scale
            Rectangle source = Rectangle();
            source.Width = (getGraphicsDeviceProperty().getViewportProperty().getWidthProperty() /
                            groundSize) * groundTexture.getWidthProperty();
            source.Height = (getGraphicsDeviceProperty().getViewportProperty().getHeightProperty() /
                             groundSize) * groundTexture.getHeightProperty();

            // Draw the ground using our source rectangle which will cause it to wrap across the screen
            spriteBatch->Draw(groundTexture,
                              getGraphicsDeviceProperty().getViewportProperty().getBoundsProperty(),
                              source, Color::White);
            spriteBatch->End();
        }

        /**
         * @brief Helper method to draw the path using PrimitiveBatch.
         */
        void DrawPath()
        {
            // Start drawing lines
            primitiveBatch->Begin(PrimitiveType::LineList);

            // Add the tank's position as the first vertex
            primitiveBatch->AddVertex(tank->getLocationProperty(), Color::White);

            for (int i = 1; i < tank->getWaypointsProperty().getCountProperty(); i++)
            {
                // Add the next waypoint location to our line list
                primitiveBatch->AddVertex(tank->getWaypointsProperty().getItem(i), Color::White);

                // If we're not at the end of our waypoint list, add this point again to act as the
                // first point for the next line.
                if (i < tank->getWaypointsProperty().getCountProperty() - 1)
                    primitiveBatch->AddVertex(tank->getWaypointsProperty().getItem(i), Color::White);
            }

            primitiveBatch->End();
        }
    };
}
