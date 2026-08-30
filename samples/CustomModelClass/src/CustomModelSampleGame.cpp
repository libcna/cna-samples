// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CustomModelSampleGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CustomModelSampleGame.hpp"

#include "CustomModel.hpp"
#include "CustomModelContentReaders.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace CustomModelSample
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Keyboard;
    using Microsoft::Xna::Framework::Input::Keys;

    CustomModelSampleGame::CustomModelSampleGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        // CNAEXT -- XNA discovers the two reflective readers from the game assembly. C++ has no
        // runtime reflection, so the game declares the same field lists once. See diff.md.
        CNAEXT CustomModelContentReaderRegistrationEXT::RegisterEXT();
    }

    const std::string& CustomModelSampleGame::GetTypeName() const
    {
        static const std::string name = "CustomModelSample.CustomModelSampleGame";
        return name;
    }

    void CustomModelSampleGame::LoadContent()
    {
        model = getContentProperty().Load<std::shared_ptr<CustomModel>>("tank");

        view = Matrix::CreateLookAt(
            Vector3(1000.0f, 500.0f, 0.0f), Vector3(0.0f, 150.0f, 0.0f), Vector3::Up);

        projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(),
            10.0f, 10000.0f);
    }

    void CustomModelSampleGame::Update(GameTime& gameTime)
    {
        HandleInput();

        const float time =
            static_cast<float>(gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());
        world = Matrix::CreateRotationY(time * 0.1f);

        Game::Update(gameTime);
    }

    void CustomModelSampleGame::Draw(const GameTime& gameTime)
    {
        graphics.getGraphicsDeviceProperty()->Clear(Color::CornflowerBlue);
        model->Draw(world, view, projection);
        Game::Draw(gameTime);
    }

    void CustomModelSampleGame::HandleInput()
    {
        const auto currentKeyboardState = Keyboard::GetState();
        const auto currentGamePadState = GamePad::GetState(PlayerIndex::One);

        if (currentKeyboardState.IsKeyDown(Keys::Escape)
            || currentGamePadState.getButtonsProperty().getBackProperty()
                == ButtonState::Pressed)
        {
            Exit();
        }
    }
}
