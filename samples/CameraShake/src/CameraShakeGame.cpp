// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CameraShakeGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "CameraShakeGame.hpp"

#include "VibrationManager.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace CameraShake
{
    using namespace Microsoft::Xna::Framework::Input::Touch;
    using SharpRuntime::String;

    CameraShakeGame::CameraShakeGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

#if defined(WINDOWS_PHONE)
        graphics.setIsFullScreenProperty(true);
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
#endif

        // Add our VibrationManager to the game for handling controller vibration
        vibration = std::make_unique<VibrationManager>(*this);
        getComponentsProperty().Add(vibration.get());

        // We use Tap and DoubleTap on Windows Phone
        TouchPanel::setEnabledGesturesProperty(GestureType::Tap | GestureType::DoubleTap);
    }

    CameraShakeGame::~CameraShakeGame() = default;

    const std::string& CameraShakeGame::GetTypeName() const
    {
        static const std::string name = "CameraShake.CameraShakeGame";
        return name;
    }

    void CameraShakeGame::LoadContent()
    {
        // Create a new SpriteBatch, which can be used to draw textures.
        spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        font = getContentProperty().Load<SpriteFont>("Font");

        ground = getContentProperty().Load<Model>("Ground");
        tank = getContentProperty().Load<Model>("Tank");

        camera.Position = Vector3(1000.0f);
        camera.Projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(),
            10.0f, 10000.0f);
    }

    void CameraShakeGame::Update(GameTime& gameTime)
    {
        // Update our input state
        gamePadPrev = gamePad;
        keyboardPrev = keyboard;
        gamePad = GamePad::GetState(PlayerIndex::One);
        keyboard = Keyboard::GetState();

        // Allows the game to exit
        if (gamePad.getButtonsProperty().getBackProperty() == ButtonState::Pressed ||
            keyboard.IsKeyDown(Keys::Escape))
        {
            this->Exit();
        }

        bool shake = false;
        bool longShake = false;

        // Check for the A button/key for a short shake
        if ((gamePad.IsButtonDown(Buttons::A) && gamePadPrev.IsButtonUp(Buttons::A)) ||
            (keyboard.IsKeyDown(Keys::A) && keyboardPrev.IsKeyUp(Keys::A)))
        {
            shake = true;
        }

        // Check for the X button/key for a long shake
        if ((gamePad.IsButtonDown(Buttons::X) && gamePadPrev.IsButtonUp(Buttons::X)) ||
            (keyboard.IsKeyDown(Keys::X) && keyboardPrev.IsKeyUp(Keys::X)))
        {
            longShake = true;
        }

        // Read all gestures
        while (TouchPanel::getIsGestureAvailableProperty())
        {
            const GestureSample gesture = TouchPanel::ReadGesture();

            // Taps generate a short shake
            if (gesture.getGestureTypeProperty() == GestureType::Tap)
            {
                shake = true;
            }

            // Double taps generate a long shake
            else if (gesture.getGestureTypeProperty() == GestureType::DoubleTap)
            {
                longShake = true;
            }
        }

        // If we're performing a long shake, call the Shake method with a 2 second length
        if (longShake)
        {
            camera.Shake(25.0f, 2.0f);
            VibrationManager::Vibrate(PlayerIndex::One, .5f, .5f, 2.0f);
        }

        // If we're performing a short shake, call the Shake method with a .4 second length
        else if (shake)
        {
            camera.Shake(25.0f, .4f);
            VibrationManager::Vibrate(PlayerIndex::One, .5f, .5f, .4f);
        }

        // Update our camera
        camera.Update(gameTime);

        Game::Update(gameTime);
    }

    void CameraShakeGame::Draw(const GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

        // Set some render states for our 3D rendering
        getGraphicsDeviceProperty().setBlendStateProperty(BlendState::Opaque);
        getGraphicsDeviceProperty().setDepthStencilStateProperty(DepthStencilState::Default);
        getGraphicsDeviceProperty().getSamplerStatesProperty()[0] = SamplerState::LinearWrap;

        // Draw our ground and tank scene using our camera's View and Projection matrices
        ground->Draw(Matrix::CreateScale(.1f), camera.getViewProperty(), camera.Projection);
        tank->Draw(Matrix::getIdentityProperty(), camera.getViewProperty(), camera.Projection);

        // Draw our instruction text
        DrawInstructions();

        Game::Draw(gameTime);
    }

    void CameraShakeGame::DrawInstructions()
    {
        // Our instructions are based on our platform
        String instructions;
#if defined(WINDOWS_PHONE)
        instructions = "Tap - Short shake\nDouble tap - Long shake";
#else
        instructions = "A - Short shake\nX - Long shake";
#endif

        // Position our text based on the title safe area
        const Rectangle titleSafe =
            getGraphicsDeviceProperty().getViewportProperty().getTitleSafeAreaProperty();
        const Vector2 position((float)titleSafe.X, (float)titleSafe.Y);

        // Use SpriteBatch to draw our text
        spriteBatch->Begin();
        spriteBatch->DrawString(*font, instructions, position + Vector2::One, Color::Black);
        spriteBatch->DrawString(*font, instructions, position, Color::White);
        spriteBatch->End();
    }
}
