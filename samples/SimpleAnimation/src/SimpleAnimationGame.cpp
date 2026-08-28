// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SimpleAnimation.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "SimpleAnimationGame.hpp"

#include <cmath>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace SimpleAnimation
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::GamePadState;
    using Microsoft::Xna::Framework::Input::Keyboard;
    using Microsoft::Xna::Framework::Input::KeyboardState;
    using Microsoft::Xna::Framework::Input::Keys;

    SimpleAnimationGame::SimpleAnimationGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");
    }

    const std::string& SimpleAnimationGame::GetTypeName() const
    {
        static const std::string name = "SimpleAnimation.SimpleAnimationGame";
        return name;
    }

    void SimpleAnimationGame::LoadContent()
    {
        tank.Load(getContentProperty());
    }

    void SimpleAnimationGame::Update(GameTime& gameTime)
    {
        HandleInput();

        const float time =
            static_cast<float>(gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());

        // Update the animation properties on the tank object. In a real game
        // you would probably take this data from user inputs or the physics
        // system, rather than just making everything rotate like this!

        tank.setWheelRotationProperty(time * 5);
        tank.setSteerRotationProperty(std::sin(time * 0.75f) * 0.5f);
        tank.setTurretRotationProperty(std::sin(time * 0.333f) * 1.25f);
        tank.setCannonRotationProperty(std::sin(time * 0.25f) * 0.333f - 0.333f);
        tank.setHatchRotationProperty(MathHelper::Clamp(std::sin(time * 2) * 2, -1.0f, 0.0f));

        Game::Update(gameTime);
    }

    void SimpleAnimationGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

        device.Clear(Color::DarkGray);

        // Calculate the camera matrices.
        const float time =
            static_cast<float>(gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());

        const Matrix rotation = Matrix::CreateRotationY(time * 0.1f);

        const Matrix view = Matrix::CreateLookAt(Vector3(1000, 500, 0),
                                                 Vector3(0, 150, 0),
                                                 Vector3::Up);

        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            device.getViewportProperty().getAspectRatioProperty(),
            10,
            10000);

        // Draw the tank model.
        tank.Draw(rotation, view, projection);

        Game::Draw(gameTime);
    }

    void SimpleAnimationGame::HandleInput()
    {
        const KeyboardState currentKeyboardState = Keyboard::GetState();
        const GamePadState currentGamePadState = GamePad::GetState(PlayerIndex::One);

        // Check for exit.
        if (currentKeyboardState.IsKeyDown(Keys::Escape) ||
            currentGamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
        {
            Exit();
        }
    }
}

/**
 * @brief The main entry point for the application.
 *
 * The original declares this as a `static class Program` at the bottom of SimpleAnimation.cs,
 * with no platform guard around it.
 *
 * @return The process exit code.
 */
int main()
{
    SimpleAnimation::SimpleAnimationGame game;
    game.Run();
    return 0;
}
