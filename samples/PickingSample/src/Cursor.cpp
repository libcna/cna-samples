// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Cursor.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Cursor.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"

namespace PickingSample
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::SpriteEffects;
    using Microsoft::Xna::Framework::Graphics::Viewport;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::GamePadState;
    using Microsoft::Xna::Framework::Input::Mouse;
    using Microsoft::Xna::Framework::Input::MouseState;
    using Microsoft::Xna::Framework::Input::Touch::TouchCollection;
    using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

    Cursor::Cursor(Game& game)
        : DrawableGameComponent(game)
    {
    }

    const std::string& Cursor::GetTypeName() const
    {
        static const std::string name = "PickingSample.Cursor";
        return name;
    }

    void Cursor::LoadContent()
    {
        cursorTexture = getGameProperty().getContentProperty().Load<Texture2D>("cursor");
        textureCenter = Vector2(static_cast<float>(cursorTexture->getWidthProperty() / 2),
                                static_cast<float>(cursorTexture->getHeightProperty() / 2));

        spriteBatch.emplace(getGraphicsDeviceProperty());

        // we want to default the cursor to start in the center of the screen
        const Viewport vp = getGraphicsDeviceProperty().getViewportProperty();
        position.X = static_cast<float>(vp.getXProperty() + (vp.getWidthProperty() / 2));
        position.Y = static_cast<float>(vp.getYProperty() + (vp.getHeightProperty() / 2));

        DrawableGameComponent::LoadContent();
    }

    void Cursor::Update(GameTime& gameTime)
    {
        // We use different input on each platform:
        // On Xbox, we use the GamePad's DPad and left thumbstick to move the cursor around the
        // screen. On Windows, we directly map the cursor to the location of the mouse.
        // On Windows Phone, we use the primary touch point for the location of the cursor.
        //
        // The original selects between the three with #if XBOX / #elif WINDOWS / #elif
        // WINDOWS_PHONE. This port is the Windows build -- which is also the executable the audit
        // compares against -- so it takes the Windows branch. The other two are ported and kept so
        // the translation is complete; nothing calls them here, exactly as nothing calls them in a
        // Windows build of the original.
        static_cast<void>(gameTime);
        UpdateWindowsInput();
    }

    void Cursor::UpdateXboxInput(GameTime& gameTime)
    {
        const GamePadState currentState = GamePad::GetState(PlayerIndex::One);

        // we'll create a vector2, called delta, which will store how much the
        // cursor position should change.
        Vector2 delta = currentState.getThumbSticksProperty().getLeftProperty();

        // down on the thumbstick is -1. however, in screen coordinates, values
        // increase as they go down the screen. so, we have to flip the sign of the
        // y component of delta.
        delta.Y *= -1;

        // check the dpad: if any of its buttons are pressed, that will change delta as well.
        if (currentState.getDPadProperty().getUpProperty() == ButtonState::Pressed)
        {
            delta.Y = -1;
        }
        if (currentState.getDPadProperty().getDownProperty() == ButtonState::Pressed)
        {
            delta.Y = 1;
        }
        if (currentState.getDPadProperty().getLeftProperty() == ButtonState::Pressed)
        {
            delta.X = -1;
        }
        if (currentState.getDPadProperty().getRightProperty() == ButtonState::Pressed)
        {
            delta.X = 1;
        }

        // normalize delta so that we know the cursor can't move faster than CursorSpeed.
        if (delta != Vector2::Zero)
        {
            delta.Normalize();
        }

        // modify position using delta, the CursorSpeed constant defined above, and
        // the elapsed game time.
        position += delta * CursorSpeed
                  * static_cast<float>(gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());

        // clamp the cursor position to the viewport, so that it can't move off the screen.
        const Viewport vp = getGraphicsDeviceProperty().getViewportProperty();
        position.X = MathHelper::Clamp(position.X, static_cast<float>(vp.getXProperty()),
                                       static_cast<float>(vp.getXProperty() + vp.getWidthProperty()));
        position.Y = MathHelper::Clamp(position.Y, static_cast<float>(vp.getYProperty()),
                                       static_cast<float>(vp.getYProperty() + vp.getHeightProperty()));
    }

    void Cursor::UpdateWindowsInput()
    {
        const MouseState mouseState = Mouse::GetState();
        position.X = static_cast<float>(mouseState.getXProperty());
        position.Y = static_cast<float>(mouseState.getYProperty());
    }

    void Cursor::UpdateWindowsPhoneInput()
    {
        const TouchCollection touches = TouchPanel::GetState();
        if (touches.getCountProperty() > 0)
        {
            position = touches[0].getPositionProperty();
        }
    }

    void Cursor::Draw(const GameTime& gameTime)
    {
        spriteBatch->Begin();

        // use textureCenter as the origin of the sprite, so that the cursor is
        // drawn centered around Position.
        spriteBatch->Draw(*cursorTexture, getPositionProperty(), std::nullopt, Color::White, 0.0f,
                          textureCenter, Vector2::One, SpriteEffects::None, 0.0f);

        spriteBatch->End();
        static_cast<void>(gameTime);
    }

    Ray Cursor::CalculateCursorRay(const Matrix& projectionMatrix, const Matrix& viewMatrix)
    {
        // create 2 positions in screenspace using the cursor position. 0 is as
        // close as possible to the camera, 1 is as far away as possible.
        const Vector3 nearSource(getPositionProperty(), 0.0f);
        const Vector3 farSource(getPositionProperty(), 1.0f);

        // use Viewport.Unproject to tell what those two screen space positions
        // would be in world space. we'll need the projection matrix and view
        // matrix, which we have saved as member variables. We also need a world
        // matrix, which can just be identity.
        const Vector3 nearPoint = getGraphicsDeviceProperty().getViewportProperty().Unproject(
            nearSource, projectionMatrix, viewMatrix, Matrix::getIdentityProperty());

        const Vector3 farPoint = getGraphicsDeviceProperty().getViewportProperty().Unproject(
            farSource, projectionMatrix, viewMatrix, Matrix::getIdentityProperty());

        // find the direction vector that goes from the nearPoint to the farPoint
        // and normalize it....
        Vector3 direction = farPoint - nearPoint;
        direction.Normalize();

        // and then create a new ray using nearPoint as the source.
        return Ray(nearPoint, direction);
    }
}
