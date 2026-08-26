// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "DistortionSampleGame.hpp"

#include <cmath>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace DistortionSample
{
    using namespace Microsoft::Xna::Framework::Input;

    DistortionSampleGame::DistortionSampleGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        distortionComponent = std::make_unique<DistortionComponent>(*this);
        getComponentsProperty().Add(distortionComponent.get());
    }

    const std::string& DistortionSampleGame::GetTypeName() const
    {
        static const std::string name = "DistortionSample.DistortionSampleGame";
        return name;
    }

    void DistortionSampleGame::Initialize()
    {
        // The original allocates the three-element array here; in C++ the array is a member
        // and its elements already exist, so only the index is reset.
        currentDistorter = 0;

        Game::Initialize();
    }

    void DistortionSampleGame::LoadContent()
    {
        spriteBatch = std::make_unique<SpriteBatch>(*graphics.getGraphicsDeviceProperty());
        spriteFont = getContentProperty().Load<SpriteFont>("hudFont");
        background = getContentProperty().Load<Texture2D>("Sunset");

        distorters[0] = Distorter();
        distorters[0].ModelName = "Dude";
        distorters[0].World = Matrix::CreateTranslation(0, -40, 0) *
            Matrix::CreateScale(8);
        distorters[0].Model_ = getContentProperty().Load<Model>("Dude");
        distorters[0].Technique =
            DistortionComponent::DistortionTechnique::PullIn;
        distorters[0].DistortionScale = 0.0003f;
        distorters[0].DistortionBlur = true;

        distorters[1] = Distorter();
        distorters[1].ModelName = "Cylinder";
        distorters[1].World = Matrix::CreateScale(200);
        distorters[1].Model_ = getContentProperty().Load<Model>("Cylinder");
        distorters[1].Technique =
            DistortionComponent::DistortionTechnique::HeatHaze;
        distorters[1].DistortionScale = 0.025f;
        distorters[1].DistortionBlur = true;

        distorters[2] = Distorter();
        distorters[2].ModelName = "Window";
        distorters[2].World = Matrix::CreateScale(500);
        distorters[2].Model_ = getContentProperty().Load<Model>("Window");
        distorters[2].Technique =
            DistortionComponent::DistortionTechnique::DisplacementMapped;
        distorters[2].DistortionScale = 0.025f;
        distorters[2].DistortionBlur = false;

        const Viewport viewport = graphics.getGraphicsDeviceProperty()->getViewportProperty();
        overlayTextLocation = Vector2(
            (float)viewport.getXProperty() + (float)viewport.getWidthProperty() * 0.1f,
            (float)viewport.getYProperty() + (float)viewport.getHeightProperty() * 0.1f);
    }

    void DistortionSampleGame::Update(GameTime& gameTime)
    {
        HandleInput();

        // update the distortion component
        distortionComponent->Distorter_ = &distorters[(std::size_t)currentDistorter];

        Game::Update(gameTime);
    }

    void DistortionSampleGame::Draw(const GameTime& gameTime)
    {
        distortionComponent->BeginDraw();

        getGraphicsDeviceProperty().Clear(Color::Black);

        // Draw the background image.
        spriteBatch->Begin(SpriteSortMode::Deferred, BlendState::Opaque);
        spriteBatch->Draw(*background,
                          getGraphicsDeviceProperty().getViewportProperty().getBoundsProperty(),
                          Color::White);
        spriteBatch->End();

        // Draw other components (which includes the distortion).
        Game::Draw(gameTime);

        // Display some text over the top. Note how we draw this after distortion
        // because we don't want the text to be affected by the postprocessing.
        DrawOverlayText();
    }

    void DistortionSampleGame::DrawOverlayText()
    {
        const Distorter& current = distorters[(std::size_t)currentDistorter];
        const String text = current.ToString() + "\n\n" +
            "A: Cycle Distorter\n" +
            "X: " + (current.DistortionBlur ? "Disable" : "Enable") + " Distorter Blur\n" +
            "B: " + (distortionComponent->ShowDistortionMap ? "Hide" : "Show") +
            " Distortion Map\n";

        spriteBatch->Begin();

        // Draw the string twice to create a drop shadow, first colored black
        // and offset one pixel to the bottom right, then again in white at the
        // intended position. this makes text easier to read over the background.
        spriteBatch->DrawString(*spriteFont, text, overlayTextLocation + Vector2::One,
                                Color::Black);
        spriteBatch->DrawString(*spriteFont, text, overlayTextLocation, Color::White);

        spriteBatch->End();
    }

    void DistortionSampleGame::HandleInput()
    {
        lastKeyboardState = currentKeyboardState;
        lastGamePadState = currentGamePadState;

        currentKeyboardState = Keyboard::GetState();
        currentGamePadState = GamePad::GetState(PlayerIndex::One);

        // Check for exit.
        if (currentKeyboardState.IsKeyDown(Keys::Escape) ||
            currentGamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
        {
            Exit();
        }

        // Cycle mode
        if ((currentGamePadState.getButtonsProperty().getAProperty() == ButtonState::Pressed &&
             lastGamePadState.getButtonsProperty().getAProperty() != ButtonState::Pressed) ||
            (currentKeyboardState.IsKeyDown(Keys::Space) &&
             lastKeyboardState.IsKeyUp(Keys::Space)) ||
            (currentKeyboardState.IsKeyDown(Keys::A) &&
             lastKeyboardState.IsKeyUp(Keys::A)))
        {
            currentDistorter = (currentDistorter + 1) % (int)distorters.size();
            viewAngle = initialViewAngle;
        }

        // Toggle showing the distortion map on or off?
        if ((currentGamePadState.getButtonsProperty().getBProperty() == ButtonState::Pressed &&
             lastGamePadState.getButtonsProperty().getBProperty() != ButtonState::Pressed) ||
            (currentKeyboardState.IsKeyDown(Keys::Tab) &&
             lastKeyboardState.IsKeyUp(Keys::Tab)) ||
            (currentKeyboardState.IsKeyDown(Keys::B) &&
             lastKeyboardState.IsKeyUp(Keys::B)))
        {
            distortionComponent->ShowDistortionMap =
                !distortionComponent->ShowDistortionMap;
        }

        // Toggle showing the distortion map on or off?
        if ((currentGamePadState.getButtonsProperty().getXProperty() == ButtonState::Pressed &&
             lastGamePadState.getButtonsProperty().getXProperty() != ButtonState::Pressed) ||
            (currentKeyboardState.IsKeyDown(Keys::LeftControl) &&
             lastKeyboardState.IsKeyUp(Keys::LeftControl)) ||
            (currentKeyboardState.IsKeyDown(Keys::X) &&
             lastKeyboardState.IsKeyUp(Keys::X)))
        {
            distorters[(std::size_t)currentDistorter].DistortionBlur =
                !distorters[(std::size_t)currentDistorter].DistortionBlur;
        }

        // rotate the camera, using the left thumbstick and arrow keys
        float viewAngleChange = currentGamePadState.getThumbSticksProperty().getLeftProperty().X;
        if (currentKeyboardState.IsKeyDown(Keys::Left))
        {
            viewAngleChange = -1;
        }
        else if (currentKeyboardState.IsKeyDown(Keys::Right))
        {
            viewAngleChange = 1;
        }
        viewAngle += viewAngleChange * CameraRotationSpeed;
        distortionComponent->View = Matrix::CreateLookAt(
            ViewDistance * Vector3((float)std::cos(viewAngle), 0, (float)std::sin(viewAngle)),
            Vector3::Zero, Vector3::Up);
    }
}
