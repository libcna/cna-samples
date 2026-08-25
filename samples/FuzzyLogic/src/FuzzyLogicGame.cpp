// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// FuzzyLogicGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "FuzzyLogicGame.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocation.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocationState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/Math.hpp"

namespace FuzzyLogic
{
    using namespace Microsoft::Xna::Framework::Input::Touch;

    FuzzyLogicGame::FuzzyLogicGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        graphics.setPreferredBackBufferWidthProperty(800);
        graphics.setPreferredBackBufferHeightProperty(480);
    }

    const std::string& FuzzyLogicGame::GetTypeName() const
    {
        static const std::string name = "FuzzyLogic.FuzzyLogicGame";
        return name;
    }

    void FuzzyLogicGame::Initialize()
    {
        // The level boundary is the viewable area but slightly
        // smaller to prevent the Entities from drawing off-screen
        levelBoundary =
            getGraphicsDeviceProperty().getViewportProperty().getTitleSafeAreaProperty();
        levelBoundary.X += 20;
        levelBoundary.Y += 20;
        levelBoundary.Width -= 40;
        levelBoundary.Height -= 40;

        // Now that we've created the graphics device, we can use its title
        // safe area to create the tank.
        tank = std::make_unique<Tank>(levelBoundary, mice);

        Game::Initialize();
    }

    void FuzzyLogicGame::LoadContent()
    {
        // Create a new SpriteBatch, which can be used to draw textures.
        spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());

        tank->LoadContent(getContentProperty());
        onePixelWhite.emplace(getGraphicsDeviceProperty(), 1, 1, false,
                              SurfaceFormat::Color);
        Color onePixelWhiteData[] = { Color::White };
        onePixelWhite->SetData(onePixelWhiteData, 1);
        font = getContentProperty().Load<SpriteFont>("hudFont");
    }

    void FuzzyLogicGame::Update(GameTime& gameTime)
    {
        HandleInput();

        tank->Update(gameTime);

        // Update all of the mice
        int i = 0;
        while (i < mice.getCountProperty())
        {
            // List<T>'s indexer hands back a mutation-tracking proxy whose operator->
            // stops at the element, so the shared_ptr is bound before it is followed.
            const std::shared_ptr<Mouse>& mouse = mice[i];
            mouse->Update(gameTime);

            // If the tank has caught any of the mice, remove them from the list.
            // who knows what happen to the mice after they're caught? Whatever it
            // is, it probably isn't pretty.
            if (Vector2::Distance(tank->getPositionProperty(),
                                  mouse->getPositionProperty()) <
                Tank::CaughtDistance)
            {
                mice.RemoveAt(i);
            }
            else
            {
                i++;
            }
        }

        // Now, if the tank has caught any mice, we'll have fewer than our desired
        // number, and we have to repopulate.
        while (mice.getCountProperty() < NumberOfMice)
        {
            std::shared_ptr<Mouse> mouse =
                std::make_shared<Mouse>(levelBoundary, tank.get());
            mouse->LoadContent(getContentProperty());
            mice.Add(mouse);
        }

        Game::Update(gameTime);
    }

    void FuzzyLogicGame::Draw(const GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(Color::DarkGray);

        spriteBatch->Begin();

        for (int i = 0; i < mice.getCountProperty(); i++)
        {
            const std::shared_ptr<Mouse>& mouse = mice[i];
            mouse->Draw(*spriteBatch, gameTime);
        }

        tank->Draw(*spriteBatch, gameTime);

        // Draw the three bars showing the tank's internal state.
        DrawBar(barDistance, tank->getFuzzyDistanceWeightProperty(), "Distance",
                gameTime, currentlySelectedWeight == 0);
        DrawBar(barAngle, tank->getFuzzyAngleWeightProperty(), "Angle",
                gameTime, currentlySelectedWeight == 1);
        DrawBar(barTime, tank->getFuzzyTimeWeightProperty(), "Time",
                gameTime, currentlySelectedWeight == 2);

        spriteBatch->End();

        Game::Draw(gameTime);
    }

    void FuzzyLogicGame::DrawBar(Rectangle bar, float barWidthNormalized,
                                 const String& label, const GameTime& gameTime,
                                 bool highlighted)
    {
        Color tintColor = Color::White;

        // if the bar is highlighted, we want to make it pulse with a red tint.
        if (highlighted)
        {
            // to do this, we'll first generate a value t, which we'll use to
            // determine how much tint to have.
            float t = (float)System::Math::Sin(
                10 * gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());

            // Sin varies from -1 to 1, and we want t to go from 0 to 1, so we'll
            // scale it now.
            t = .5f + .5f * t;

            // finally, we'll calculate our tint color by using Lerp to generate
            // a color in between Red and White.
            tintColor = Color(Vector4::Lerp(
                Color::Red.ToVector4(), Color::White.ToVector4(), t));
        }

        // calculate how wide the bar should be, and then draw it.
        bar.Width = (int)(bar.Width * barWidthNormalized);
        spriteBatch->Draw(*onePixelWhite, bar, tintColor);

        // finally, draw the label to the left of the bar.
        Vector2 labelSize = font->MeasureString(label);
        Vector2 labelPosition((float)bar.X - 5 - labelSize.X, (float)bar.Y);
        spriteBatch->DrawString(*font, label, labelPosition, tintColor);
    }

    bool FuzzyLogicGame::IsPressed(Keys key) const
    {
        return (currentKeyboardState.IsKeyUp(key) &&
            lastKeyboardState.IsKeyDown(key));
    }

    bool FuzzyLogicGame::IsPressed(Buttons button) const
    {
        return (currentGamePadState.IsButtonUp(button) &&
            lastGamePadState.IsButtonDown(button));
    }

    void FuzzyLogicGame::HandleInput()
    {
        currentKeyboardState = Keyboard::GetState();
        currentGamePadState = GamePad::GetState(PlayerIndex::One);

        // Check for exit.
        if (currentKeyboardState.IsKeyDown(Keys::Escape) ||
        currentGamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
        {
            Exit();
        }

        // Check to see whether the user wants to modify their currently selected
        // weight.
        if (IsPressed(Keys::Up) || IsPressed(Buttons::DPadUp) ||
            IsPressed(Buttons::LeftThumbstickUp))
        {
            currentlySelectedWeight--;
            if (currentlySelectedWeight < 0)
                currentlySelectedWeight = 2;
        }

        if (IsPressed(Keys::Down) || IsPressed(Buttons::DPadDown) ||
            IsPressed(Buttons::LeftThumbstickDown))
        {
            currentlySelectedWeight = (currentlySelectedWeight + 1) % 3;
        }

        // Figure out how much the user wants to change the current weight, if at
        // all. the input thumbsticks vary from -1 to 1, which is too much all at
        // once, so we'll scale it down a bit.
        float changeAmount = currentGamePadState.getThumbSticksProperty().getLeftProperty().X;

        TouchCollection touchState = TouchPanel::GetState();

        // Interpert touch screen presses - get only the first one for this specific case
        if (touchState.getCountProperty() > 0)
        {
            TouchLocation location = touchState[0];

            switch (location.getStateProperty())
            {
                case TouchLocationState::Pressed:
                {
                    // Save first touch coordinates
                    lastTouchPoint = location.getPositionProperty();

                    isDragging = true;

                    // Create a rectangle for the touch point
                    Rectangle touch((int)lastTouchPoint.X, (int)lastTouchPoint.Y, 20, 20);

                    // Check for collision with the bars
                    if (barDistance.Intersects(touch))
                        currentlySelectedWeight = 0;
                    else if (barAngle.Intersects(touch))
                        currentlySelectedWeight = 1;
                    else if (barTime.Intersects(touch))
                        currentlySelectedWeight = 2;

                    changeAmount = 0;
                    break;
                }
                case TouchLocationState::Moved:
                    if (isDragging && currentlySelectedWeight > -1)
                    {
                        float DragDelta = location.getPositionProperty().X - lastTouchPoint.X;

                        if (DragDelta > 0)
                            changeAmount = 1;
                        else if (DragDelta < 0)
                            changeAmount = -1.0f;
                    }
                    break;
                case TouchLocationState::Released:
                    // Make coordinates irrelevant
                    if (isDragging)
                    {
                        lastTouchPoint.X = -1;
                        lastTouchPoint.Y = -1;
                        isDragging = false;
                    }
                    break;
                default:
                    break;
            }
        }

        if (currentKeyboardState.IsKeyDown(Keys::Right) ||
            currentGamePadState.IsButtonDown(Buttons::DPadRight))
        {
            changeAmount = 1;
        }
        if (currentKeyboardState.IsKeyDown(Keys::Left) ||
            currentGamePadState.IsButtonDown(Buttons::DPadLeft))
        {
            changeAmount = -1.0f;
        }
        changeAmount *= .025f;

        // Apply to the changeAmount to the currentlySelectedWeight
        switch (currentlySelectedWeight)
        {
            case 0:
                tank->setFuzzyDistanceWeightProperty(
                    tank->getFuzzyDistanceWeightProperty() + changeAmount);
                break;
            case 1:
                tank->setFuzzyAngleWeightProperty(
                    tank->getFuzzyAngleWeightProperty() + changeAmount);
                break;
            case 2:
                tank->setFuzzyTimeWeightProperty(
                    tank->getFuzzyTimeWeightProperty() + changeAmount);
                break;
            default:
                break;
        }

        lastKeyboardState = currentKeyboardState;
        lastGamePadState = currentGamePadState;
    }
}
