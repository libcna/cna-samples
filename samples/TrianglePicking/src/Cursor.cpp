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

namespace TrianglePicking
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

    Cursor::Cursor(Game& game, ContentManager& content)
        : DrawableGameComponent(game), content(&content)
    {
    }

    const std::string& Cursor::GetTypeName() const
    {
        static const std::string name = "TrianglePicking.Cursor";
        return name;
    }

    void Cursor::LoadContent()
    {
        cursorTexture = content->Load<Texture2D>("cursor");
        textureCenter = Vector2(static_cast<float>(cursorTexture->getWidthProperty() / 2),
                                static_cast<float>(cursorTexture->getHeightProperty() / 2));

        spriteBatch.emplace(getGraphicsDeviceProperty());

        DrawableGameComponent::LoadContent();
    }

    void Cursor::Draw(const GameTime& gameTime)
    {
        spriteBatch->Begin();

        // use textureCenter as the origin of the sprite, so that the cursor is
        // drawn centered around Position.
        spriteBatch->Draw(*cursorTexture, getPositionProperty(), std::nullopt, Color::White, 0.0f,
                          textureCenter, Vector2::One, SpriteEffects::None, 0.0f);

        spriteBatch->End();
        DrawableGameComponent::Draw(gameTime);
    }

    void Cursor::Update(GameTime& gameTime)
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
        if (currentState.getDPadProperty().getUpProperty() == ButtonState::Pressed)    delta.Y = -1;
        if (currentState.getDPadProperty().getDownProperty() == ButtonState::Pressed)  delta.Y = 1;
        if (currentState.getDPadProperty().getLeftProperty() == ButtonState::Pressed)  delta.X = -1;
        if (currentState.getDPadProperty().getRightProperty() == ButtonState::Pressed) delta.X = 1;

        // normalize delta so that we know the cursor can't move faster than CursorSpeed.
        if (delta != Vector2::Zero)
        {
            delta.Normalize();
        }

        // The original guards this block with `#else` against `#if XBOX360`: on Windows the
        // cursor tracks the mouse and the thumbstick pushes the OS pointer along with it. This
        // port is the Windows build, so it takes that branch.
        const MouseState mouseState = Mouse::GetState();
        position.X = static_cast<float>(mouseState.getXProperty());
        position.Y = static_cast<float>(mouseState.getYProperty());

        if (getGameProperty().getIsActiveProperty())
        {
            const Viewport vp = getGraphicsDeviceProperty().getViewportProperty();
            const float vpX = static_cast<float>(vp.getXProperty());
            const float vpY = static_cast<float>(vp.getYProperty());
            const float vpW = static_cast<float>(vp.getWidthProperty());
            const float vpH = static_cast<float>(vp.getHeightProperty());

            if ((vpX <= position.X) && (position.X <= (vpX + vpW))
                && (vpY <= position.Y) && (position.Y <= (vpY + vpH)))
            {
                position += delta * CursorSpeed
                          * static_cast<float>(
                                gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());
                position.X = MathHelper::Clamp(position.X, vpX, vpX + vpW);
                position.Y = MathHelper::Clamp(position.Y, vpY, vpY + vpH);
            }
            else if (delta.LengthSquared() > 0.0f)
            {
                position.X = vpX + vpW / 2;
                position.Y = vpY + vpH / 2;
            }

            Mouse::SetPosition(static_cast<int>(position.X), static_cast<int>(position.Y));
        }

        DrawableGameComponent::Update(gameTime);
    }

    Ray Cursor::CalculateCursorRay(const Matrix& projectionMatrix, const Matrix& viewMatrix)
    {
        // create 2 positions in screenspace using the cursor position. 0 is as
        // close as possible to the camera, 1 is as far away as possible.
        const Vector3 nearSource(getPositionProperty(), 0.0f);
        const Vector3 farSource(getPositionProperty(), 1.0f);

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
