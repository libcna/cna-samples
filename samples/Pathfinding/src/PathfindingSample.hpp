// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PathfindingGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "ContentReaders.hpp"
#include "Map.hpp"
#include "PathFinder.hpp"
#include "Tank.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"

namespace Pathfinding
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;
    using namespace Microsoft::Xna::Framework::Input::Touch;

    /**
     * @brief This is the main type for your game.
     */
    class Pathfinding : public Microsoft::Xna::Framework::Game
    {
        static constexpr int bottomUIHeight = 80;
        static constexpr int sliderButtonWidth = 20;

        KeyboardState currentKeyboardState;
        GamePadState currentGamePadState;

        Texture2D ButtonA;
        Texture2D ButtonB;
        Texture2D ButtonX;
        Texture2D ButtonY;
        Texture2D onePixelWhite;

        Rectangle buttonStartStop = Rectangle(5, 405, 110, 30);
        Rectangle buttonReset = Rectangle(125, 405, 110, 30);
        Rectangle buttonNextMap = Rectangle(245, 405, 110, 30);
        Rectangle buttonPathfinding = Rectangle(365, 405, 270, 30);
        Rectangle barTimeStep = Rectangle(125, 450, 200, 20);

        Vector2 pathStatusPosition;
        Vector2 searchStepsPosition;

        Rectangle gameplayArea;

        // XNA's SpriteFont is a reference type, so the original's field is null until
        // LoadContent assigns it; std::optional is this port's established stand-in.
        std::optional<SpriteFont> HUDFont;

        GraphicsDeviceManager graphics;
        std::unique_ptr<SpriteBatch> spriteBatch;

        Map map;
        Tank tank;
        PathFinder pathFinder;

    public:
        /**
         * @brief Constructs the game and selects its content root directory.
         */
        Pathfinding()
            : graphics(this)
        {
            getContentProperty().setRootDirectoryProperty("Content");

#if defined(WINDOWS_PHONE)
            // Frame rate is 30 fps by default for Windows Phone.
            setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
            graphics.setIsFullScreenProperty(true);
#endif

            // The four map assets are XNB objects of a type this sample declares, so the
            // closed reader for it must exist before Content.Load reaches them.
            PathfindingData::ContentReaders::Register();
        }

        /**
         * @brief Returns the fully qualified logical type name of this game.
         * @return "Pathfinding.Pathfinding".
         */
        [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name = "Pathfinding.Pathfinding";
            return name;
        }

    protected:
        /**
         * @brief Allows the game to perform any initialization it needs to before starting to
         *        run.
         */
        void Initialize() override
        {
            Game::Initialize();

            gameplayArea = getGraphicsDeviceProperty().getViewportProperty().getTitleSafeAreaProperty();

            // The bottom part of the screen houses controls for the buttons
            gameplayArea.Height -= bottomUIHeight;

            pathStatusPosition = Vector2(645, 425);
            searchStepsPosition = Vector2(645, 445);

            TouchPanel::setEnabledGesturesProperty(GestureType::Tap);

            map.UpdateMapViewport(gameplayArea);
            tank.Initialize(map);
            pathFinder.Initialize(map);
        }

        /**
         * @brief LoadContent will be called once per game and is the place to load all of your
         *        content.
         */
        void LoadContent() override
        {
            ButtonA = getContentProperty().Load<Texture2D>("xboxControllerButtonA");
            ButtonB = getContentProperty().Load<Texture2D>("xboxControllerButtonB");
            ButtonX = getContentProperty().Load<Texture2D>("xboxControllerButtonX");
            ButtonY = getContentProperty().Load<Texture2D>("xboxControllerButtonY");

            HUDFont = getContentProperty().Load<SpriteFont>("HUDFont");

            onePixelWhite = Texture2D(getGraphicsDeviceProperty(), 1, 1);
            {
                Color pixel[] = { Color::White };
                onePixelWhite.SetData(pixel, 1);
            }

            // Create a new SpriteBatch, which can be used to draw textures.
            spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());

            map.LoadContent(getContentProperty());
            tank.LoadContent(getContentProperty());
            pathFinder.LoadContent(getContentProperty());

            Game::LoadContent();
        }

        /**
         * @brief UnloadContent will be called once per game and is the place to unload all
         *        content.
         */
        void UnloadContent() override
        {
        }

        /**
         * @brief Allows the game to run logic such as updating the world, checking for
         *        collisions, gathering input, and playing audio.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override
        {
            HandleInput();

            if (map.getMapReloadProperty())
            {
                map.ReloadMap();
                map.UpdateMapViewport(gameplayArea);
                tank.Reset();
                pathFinder.Reset();
            }

            if (pathFinder.getSearchStatusProperty() == SearchStatus::PathFound &&
                !tank.getMovingProperty())
            {
                for (const Point& point : pathFinder.FinalPath())
                {
                    tank.getWaypointsProperty().Enqueue(map.MapToWorld(point, true));
                }
                tank.setMovingProperty(true);
            }
            pathFinder.Update(gameTime);
            tank.Update(gameTime);
            Game::Update(gameTime);
        }

        /**
         * @brief This is called when the game should draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override
        {
            getGraphicsDeviceProperty().Clear(Color::Black);

            map.Draw(*spriteBatch);
            pathFinder.Draw(*spriteBatch);
            tank.Draw(*spriteBatch);

            DrawHUD(*spriteBatch);
            DrawPathStatus(*spriteBatch);

            Game::Draw(gameTime);
        }

    private:
        /**
         * Helper function used by Draw. It is used to draw the slider bars
         */
        void DrawBar(Rectangle bar, float barWidthNormalized, const std::string& label)
        {
            // Calculate how wide the bar should be, and then draw it.
            bar.Height /= 2;
            spriteBatch->Draw(onePixelWhite, bar, Color::White);

            // Draw the slider
            spriteBatch->Draw(onePixelWhite, Rectangle(bar.X + (int)(bar.Width * barWidthNormalized),
                              bar.Y - bar.Height / 2, sliderButtonWidth, bar.Height * 2), Color::Orange);

            // Finally, draw the label to the left of the bar.
            Vector2 labelSize = HUDFont->MeasureString(label);
            Vector2 labelPosition = Vector2(bar.X - 10 - labelSize.X, (float)(bar.Y - bar.Height / 2));
            spriteBatch->DrawString(*HUDFont, label, labelPosition, Color::White);
        }

        /**
         * Helper function used to draw the buttons
         */
        void DrawButton(Rectangle button, const std::string& label)
        {
            spriteBatch->Draw(onePixelWhite, button, Color::Orange);
            spriteBatch->DrawString(*HUDFont, label,
                Vector2((float)(button.getLeftProperty() + 10), (float)(button.getTopProperty() + 5)),
                Color::Black);
        }

        /**
         * Helper function used to draw the HUD
         */
        void DrawHUD(SpriteBatch& spriteBatch)
        {
            spriteBatch.Begin();

            DrawBar(barTimeStep, pathFinder.getTimeStepProperty(), "Time Step:");

#if defined(WINDOWS_PHONE)
            DrawButton(buttonStartStop, "Start/Stop");
            DrawButton(buttonReset, "Reset");
            DrawButton(buttonPathfinding, "Pathfinding mode: " +
                SearchMethodToString(pathFinder.getSearchMethodProperty()));
            DrawButton(buttonNextMap, "Next Map");
#else
            float textureWidth = (float)ButtonA.getWidthProperty();

            spriteBatch.Draw(ButtonA, Vector2(10, 400), Color::White);
            spriteBatch.DrawString(
                *HUDFont, " Start/Stop",
                Vector2(10 + textureWidth, 400), Color::White);

            spriteBatch.Draw(ButtonB, Vector2(150, 400), Color::White);
            spriteBatch.DrawString(
                *HUDFont, " Reset",
                Vector2(150 + textureWidth, 400), Color::White);

            spriteBatch.Draw(ButtonY, Vector2(250, 400), Color::White);
            spriteBatch.DrawString(
                *HUDFont, " Next map",
                Vector2(250 + textureWidth, 400), Color::White);

            spriteBatch.Draw(ButtonX, Vector2(400, 400), Color::White);
            spriteBatch.DrawString(
                *HUDFont, " Pathfinding mode: " +
                SearchMethodToString(pathFinder.getSearchMethodProperty()),
                Vector2(400 + textureWidth, 400), Color::White);

#endif
            spriteBatch.End();
        }

        /**
         * Helper function used to draw the path status of the tank
         */
        void DrawPathStatus(SpriteBatch& spriteBatch)
        {
            spriteBatch.Begin();

            std::string stepString =
                "Search Steps: " + std::to_string(pathFinder.getTotalSearchStepsProperty());
            spriteBatch.DrawString(*HUDFont, stepString, searchStepsPosition,
                Color::White);

            switch (pathFinder.getSearchStatusProperty())
            {
                case SearchStatus::Stopped:
                    spriteBatch.DrawString(
                        *HUDFont, "Not Searching", pathStatusPosition, Color::White);
                    break;
                case SearchStatus::Searching:
                    spriteBatch.DrawString(
                        *HUDFont, "Searching...", pathStatusPosition, Color::White);
                    break;
                case SearchStatus::PathFound:
                    spriteBatch.DrawString(
                        *HUDFont, "Path Found!", pathStatusPosition, Color::Green);
                    break;
                case SearchStatus::NoPath:
                    spriteBatch.DrawString(
                        *HUDFont, "No Path Found!", pathStatusPosition, Color::Red);
                    break;
                default:
                    break;
            }
            spriteBatch.End();
        }

        /**
         * Handle input for the sample
         */
        void HandleInput()
        {
            KeyboardState previousKeyboardState = currentKeyboardState;
            GamePadState previousGamePadState = currentGamePadState;

            currentGamePadState = GamePad::GetState(PlayerIndex::One);
            currentKeyboardState = Keyboard::GetState();

            // Allows the game to exit
            if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getBackProperty() ==
                ButtonState::Pressed)
                this->Exit();

            if ((previousGamePadState.getButtonsProperty().getAProperty() == ButtonState::Released &&
                 currentGamePadState.getButtonsProperty().getAProperty() == ButtonState::Pressed) ||
                (previousKeyboardState.IsKeyUp(Keys::A) &&
                 currentKeyboardState.IsKeyDown(Keys::A)))
            {
                pathFinder.setIsSearchingProperty(!pathFinder.getIsSearchingProperty());
            }

            if ((previousGamePadState.getButtonsProperty().getBProperty() == ButtonState::Released &&
                 currentGamePadState.getButtonsProperty().getBProperty() == ButtonState::Pressed) ||
                (previousKeyboardState.IsKeyUp(Keys::B) &&
                 currentKeyboardState.IsKeyDown(Keys::B)))
            {
                map.setMapReloadProperty(true);
            }

            if ((previousGamePadState.getButtonsProperty().getXProperty() == ButtonState::Released &&
                 currentGamePadState.getButtonsProperty().getXProperty() == ButtonState::Pressed) ||
                (previousKeyboardState.IsKeyUp(Keys::X) &&
                 currentKeyboardState.IsKeyDown(Keys::X)))
            {
                pathFinder.NextSearchType();
            }

            if ((previousGamePadState.getButtonsProperty().getYProperty() == ButtonState::Released &&
                 currentGamePadState.getButtonsProperty().getYProperty() == ButtonState::Pressed) ||
                (previousKeyboardState.IsKeyUp(Keys::Y) &&
                 currentKeyboardState.IsKeyDown(Keys::Y)))
            {
                map.CycleMap();
            }

            if ((previousGamePadState.getDPadProperty().getRightProperty() == ButtonState::Released &&
                 currentGamePadState.getDPadProperty().getRightProperty() == ButtonState::Pressed) ||
                (previousKeyboardState.IsKeyUp(Keys::Right) &&
                 currentKeyboardState.IsKeyDown(Keys::Right)))
            {
                pathFinder.timeStep += .1f;
            }

            if ((previousGamePadState.getDPadProperty().getLeftProperty() == ButtonState::Released &&
                 currentGamePadState.getDPadProperty().getLeftProperty() == ButtonState::Pressed) ||
                (previousKeyboardState.IsKeyUp(Keys::Left) &&
                 currentKeyboardState.IsKeyDown(Keys::Left)))
            {
                pathFinder.timeStep -= .1f;
            }

            pathFinder.timeStep = MathHelper::Clamp(pathFinder.timeStep, 0.0f, 1.0f);

            TouchCollection rawTouch = TouchPanel::GetState();

            // Use raw touch for the sliders
            if (rawTouch.getCountProperty() > 0)
            {
                // Only grab the first one
                TouchLocation touchLocation = rawTouch[0];

                // Create a collidable rectangle to determine if we touched the controls
                Rectangle touchRectangle = Rectangle((int)touchLocation.getPositionProperty().X,
                                                     (int)touchLocation.getPositionProperty().Y, 10, 10);

                // Have the sliders rely on the raw touch to function properly
                if (barTimeStep.Intersects(touchRectangle))
                {
                    pathFinder.timeStep =
                        (float)(touchRectangle.X - barTimeStep.X) / (float)barTimeStep.Width;
                }
            }

            // Next we handle all of the gestures. since we may have multiple gestures available,
            // we use a loop to read in all of the gestures. this is important to make sure the
            // TouchPanel's queue doesn't get backed up with old data
            while (TouchPanel::getIsGestureAvailableProperty())
            {
                // Read the next gesture from the queue
                GestureSample gesture = TouchPanel::ReadGesture();

                // Create a collidable rectangle to determine if we touched the controls
                Rectangle touch = Rectangle((int)gesture.getPositionProperty().X,
                                            (int)gesture.getPositionProperty().Y, 20, 20);

                // We can use the type of gesture to determine our behavior
                switch (gesture.getGestureTypeProperty())
                {
                    case GestureType::Tap:
                        if (buttonStartStop.Intersects(touch))
                        {
                            pathFinder.setIsSearchingProperty(!pathFinder.getIsSearchingProperty());
                        }
                        else if (buttonReset.Intersects(touch))
                        {
                            map.setMapReloadProperty(true);
                        }
                        else if (buttonPathfinding.Intersects(touch))
                        {
                            pathFinder.NextSearchType();
                        }
                        else if (buttonNextMap.Intersects(touch))
                        {
                            map.CycleMap();
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    };
}
