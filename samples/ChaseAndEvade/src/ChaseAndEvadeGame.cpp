// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ChaseAndEvadeGame.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DisplayOrientation.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "System/Math.hpp"
#include "System/TimeSpan.hpp"

namespace ChaseAndEvade
{
    using namespace Microsoft::Xna::Framework::Input;

    ChaseAndEvadeGame::ChaseAndEvadeGame()
        : graphics(this)
    {
        graphics.setSupportedOrientationsProperty(DisplayOrientation::Portrait);
        getContentProperty().setRootDirectoryProperty("Content");

#if defined(WINDOWS_PHONE)
        graphics.setPreferredBackBufferWidthProperty(480);
        graphics.setPreferredBackBufferHeightProperty(800);
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
        graphics.setIsFullScreenProperty(true);
#else
        graphics.setPreferredBackBufferWidthProperty(853);
        graphics.setPreferredBackBufferHeightProperty(480);
#endif
    }

    const std::string& ChaseAndEvadeGame::GetTypeName() const
    {
        static const std::string name = "ChaseAndEvade.ChaseAndEvadeGame";
        return name;
    }

    std::string ChaseAndEvadeGame::ToString(TankAiState state)
    {
        switch (state)
        {
            case TankAiState::Chasing: return "Chasing";
            case TankAiState::Caught:  return "Caught";
            case TankAiState::Wander:  return "Wander";
        }
        return "Wander";
    }

    std::string ChaseAndEvadeGame::ToString(MouseAiState state)
    {
        switch (state)
        {
            case MouseAiState::Evading: return "Evading";
            case MouseAiState::Wander:  return "Wander";
        }
        return "Wander";
    }

    void ChaseAndEvadeGame::Initialize()
    {
        Game::Initialize();

        // once base.Initialize has finished, the GraphicsDevice will have been
        // created, and we'll know how big the Viewport is. We want the tank, cat
        // and mouse to be spread out across the screen, so we'll use the viewport
        // to figure out where they should be.
        Viewport vp = graphics.getGraphicsDeviceProperty()->getViewportProperty();
        tankPosition = Vector2((float)(vp.getWidthProperty() / 4),
                               (float)(vp.getHeightProperty() / 2));
        catPosition = Vector2((float)(vp.getWidthProperty() / 2),
                              (float)(vp.getHeightProperty() / 2));
        mousePosition = Vector2((float)(3 * vp.getWidthProperty() / 4),
                                (float)(vp.getHeightProperty() / 2));
    }

    void ChaseAndEvadeGame::LoadContent()
    {
        // create a SpriteBatch, and load the textures and font that we'll need
        // during the game.
        spriteBatch = std::make_unique<SpriteBatch>(*graphics.getGraphicsDeviceProperty());

        spriteFont = getContentProperty().Load<SpriteFont>("hudFont");
        tankTexture = getContentProperty().Load<Texture2D>("Tank");
        catTexture = getContentProperty().Load<Texture2D>("Cat");
        mouseTexture = getContentProperty().Load<Texture2D>("Mouse");

        // once all the content is loaded, we can calculate the centers of each
        // of the textures that we loaded. SpriteBatch.Draw will center the sprite
        // on the vector that we pass in as the "origin" parameter, so we'll just
        // calculate that to be the middle of the texture.
        tankTextureCenter =
            Vector2((float)(tankTexture.getWidthProperty() / 2),
                    (float)(tankTexture.getHeightProperty() / 2));
        catTextureCenter =
            Vector2((float)(catTexture.getWidthProperty() / 2),
                    (float)(catTexture.getHeightProperty() / 2));
        mouseTextureCenter =
            Vector2((float)(mouseTexture.getWidthProperty() / 2),
                    (float)(mouseTexture.getHeightProperty() / 2));
    }

    void ChaseAndEvadeGame::Update(GameTime& gameTime)
    {
        // handle input will read the controller input, and update the cat
        // to move according to the user's whim.
        HandleInput();

        // UpdateTank will run the AI code that controls the tank's movement...
        UpdateTank();

        // ... and UpdateMouse does the same thing for the mouse.
        UpdateMouse();

        // once we're done with that, we'll use the ClampToViewport helper function
        // to clamp everyone's position so that they stay on the screen.
        tankPosition = ClampToViewport(tankPosition);
        catPosition = ClampToViewport(catPosition);
        mousePosition = ClampToViewport(mousePosition);

        Game::Update(gameTime);
    }

    Vector2 ChaseAndEvadeGame::ClampToViewport(Vector2 vector)
    {
        Viewport vp = graphics.getGraphicsDeviceProperty()->getViewportProperty();
        vector.X = MathHelper::Clamp(vector.X, (float)vp.getXProperty(),
                                     (float)(vp.getXProperty() + vp.getWidthProperty()));
        vector.Y = MathHelper::Clamp(vector.Y, (float)vp.getYProperty(),
                                     (float)(vp.getYProperty() + vp.getHeightProperty()));
        return vector;
    }

