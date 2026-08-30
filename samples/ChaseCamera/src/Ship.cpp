// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Ship.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Ship.hpp"

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "System/Math.hpp"

namespace ChaseCameraSample
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Keyboard;
    using Microsoft::Xna::Framework::Input::Keys;
    using Microsoft::Xna::Framework::Input::Mouse;

    Ship::Ship(GraphicsDevice& device)
        : graphicsDevice(device)
    {
        Reset();
    }

    Vector3 Ship::getRightProperty() const
    {
        return right;
    }

    Matrix Ship::getWorldProperty() const
    {
        return world;
    }

    void Ship::Reset()
    {
        Position = Vector3(0.0f, MinimumAltitude, 0.0f);
        Direction = Vector3::Forward;
        Up = Vector3::Up;
        right = Vector3::Right;
        Velocity = Vector3::Zero;
    }

    bool Ship::TouchLeft() const
    {
        const auto mouseState = Mouse::GetState();
        return mouseState.getLeftButtonProperty() == ButtonState::Pressed
            && mouseState.getXProperty()
                <= graphicsDevice.getViewportProperty().getWidthProperty() / 3;
    }

    bool Ship::TouchRight() const
    {
        const auto mouseState = Mouse::GetState();
        return mouseState.getLeftButtonProperty() == ButtonState::Pressed
            && mouseState.getXProperty()
                >= 2 * graphicsDevice.getViewportProperty().getWidthProperty() / 3;
    }

    bool Ship::TouchDown() const
    {
        const auto mouseState = Mouse::GetState();
        return mouseState.getLeftButtonProperty() == ButtonState::Pressed
            && mouseState.getYProperty()
                <= graphicsDevice.getViewportProperty().getHeightProperty() / 3;
    }

    bool Ship::TouchUp() const
    {
        const auto mouseState = Mouse::GetState();
        return mouseState.getLeftButtonProperty() == ButtonState::Pressed
            && mouseState.getYProperty()
                >= 2 * graphicsDevice.getViewportProperty().getHeightProperty() / 3;
    }

    void Ship::Update(const GameTime& gameTime)
    {
        const auto keyboardState = Keyboard::GetState();
        const auto gamePadState = GamePad::GetState(PlayerIndex::One);
        const auto mouseState = Mouse::GetState();

        const Single elapsed = static_cast<Single>(
            gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty());

        Vector2 rotationAmount = -gamePadState.getThumbSticksProperty().getLeftProperty();
        if (keyboardState.IsKeyDown(Keys::Left) || TouchLeft())
        {
            rotationAmount.X = 1.0f;
        }
        if (keyboardState.IsKeyDown(Keys::Right) || TouchRight())
        {
            rotationAmount.X = -1.0f;
        }
        if (keyboardState.IsKeyDown(Keys::Up) || TouchUp())
        {
            rotationAmount.Y = -1.0f;
        }
        if (keyboardState.IsKeyDown(Keys::Down) || TouchDown())
        {
            rotationAmount.Y = 1.0f;
        }

        rotationAmount = rotationAmount * RotationRate * elapsed;

        if (Up.Y < 0.0f)
        {
            rotationAmount.X = -rotationAmount.X;
        }

        const Matrix rotationMatrix = Matrix::CreateFromAxisAngle(
            getRightProperty(), rotationAmount.Y) * Matrix::CreateRotationY(rotationAmount.X);

        Direction = Vector3::TransformNormal(Direction, rotationMatrix);
        Up = Vector3::TransformNormal(Up, rotationMatrix);

        Direction.Normalize();
        Up.Normalize();

        right = Vector3::Cross(Direction, Up);
        Up = Vector3::Cross(getRightProperty(), Direction);

        Single thrustAmount = gamePadState.getTriggersProperty().getRightProperty();
        if (keyboardState.IsKeyDown(Keys::Space)
            || mouseState.getLeftButtonProperty() == ButtonState::Pressed)
        {
            thrustAmount = 1.0f;
        }

        const Vector3 force = Direction * thrustAmount * ThrustForce;

        const Vector3 acceleration = force / Mass;
        Velocity += acceleration * elapsed;

        Velocity *= DragFactor;

        Position += Velocity * elapsed;

        Position.Y = System::Math::Max(Position.Y, MinimumAltitude);

        world = Matrix::getIdentityProperty();
        world.setForwardProperty(Direction);
        world.setUpProperty(Up);
        world.setRightProperty(right);
        world.setTranslationProperty(Position);
    }
}
