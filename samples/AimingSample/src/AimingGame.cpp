// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "AimingGame.hpp"

#include <optional>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DisplayOrientation.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "System/Math.hpp"
#include "System/TimeSpan.hpp"

namespace Aiming
{
    using namespace Microsoft::Xna::Framework::Input;

    AimingGame::AimingGame()
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

    const std::string& AimingGame::GetTypeName() const
    {
        static const std::string name = "Aiming.AimingGame";
        return name;
    }

    void AimingGame::Initialize()
    {
        Game::Initialize();

        // once base.Initialize has finished, the GraphicsDevice will have been
        // created, and we'll know how big the Viewport is. We want the spotlight
        // to be centered in the middle of the screen, so we'll use the viewport
        // to calculate where that is.
        Viewport vp = graphics.getGraphicsDeviceProperty()->getViewportProperty();
        spotlightPosition.X = (float)(vp.getXProperty() + vp.getWidthProperty() / 2);
        spotlightPosition.Y = (float)(vp.getYProperty() + vp.getHeightProperty() / 2);

        // we'll use the viewport size again, this time to put the cat on the
        // screen. He goes 1/4 of the way across and halfway down.
        catPosition.X = (float)(vp.getXProperty() + vp.getWidthProperty() / 4);
        catPosition.Y = (float)(vp.getYProperty() + vp.getHeightProperty() / 2);
    }

    void AimingGame::LoadContent()
    {
        // load our textures, and create a sprite batch...
        spotlightTexture = getContentProperty().Load<Texture2D>("spotlight");
        catTexture = getContentProperty().Load<Texture2D>("cat");
        spriteBatch = std::make_unique<SpriteBatch>(*graphics.getGraphicsDeviceProperty());

        // now that we've loaded our textures, we can use them to calculate some
        // values that we'll use when drawing them. When we draw the spotlight,
        // it needs to rotate around the "source" of the light. since
        // spriteBatch.Draw will rotate sprites around the "origin" parameter,
        // we need spotlightOrigin to be the "source" of the light. Since I drew
        // spotlight.png myself, I happen to know that the source is halfway
        // down the left hand side of the texture.
        spotlightOrigin.X = 0;
        spotlightOrigin.Y = (float)(spotlightTexture.getHeightProperty() / 2);

        // Next, we want spriteBatch to draw the cat texture centered on the
        // "catPosition" vector. SpriteBatch.Draw will center the sprite on the
        // "origin" parameter, so we'll just calculate that to be the middle of
        // the texture.
        catOrigin.X = (float)(catTexture.getWidthProperty() / 2);
        catOrigin.Y = (float)(catTexture.getHeightProperty() / 2);
    }

    void AimingGame::Update(GameTime& gameTime)
    {
        HandleInput();

        // clamp the cat's position so that it stays on the screen.
        Viewport vp = graphics.getGraphicsDeviceProperty()->getViewportProperty();
        catPosition.X = MathHelper::Clamp(catPosition.X, (float)vp.getXProperty(),
                                          (float)(vp.getXProperty() + vp.getWidthProperty()));
        catPosition.Y = MathHelper::Clamp(catPosition.Y, (float)vp.getYProperty(),
                                          (float)(vp.getYProperty() + vp.getHeightProperty()));

        // use the TurnToFace function to update the spotlightAngle to face
        // towards the cat.
        spotlightAngle = TurnToFace(spotlightPosition, catPosition, spotlightAngle,
            SpotlightTurnSpeed);

        Game::Update(gameTime);
    }

    float AimingGame::TurnToFace(Vector2 position, Vector2 faceThis,
        float currentAngle, float turnSpeed)
    {
        // consider this diagram:
        //         C
        //        /|
        //      /  |
        //    /    | y
        //  / o    |
        // S--------
        //     x
        //
        // where S is the position of the spot light, C is the position of the cat,
        // and "o" is the angle that the spot light should be facing in order to
        // point at the cat. we need to know what o is. using trig, we know that
        //      tan(theta)       = opposite / adjacent
        //      tan(o)           = y / x
        // if we take the arctan of both sides of this equation...
        //      arctan( tan(o) ) = arctan( y / x )
        //      o                = arctan( y / x )
        // so, we can use x and y to find o, our "desiredAngle."
        // x and y are just the differences in position between the two objects.
        float x = faceThis.X - position.X;
        float y = faceThis.Y - position.Y;

        // we'll use the Atan2 function. Atan will calculates the arc tangent of
        // y / x for us, and has the added benefit that it will use the signs of x
        // and y to determine what cartesian quadrant to put the result in.
        float desiredAngle = (float)System::Math::Atan2(y, x);

        // so now we know where we WANT to be facing, and where we ARE facing...
        // if we weren't constrained by turnSpeed, this would be easy: we'd just
        // return desiredAngle.
        // instead, we have to calculate how much we WANT to turn, and then make
        // sure that's not more than turnSpeed.
        float difference = WrapAngle(desiredAngle - currentAngle);

        difference = MathHelper::Clamp(difference, -turnSpeed, turnSpeed);

        // so, the closest we can get to our target is currentAngle + difference.
        // return that, using WrapAngle to keep in the -Pi to Pi range.
        return WrapAngle(currentAngle + difference);
    }

    float AimingGame::WrapAngle(float radians)
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

    void AimingGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();
        device.Clear(Color::Black);

        // draw the cat.
        spriteBatch->Begin();
        spriteBatch->Draw(catTexture, catPosition, std::nullopt, Color::White,
            0.0f, catOrigin, 1.0f, SpriteEffects::None, 0.0f);
        spriteBatch->End();

        // draw the spotlight in its own batch, with additive blending, so it glows
        // over whatever is already on the screen.
        spriteBatch->Begin(SpriteSortMode::FrontToBack, BlendState::Additive);
        spriteBatch->Draw(spotlightTexture, spotlightPosition, std::nullopt, Color::White,
            spotlightAngle, spotlightOrigin, 1.0f, SpriteEffects::None, 0.0f);
        spriteBatch->End();

        Game::Draw(gameTime);
    }

    void AimingGame::HandleInput()
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

        Vector2 mousePosition = Vector2((float)currentMouseState.getXProperty(),
                                        (float)currentMouseState.getYProperty());
        if (currentMouseState.getLeftButtonProperty() == ButtonState::Pressed &&
            mousePosition != catPosition)
        {
            catMovement = mousePosition - catPosition;
            float delta = CatSpeed - MathHelper::Clamp(catMovement.Length(), 0.0f, CatSpeed);
            smoothStop = 1 - delta / CatSpeed;
        }

        if (catMovement != Vector2::Zero)
        {
            catMovement.Normalize();
        }

        catPosition += catMovement * CatSpeed * smoothStop;
    }
}