    void ChaseAndEvadeGame::UpdateMouse()
    {
        // first, we'll calculate the mouse's distance from the cat, and use that
        // to decide whether it should be evading or wandering.
        float distanceFromCat = Vector2::Distance(mousePosition, catPosition);
        if (distanceFromCat > MouseEvadeDistance + MouseHysteresis)
        {
            mouseState = MouseAiState::Wander;
        }
        else if (distanceFromCat < MouseEvadeDistance - MouseHysteresis)
        {
            mouseState = MouseAiState::Evading;
        }

        float currentMouseSpeed;
        if (mouseState == MouseAiState::Evading)
        {
            // if the mouse is evading, we want it to move away from the cat, so we
            // aim at a point on the far side of the mouse from the cat.
            Vector2 seekPosition = 2 * mousePosition - catPosition;
            mouseOrientation = TurnToFace(mousePosition, seekPosition,
                mouseOrientation, MouseTurnSpeed);
            currentMouseSpeed = MaxMouseSpeed;
        }
        else
        {
            Wander(mousePosition, mouseWanderDirection, mouseOrientation,
                MouseTurnSpeed);
            currentMouseSpeed = .25f * MaxMouseSpeed;
        }

        Vector2 heading = Vector2(
            (float)System::Math::Cos(mouseOrientation),
            (float)System::Math::Sin(mouseOrientation));
        mousePosition += heading * currentMouseSpeed;
    }

    void ChaseAndEvadeGame::UpdateTank()
    {
        // first, we need to use the tank's current state to decide what its
        // thresholds should be.
        float tankChaseThreshold = TankChaseDistance;
        float tankCaughtThreshold = TankCaughtDistance;

        if (tankState == TankAiState::Wander)
        {
            tankChaseThreshold -= TankHysteresis / 2;
        }
        else if (tankState == TankAiState::Chasing)
        {
            tankChaseThreshold += TankHysteresis / 2;
            tankCaughtThreshold -= TankHysteresis / 2;
        }
        else if (tankState == TankAiState::Caught)
        {
            tankCaughtThreshold += TankHysteresis / 2;
        }

        float distanceFromCat = Vector2::Distance(tankPosition, catPosition);
        if (distanceFromCat > tankChaseThreshold)
        {
            tankState = TankAiState::Wander;
        }
        else if (distanceFromCat > tankCaughtThreshold)
        {
            tankState = TankAiState::Chasing;
        }
        else
        {
            tankState = TankAiState::Caught;
        }

        float currentTankSpeed;
        if (tankState == TankAiState::Chasing)
        {
            tankOrientation = TurnToFace(tankPosition, catPosition, tankOrientation,
                TankTurnSpeed);
            currentTankSpeed = MaxTankSpeed;
        }
        else if (tankState == TankAiState::Wander)
        {
            Wander(tankPosition, tankWanderDirection, tankOrientation,
                TankTurnSpeed);
            currentTankSpeed = .25f * MaxTankSpeed;
        }
        else
        {
            currentTankSpeed = 0.0f;
        }

        Vector2 heading = Vector2(
            (float)System::Math::Cos(tankOrientation),
            (float)System::Math::Sin(tankOrientation));
        tankPosition += heading * currentTankSpeed;
    }

    void ChaseAndEvadeGame::Wander(Vector2 position, Vector2& wanderDirection,
        float& orientation, float turnSpeed)
    {
        // The wander effect is accomplished by having the character aim in a random
        // direction. Every frame, this random direction is slightly modified.
        wanderDirection.X +=
            MathHelper::Lerp(-.25f, .25f, (float)random.NextDouble());
        wanderDirection.Y +=
            MathHelper::Lerp(-.25f, .25f, (float)random.NextDouble());

        if (wanderDirection != Vector2::Zero)
        {
            wanderDirection.Normalize();
        }

        orientation = TurnToFace(position, position + wanderDirection, orientation,
            .15f * turnSpeed);

        // if the character is outside the screen, we want to turn it back towards
        // the center.
        Vector2 screenCenter = Vector2::Zero;
        screenCenter.X = (float)(graphics.getGraphicsDeviceProperty()->getViewportProperty()
                                     .getWidthProperty() / 2);
        screenCenter.Y = (float)(graphics.getGraphicsDeviceProperty()->getViewportProperty()
                                     .getHeightProperty() / 2);

        float distanceFromScreenCenter = Vector2::Distance(screenCenter, position);
        float MaxDistanceFromScreenCenter =
            System::Math::Min(screenCenter.Y, screenCenter.X);

        float normalizedDistance =
            distanceFromScreenCenter / MaxDistanceFromScreenCenter;

        float turnToCenterSpeed = .3f * normalizedDistance * normalizedDistance *
            turnSpeed;

        orientation = TurnToFace(position, screenCenter, orientation,
            turnToCenterSpeed);
    }

