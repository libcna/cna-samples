// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Audio3DGame.hpp"

#include "Cat.hpp"
#include "Dog.hpp"
#include "QuadDrawer.hpp"
#include "SpriteEntity.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace Audio3D
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Game;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::BlendState;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Graphics::Texture2D;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Keyboard;
    using Microsoft::Xna::Framework::Input::Keys;
    using SharpRuntime::Single;

    Audio3DGame::Audio3DGame()
        : graphics(this),
          audioManager(std::make_unique<AudioManager>(*this)),
          cat(std::make_unique<Cat>()),
          dog(std::make_unique<Dog>())
    {
        getContentProperty().setRootDirectoryProperty("Content");
        getComponentsProperty().Add(audioManager.get());
    }

    Audio3DGame::~Audio3DGame() = default;

    void Audio3DGame::LoadContent()
    {
        catTexture.emplace(getContentProperty().Load<Texture2D>("CatTexture"));
        dogTexture.emplace(getContentProperty().Load<Texture2D>("DogTexture"));

        cat->setTextureProperty(&*catTexture);
        dog->setTextureProperty(&*dogTexture);

        checkerTexture.emplace(getContentProperty().Load<Texture2D>("checker"));
        quadDrawer = std::make_unique<QuadDrawer>(*graphics.getGraphicsDeviceProperty());
    }

    void Audio3DGame::Update(GameTime& gameTime)
    {
        HandleInput();
        UpdateCamera();

        auto& listener = audioManager->getListenerProperty();
        listener.setPositionProperty(cameraPosition);
        listener.setForwardProperty(cameraForward);
        listener.setUpProperty(cameraUp);
        listener.setVelocityProperty(cameraVelocity);

        cat->Update(gameTime, *audioManager);
        dog->Update(gameTime, *audioManager);

        Game::Update(gameTime);
    }

    void Audio3DGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice* device = graphics.getGraphicsDeviceProperty();
        device->Clear(Color::CornflowerBlue);
        device->setBlendStateProperty(BlendState::AlphaBlend);

        const Matrix view = Matrix::CreateLookAt(
            cameraPosition,
            cameraPosition + cameraForward,
            cameraUp);

        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            1.0f,
            device->getViewportProperty().getAspectRatioProperty(),
            1.0f,
            100000.0f);

        const Matrix groundTransform = Matrix::CreateScale(20000.0f)
            * Matrix::CreateRotationX(MathHelper::PiOver2);

        quadDrawer->DrawQuad(*checkerTexture, 32.0f, groundTransform, view, projection);
        cat->Draw(*quadDrawer, cameraPosition, view, projection);
        dog->Draw(*quadDrawer, cameraPosition, view, projection);

        Game::Draw(gameTime);
    }

    void Audio3DGame::HandleInput()
    {
        currentKeyboardState = Keyboard::GetState();
        currentGamePadState = GamePad::GetState(PlayerIndex::One);

        if (currentKeyboardState.IsKeyDown(Keys::Escape)
            || currentGamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
        {
            Exit();
        }
    }

    void Audio3DGame::UpdateCamera()
    {
        constexpr Single turnSpeed = 0.05f;
        constexpr Single accelerationSpeed = 4.0f;
        constexpr Single frictionAmount = 0.98f;

        Single turn = -currentGamePadState.getThumbSticksProperty().getLeftProperty().X
            * turnSpeed;

        if (currentKeyboardState.IsKeyDown(Keys::Left))
        {
            turn += turnSpeed;
        }

        if (currentKeyboardState.IsKeyDown(Keys::Right))
        {
            turn -= turnSpeed;
        }

        cameraForward = Vector3::TransformNormal(
            cameraForward,
            Matrix::CreateRotationY(turn));

        Single accel = currentGamePadState.getThumbSticksProperty().getLeftProperty().Y
            * accelerationSpeed;

        if (currentKeyboardState.IsKeyDown(Keys::Up))
        {
            accel += accelerationSpeed;
        }

        if (currentKeyboardState.IsKeyDown(Keys::Down))
        {
            accel -= accelerationSpeed;
        }

        cameraVelocity += cameraForward * accel;
        cameraPosition += cameraVelocity;
        cameraVelocity *= frictionAmount;
    }

    const std::string& Audio3DGame::GetTypeName() const
    {
        static const std::string name = "Audio3D.Audio3DGame";
        return name;
    }
}
