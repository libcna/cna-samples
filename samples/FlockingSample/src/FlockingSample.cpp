// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// FlockingSample.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "FlockingSample.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/Math.hpp"
#include "System/TimeSpan.hpp"

namespace Flocking
{
    using namespace Microsoft::Xna::Framework::Input::Touch;

    FlockingSample::FlockingSample()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

#if defined(WINDOWS_PHONE)
        // Frame rate is 30 fps by default for Windows Phone.
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
        graphics.setIsFullScreenProperty(true);
#endif

        ResetAIParams();
    }

    const std::string& FlockingSample::GetTypeName() const
    {
        static const std::string name = "Flocking.FlockingSample";
        return name;
    }

    void FlockingSample::Initialize()
    {
        // Enable the gestures we care about. you must set EnabledGestures before
        // you can use any of the other gesture APIs.
        TouchPanel::setEnabledGesturesProperty(
            GestureType::Tap |
            GestureType::FreeDrag);

        Game::Initialize();
    }

    void FlockingSample::LoadContent()
    {
        spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        catTexture = getContentProperty().Load<Texture2D>("cat");
        birdTexture = getContentProperty().Load<Texture2D>("mouse");

#if defined(WINDOWS) || defined(XBOX)
        bButton = getContentProperty().Load<Texture2D>("xboxControllerButtonB");
        xButton = getContentProperty().Load<Texture2D>("xboxControllerButtonX");
        yButton = getContentProperty().Load<Texture2D>("xboxControllerButtonY");
#endif
        hudFont = getContentProperty().Load<SpriteFont>("HUDFont");

        onePixelWhite = Texture2D(
            getGraphicsDeviceProperty(), 1, 1, false, SurfaceFormat::Color);
        {
            Color pixel[] = { Color::White };
            onePixelWhite.SetData(pixel, 1);
        }
    }

    void FlockingSample::HandleInput()
    {
        inputState.Update();

        // Check for exit.
        if (inputState.getExitProperty())
        {
            Exit();
        }

        float dragDelta = 0.0f;

        // Check to see whether the user wants to modify their currently selected
        // weight.
        if (inputState.getUpProperty())
        {
            selectionNum--;
            if (selectionNum < 0)
                selectionNum = 1;
        }
        if (inputState.getDownProperty())
        {
            selectionNum = (selectionNum + 1) % 2;
        }

        // Update move for the cat
        if (cat != nullptr)
        {
            cat->HandleInput(inputState);
        }

        // Turn the cat on or off
        if (inputState.getToggleCatButtonProperty())
        {
            ToggleCat();
        }

        // Resets flock parameters back to default
        if (inputState.getResetDistancesProperty())
        {
            ResetAIParams();
            aiParameterUpdate = true;
        }

        // Resets the location and orientation of the members of the flock
        if (inputState.getResetFlockProperty())
        {
            flock->ResetFlock();
            aiParameterUpdate = true;
        }

        dragDelta = inputState.getSliderMoveProperty();

        // Apply to the changeAmount to the currentlySelectedWeight
        switch (selectionNum)
        {
            case 0:
                flockParams.DetectionDistance += dragDelta;
                break;
            case 1:
                flockParams.SeparationDistance += dragDelta;
                break;
            default:
                break;
        }

        if (dragDelta != 0.0f)
            aiParameterUpdate = true;

        // By default we can move the cat but if a touch registers against a control do not move the cat
        moveCat = true;

        TouchCollection rawTouch = TouchPanel::GetState();

        // Use raw touch for the sliders
        if (rawTouch.getCountProperty() > 0)
        {
            // Only grab the first one
            TouchLocation touchLocation = rawTouch[0];

            // Create a collidable rectangle to determine if we touched the controls
            Rectangle touchRectangle = Rectangle((int)touchLocation.getPositionProperty().X,
                                                 (int)touchLocation.getPositionProperty().Y, 20, 20);

            // Have the sliders rely on the raw touch to function properly
            SliderInputHelper(touchRectangle);
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
                    if (buttonResetDistance.Intersects(touch))
                    {
                        // Resets flock parameters back to default
                        ResetAIParams();
                        aiParameterUpdate = true;
                        moveCat = false;
                    }
                    else if (buttonResetFlock.Intersects(touch))
                    {
                        // Resets the location and orientation of the members of the flock
                        flock->ResetFlock();
                        aiParameterUpdate = true;
                        moveCat = false;
                    }
                    else if (buttonToggleCat.Intersects(touch))
                    {
                        ToggleCat();
                        moveCat = false;
                    }
                    break;
                default:
                    break;
            }

            // Check if we can move the cat
            if (cat != nullptr && moveCat)
            {
                // If we did not touch any controls then move the cat
                cat->setLocationProperty(gesture.getPositionProperty());
            }
        }

        // Clamp the slider values
        flockParams.DetectionDistance =
            MathHelper::Clamp(flockParams.DetectionDistance, sliderMin, sliderMax);
        flockParams.SeparationDistance =
            MathHelper::Clamp(flockParams.SeparationDistance, sliderMin, sliderMax);

        if (aiParameterUpdate)
        {
            flock->setFlockParamsProperty(flockParams);
        }
    }

    void FlockingSample::SliderInputHelper(Rectangle touchRectangle)
    {
        if (barDetectionDistance.Intersects(touchRectangle))
        {
            selectionNum = 0;
            aiParameterUpdate = true;
            moveCat = false;
            flockParams.DetectionDistance =
                (float)(touchRectangle.X - barDetectionDistance.X);
        }
        else if (barSeparationDistance.Intersects(touchRectangle))
        {
            selectionNum = 1;
            aiParameterUpdate = true;
            moveCat = false;
            // The original measures this one from barDetectionDistance.X too, not from
            // barSeparationDistance.X. Both bars share an X, so it makes no difference
            // here; it is left as written rather than "corrected".
            flockParams.SeparationDistance =
                (float)(touchRectangle.X - barDetectionDistance.X);
        }
    }

    void FlockingSample::Update(GameTime& gameTime)
    {
        HandleInput();

        if (cat != nullptr)
        {
            cat->Update(gameTime);
        }

        if (flock != nullptr)
        {
            flock->Update(gameTime, cat.get());
        }
        else
        {
            SpawnFlock();
        }

        Game::Update(gameTime);
    }

    void FlockingSample::Draw(const GameTime& gameTime)
    {
        graphics.getGraphicsDeviceProperty()->Clear(Color::CornflowerBlue);

        spriteBatch->Begin();

        if (flock != nullptr)
        {
            flock->Draw(*spriteBatch, gameTime);
        }

        if (cat != nullptr)
        {
            cat->Draw(*spriteBatch, gameTime);
        }

        // Draw all the HUD elements
        DrawBar(barDetectionDistance, flockParams.DetectionDistance / 100.0f,
                "Detection Distance:", gameTime, selectionNum == 0);
        DrawBar(barSeparationDistance, flockParams.SeparationDistance / 100.0f,
                "Separation  Distance:", gameTime, selectionNum == 1);

#if defined(WINDOWS_PHONE)
        DrawButton(buttonResetDistance, "Reset Distance");
        DrawButton(buttonResetFlock, "Reset Flock");
        DrawButton(buttonToggleCat, "Add/Remove Cat");
#else
        spriteBatch->Draw(bButton,
            Vector2(hudLocX + 110.0f, hudLocY), Color::White);
        spriteBatch->Draw(xButton,
            Vector2(hudLocX + 110.0f, hudLocY + 20.0f), Color::White);
        spriteBatch->Draw(yButton,
            Vector2(hudLocX + 110.0f, hudLocY + 40.0f), Color::White);
        spriteBatch->DrawString(*hudFont, "Reset Distances",
            Vector2(hudLocX + 135.0f, hudLocY), Color::White);
        spriteBatch->DrawString(*hudFont, "Reset flock",
            Vector2(hudLocX + 135.0f, hudLocY + 20.0f), Color::White);
        spriteBatch->DrawString(*hudFont, "Spawn/remove cat",
            Vector2(hudLocX + 135.0f, hudLocY + 40.0f), Color::White);
#endif

        spriteBatch->End();

        Game::Draw(gameTime);
    }

    void FlockingSample::DrawButton(Rectangle button, const std::string& label)
    {
        spriteBatch->Draw(onePixelWhite, button, Color::Orange);
        spriteBatch->DrawString(*hudFont, label,
            Vector2((float)(button.getLeftProperty() + 10),
                    (float)(button.getTopProperty() + 10)), Color::Black);
    }

    void FlockingSample::DrawBar(Rectangle bar, float barWidthNormalized,
                                 const std::string& label, const GameTime& gameTime,
                                 bool highlighted)
    {
        Color tintColor = Color::White;

        // If the bar is highlighted, we want to make it pulse with a red tint.
        if (highlighted)
        {
            // To do this, we'll first generate a value t, which we'll use to
            // determine how much tint to have.
            float t = (float)System::Math::Sin(
                10 * gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());

            // Sin varies from -1 to 1, and we want t to go from 0 to 1, so we'll
            // scale it now.
            t = .5f + .5f * t;

            // Finally, we'll calculate our tint color by using Lerp to generate
            // a color in between Red and White.
            tintColor = Color(Vector4::Lerp(
                Color::Red.ToVector4(), Color::White.ToVector4(), t));
        }

        // Calculate how wide the bar should be, and then draw it.
        bar.Height /= 2;
        spriteBatch->Draw(onePixelWhite, bar, Color::White);

        // Draw the slider
        spriteBatch->Draw(onePixelWhite, Rectangle(bar.X + (int)(bar.Width * barWidthNormalized),
                          bar.Y - bar.Height / 2, sliderButtonWidth, bar.Height * 2), Color::Orange);

        // Finally, draw the label to the left of the bar.
        Vector2 labelSize = hudFont->MeasureString(label);
        Vector2 labelPosition = Vector2(bar.X - 5 - labelSize.X, (float)bar.Y);
        spriteBatch->DrawString(*hudFont, label, labelPosition, tintColor);
    }

    void FlockingSample::SpawnFlock()
    {
        if (flock == nullptr)
        {
            flock = std::make_unique<Flock>(birdTexture,
                getGraphicsDeviceProperty().getViewportProperty().getTitleSafeAreaProperty().Width,
                getGraphicsDeviceProperty().getViewportProperty().getTitleSafeAreaProperty().Height,
                flockParams);
        }
    }

    void FlockingSample::ResetAIParams()
    {
        flockParams.DetectionDistance = detectionDefault;
        flockParams.SeparationDistance = separationDefault;
        flockParams.MoveInOldDirectionInfluence = moveInOldDirInfluenceDefault;
        flockParams.MoveInFlockDirectionInfluence = moveInFlockDirInfluenceDefault;
        flockParams.MoveInRandomDirectionInfluence = moveInRandomDirInfluenceDefault;
        flockParams.MaxTurnRadians = maxTurnRadiansDefault;
        flockParams.PerMemberWeight = perMemberWeightDefault;
        flockParams.PerDangerWeight = perDangerWeightDefault;
    }

    void FlockingSample::ToggleCat()
    {
        if (cat == nullptr)
        {
            cat = std::make_unique<Cat>(catTexture,
                getGraphicsDeviceProperty().getViewportProperty().getTitleSafeAreaProperty().Width,
                getGraphicsDeviceProperty().getViewportProperty().getTitleSafeAreaProperty().Height);
        }
        else
        {
            cat = nullptr;
        }
    }
}