    float ChaseAndEvadeGame::TurnToFace(Vector2 position, Vector2 faceThis,
        float currentAngle, float turnSpeed)
    {
        float x = faceThis.X - position.X;
        float y = faceThis.Y - position.Y;

        float desiredAngle = (float)System::Math::Atan2(y, x);

        float difference = WrapAngle(desiredAngle - currentAngle);

        difference = MathHelper::Clamp(difference, -turnSpeed, turnSpeed);

        return WrapAngle(currentAngle + difference);
    }

    float ChaseAndEvadeGame::WrapAngle(float radians)
    {
        while (radians < -MathHelper::Pi)
        {
            radians += MathHelper::TwoPi;
        }
        while (radians > MathHelper::Pi)
        {
            radians -= MathHelper::TwoPi;
        }
        return radians;
    }

    void ChaseAndEvadeGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();
        device.Clear(Color::CornflowerBlue);

        spriteBatch->Begin();

        spriteBatch->Draw(tankTexture, tankPosition, std::nullopt, Color::White,
            tankOrientation, tankTextureCenter, 1.0f, SpriteEffects::None, 0.0f);
        spriteBatch->Draw(catTexture, catPosition, std::nullopt, Color::White,
            0.0f, catTextureCenter, 1.0f, SpriteEffects::None, 0.0f);
        spriteBatch->Draw(mouseTexture, mousePosition, std::nullopt, Color::White,
            mouseOrientation, mouseTextureCenter, 1.0f, SpriteEffects::None, 0.0f);

        // draw some text showing the tank's and mouse's current states.
        Vector2 shadowOffset = Vector2::One;
        spriteBatch->DrawString(*spriteFont, "Tank State: " + ToString(tankState),
            Vector2(50, 50) + shadowOffset, Color::Black);
        spriteBatch->DrawString(*spriteFont, "Tank State: " + ToString(tankState),
            Vector2(50, 50), Color::White);

        spriteBatch->DrawString(*spriteFont, "Mouse State: " + ToString(mouseState),
            Vector2(50, 75) + shadowOffset, Color::Black);
        spriteBatch->DrawString(*spriteFont, "Mouse State: " + ToString(mouseState),
            Vector2(50, 75), Color::White);

        spriteBatch->End();

        Game::Draw(gameTime);
    }

    void ChaseAndEvadeGame::HandleInput()
    {
#if defined(WINDOWS_PHONE)
        KeyboardState currentKeyboardState = KeyboardState();
#else
        KeyboardState currentKeyboardState = Keyboard::GetState();
#endif
        GamePadState currentGamePadState = GamePad::GetState(PlayerIndex::One);
        MouseState currentMouseState = Mouse::GetState();

        // Check for exit.
        if (currentKeyboardState.IsKeyDown(Keys::Escape) ||
            currentGamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
        {
            Exit();
        }

        // check for cat movement
        Vector2 catMovement = currentGamePadState.getThumbSticksProperty().getLeftProperty();
        catMovement.Y *= -1;

        if (currentKeyboardState.IsKeyDown(Keys::Left) ||
            currentGamePadState.getDPadProperty().getLeftProperty() == ButtonState::Pressed)
        {
            catMovement.X -= 1.0f;
        }
        if (currentKeyboardState.IsKeyDown(Keys::Right) ||
            currentGamePadState.getDPadProperty().getRightProperty() == ButtonState::Pressed)
        {
            catMovement.X += 1.0f;
        }
        if (currentKeyboardState.IsKeyDown(Keys::Up) ||
            currentGamePadState.getDPadProperty().getUpProperty() == ButtonState::Pressed)
        {
            catMovement.Y -= 1.0f;
        }
        if (currentKeyboardState.IsKeyDown(Keys::Down) ||
            currentGamePadState.getDPadProperty().getDownProperty() == ButtonState::Pressed)
        {
            catMovement.Y += 1.0f;
        }

        float smoothStop = 1;

        // The original names this local `mousePosition`, shadowing the field of the same
        // name that holds the mouse *sprite's* position. It is the pointer's position, and
        // the field is not used in this scope, so the shadowing is harmless -- kept as
        // written rather than renamed.
        Vector2 mousePosition = Vector2((float)currentMouseState.getXProperty(),
                                        (float)currentMouseState.getYProperty());
        if (currentMouseState.getLeftButtonProperty() == ButtonState::Pressed &&
            mousePosition != catPosition)
        {
            catMovement = mousePosition - catPosition;
            float delta = MaxCatSpeed -
                MathHelper::Clamp(catMovement.Length(), 0.0f, MaxCatSpeed);
            smoothStop = 1 - delta / MaxCatSpeed;
        }

        if (catMovement != Vector2::Zero)
        {
            catMovement.Normalize();
        }

        catPosition += catMovement * MaxCatSpeed * smoothStop;
    }
}
