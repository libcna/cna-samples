// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// WaypointSample.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "WaypointSample.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/Math.hpp"

namespace Waypoint
{
    const std::string WaypointSample::helpText =
        "Use the arrow keys to move the cursor\n"
        "Press A to place a waypoint\n"
        "Press B to change steering behavior\n"
        "Press X to reset the tank and waypoints\n";

    WaypointSample::WaypointSample()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

#if defined(WINDOWS_PHONE)
        graphics.setPreferredBackBufferWidthProperty(480);
        graphics.setPreferredBackBufferHeightProperty(800);

        graphics.setSupportedOrientationsProperty(DisplayOrientation::Default);
        graphics.setIsFullScreenProperty(true);
#else
        // Pre-autoscale settings.
        graphics.setPreferredBackBufferWidthProperty(screenWidth);
        graphics.setPreferredBackBufferHeightProperty(screenHeight);
#endif

        tank = std::make_unique<Tank>(*this);
        getComponentsProperty().Add(tank.get());
    }

    const std::string& WaypointSample::GetTypeName() const
    {
        static const std::string name = "Waypoint.WaypointSample";
        return name;
    }

    void WaypointSample::Initialize()
    {
        // This places the HUD near the upper left corner of the screen
        hudLocation = Vector2(
            (float)System::Math::Floor(screenWidth * .1f),
            (float)System::Math::Floor(screenHeight * .1f));

        // places the cursor in the center of the screen
        cursorLocation =
            Vector2((float)screenWidth / 2, (float)screenHeight / 2);

        // places the tank halfway between the center of the screen and the
        // upper left corner
        tank->Reset(
            Vector2((float)screenWidth / 4, (float)screenHeight / 4));

        Game::Initialize();

#if defined(WINDOWS_PHONE)
        renderTarget = RenderTarget2D(graphics.getGraphicsDeviceProperty(), 800, 480);
#endif
    }

    void WaypointSample::LoadContent()
    {
        // Create a new SpriteBatch, which can be used to draw textures.
        spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());

        cursorTexture = getContentProperty().Load<Texture2D>("cursor");
        cursorCenter =
            Vector2((float)(cursorTexture.getWidthProperty() / 2),
                    (float)(cursorTexture.getHeightProperty() / 2));

        hudFont = getContentProperty().Load<SpriteFont>("HUDFont");

#if defined(WINDOWS_PHONE)
        blankTexture = getContentProperty().Load<Texture2D>("blank");
#endif
    }

    void WaypointSample::Update(GameTime& gameTime)
    {
        // Allows the game to exit
        if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getBackProperty() ==
            ButtonState::Pressed)
            this->Exit();

        float elapsedTime = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        HandleInput(elapsedTime);

        Game::Update(gameTime);
    }

    void WaypointSample::Draw(const GameTime& gameTime)
    {
#if defined(WINDOWS_PHONE)
        getGraphicsDeviceProperty().SetRenderTarget(&renderTarget);
#endif

        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

        Game::Draw(gameTime);

        std::string HudString =
            "Behavior Type: " + BehaviorTypeToString(tank->getBehaviorTypeProperty());

        spriteBatch->Begin();

#if defined(WINDOWS_PHONE)
        DrawMenuBar();
#endif

        // Draw the cursor
        spriteBatch->Draw(cursorTexture, cursorLocation, std::nullopt, Color::White, 0.0f,
            cursorCenter, 1.0f, SpriteEffects::None, 0.0f);

#if !defined(WINDOWS_PHONE)
        // Draw the string for current behavior
        spriteBatch->DrawString(*hudFont, HudString, hudLocation, Color::White);

        // draw our helper text so users know what they're doing.
        spriteBatch->DrawString(*hudFont, helpText, Vector2(10.0f, 250.0f), Color::White);
#endif

        spriteBatch->End();

#if defined(WINDOWS_PHONE)
        getGraphicsDeviceProperty().SetRenderTarget(nullptr);

        spriteBatch->Begin();
        spriteBatch->Draw(renderTarget,
           Vector2(240, 400),
           std::nullopt,
           Color::White,
           MathHelper::PiOver2,
           Vector2(400, 240),
           1.0f,
           SpriteEffects::None,
           0);
        spriteBatch->End();
#endif
    }

#if defined(WINDOWS_PHONE)
    void WaypointSample::DrawMenuBar()
    {
        // Draw white rectangle
        Rectangle rect = Rectangle(0, 0, screenWidth, menuBar_Height);
        spriteBatch->Draw(blankTexture, rect, Color::White);

        // Draw first "Button"
        Rectangle buttonRect1 = Rectangle(menuBarButton1_Left, menuBarButtonTop,
                                          menuBarButtonWidth, menuBarButtonHeight);
        spriteBatch->Draw(blankTexture, buttonRect1, Color::Orange);
        spriteBatch->DrawString(*hudFont, "Clear",
            Vector2((float)(menuBarButton1_Left + 60), (float)(menuBarButtonTop * 2)),
            Color::Black);

        // Draw second "Button"
        Rectangle buttonRect2 = Rectangle(menuBarButton2_Left, menuBarButtonTop,
                                          menuBarButtonWidth, menuBarButtonHeight);
        spriteBatch->Draw(blankTexture, buttonRect2, Color::Orange);
        spriteBatch->DrawString(*hudFont,
            BehaviorTypeToString(tank->getBehaviorTypeProperty()),
            Vector2((float)(menuBarButton2_Left + 50), (float)(menuBarButtonTop * 2)),
            Color::Black);
    }
#endif

    void WaypointSample::HandleInput(float elapsedTime)
    {
        previousGamePadState = currentGamePadState;
        previousKeyboardState = currentKeyboardState;
        currentGamePadState = GamePad::GetState(PlayerIndex::One);
        currentKeyboardState = Keyboard::GetState();

        // Allows the game to exit
        if (currentGamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed ||
            currentKeyboardState.IsKeyDown(Keys::Escape))
            this->Exit();

        // Update the cursor location by listening for left thumbstick input on
        // the GamePad and direction key input on the Keyboard, making sure to
        // keep the cursor inside the screen boundary
        cursorLocation.X +=
            currentGamePadState.getThumbSticksProperty().getLeftProperty().X *
            cursorMoveSpeed * elapsedTime;
        cursorLocation.Y -=
            currentGamePadState.getThumbSticksProperty().getLeftProperty().Y *
            cursorMoveSpeed * elapsedTime;

        if (currentKeyboardState.IsKeyDown(Keys::Up))
        {
            cursorLocation.Y -= elapsedTime * cursorMoveSpeed;
        }
        if (currentKeyboardState.IsKeyDown(Keys::Down))
        {
            cursorLocation.Y += elapsedTime * cursorMoveSpeed;
        }
        if (currentKeyboardState.IsKeyDown(Keys::Left))
        {
            cursorLocation.X -= elapsedTime * cursorMoveSpeed;
        }
        if (currentKeyboardState.IsKeyDown(Keys::Right))
        {
            cursorLocation.X += elapsedTime * cursorMoveSpeed;
        }

        cursorLocation.X = MathHelper::Clamp(cursorLocation.X, 0.0f, (float)screenWidth);
        cursorLocation.Y = MathHelper::Clamp(cursorLocation.Y, 0.0f, (float)screenHeight);

        // Change the tank move behavior if the user pressed B on
        // the GamePad or on the Keyboard.
        if ((previousGamePadState.getButtonsProperty().getBProperty() == ButtonState::Released &&
            currentGamePadState.getButtonsProperty().getBProperty() == ButtonState::Pressed) ||
            (previousKeyboardState.IsKeyUp(Keys::B) &&
            currentKeyboardState.IsKeyDown(Keys::B)))
        {
            tank->CycleBehaviorType();
        }

        // Add the cursor's location to the WaypointList if the user pressed A on
        // the GamePad or on the Keyboard.
        if ((previousGamePadState.getButtonsProperty().getAProperty() == ButtonState::Released &&
            currentGamePadState.getButtonsProperty().getAProperty() == ButtonState::Pressed) ||
            (previousKeyboardState.IsKeyUp(Keys::A) &&
            currentKeyboardState.IsKeyDown(Keys::A)))
        {
            tank->getWaypointsProperty().Enqueue(cursorLocation);
        }

        // Delete all the current waypoints and reset the tanks' location if
        // the user pressed X on the GamePad or on the Keyboard.
        if ((previousGamePadState.getButtonsProperty().getXProperty() == ButtonState::Released &&
            currentGamePadState.getButtonsProperty().getXProperty() == ButtonState::Pressed) ||
            (previousKeyboardState.IsKeyUp(Keys::X) &&
            currentKeyboardState.IsKeyDown(Keys::X)))
        {
            tank->Reset(
                Vector2((float)screenWidth / 4, (float)screenHeight / 4));
        }
    }
}
