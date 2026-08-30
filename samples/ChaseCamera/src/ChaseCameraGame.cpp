// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ChaseCameraGame.hpp"

#include <cstddef>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DisplayOrientation.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "System/InvalidCastException.hpp"
#include "System/TimeSpan.hpp"

namespace ChaseCameraSample
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::DisplayOrientation;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::BasicEffect;
    using Microsoft::Xna::Framework::Graphics::BlendState;
    using Microsoft::Xna::Framework::Graphics::DepthStencilState;
    using Microsoft::Xna::Framework::Graphics::Effect;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Graphics::Model;
    using Microsoft::Xna::Framework::Graphics::ModelMesh;
    using Microsoft::Xna::Framework::Graphics::SamplerState;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteFont;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Keyboard;
    using Microsoft::Xna::Framework::Input::KeyboardState;
    using Microsoft::Xna::Framework::Input::Keys;
    using Microsoft::Xna::Framework::Input::Mouse;

    ChaseCameraGame::ChaseCameraGame()
        : graphics(this)
    {
        graphics.setSupportedOrientationsProperty(DisplayOrientation::Portrait);

        getContentProperty().setRootDirectoryProperty("Content");
        setIsMouseVisibleProperty(true);

#if defined(WINDOWS_PHONE)
        graphics.setPreferredBackBufferWidthProperty(480);
        graphics.setPreferredBackBufferHeightProperty(800);

        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));

        graphics.setIsFullScreenProperty(true);
#else
        graphics.setPreferredBackBufferWidthProperty(853);
        graphics.setPreferredBackBufferHeightProperty(480);
#endif

        camera = std::make_unique<ChaseCamera>();

        camera->setDesiredPositionOffsetProperty(Vector3(0.0f, 2000.0f, 3500.0f));
        camera->setLookAtOffsetProperty(Vector3(0.0f, 150.0f, 0.0f));

        camera->setNearPlaneDistanceProperty(10.0f);
        camera->setFarPlaneDistanceProperty(100000.0f);
    }

    const std::string& ChaseCameraGame::GetTypeName() const
    {
        static const std::string name = "ChaseCameraSample.ChaseCameraGame";
        return name;
    }

    void ChaseCameraGame::Initialize()
    {
        Game::Initialize();

        ship = std::make_unique<Ship>(getGraphicsDeviceProperty());

        const auto viewport = graphics.getGraphicsDeviceProperty()->getViewportProperty();
        camera->setAspectRatioProperty(
            static_cast<Single>(viewport.getWidthProperty())
            / static_cast<Single>(viewport.getHeightProperty()));

        UpdateCameraChaseTarget();
        camera->Reset();
    }

    void ChaseCameraGame::LoadContent()
    {
        spriteBatch.emplace(*graphics.getGraphicsDeviceProperty());
        spriteFont.emplace(getContentProperty().Load<SpriteFont>("gameFont"));

        shipModel.emplace(getContentProperty().Load<Model>("Ship"));
        groundModel.emplace(getContentProperty().Load<Model>("Ground"));
    }

    void ChaseCameraGame::Update(GameTime& gameTime)
    {
        lastKeyboardState = currentKeyboardState;
        lastGamePadState = currentGamePadState;
        lastMousState = currentMouseState;

#if defined(WINDOWS_PHONE)
        currentKeyboardState = KeyboardState();
#else
        currentKeyboardState = Keyboard::GetState();
#endif
        currentGamePadState = GamePad::GetState(PlayerIndex::One);
        currentMouseState = Mouse::GetState();

        if (currentKeyboardState.IsKeyDown(Keys::Escape)
            || currentGamePadState.getButtonsProperty().getBackProperty()
                == ButtonState::Pressed)
        {
            Exit();
        }

        const auto viewport = getGraphicsDeviceProperty().getViewportProperty();
        const bool touchTopLeft =
            currentMouseState.getLeftButtonProperty() == ButtonState::Pressed
            && lastMousState.getLeftButtonProperty() != ButtonState::Pressed
            && currentMouseState.getXProperty() < viewport.getWidthProperty() / 10
            && currentMouseState.getYProperty() < viewport.getHeightProperty() / 10;

        if ((lastKeyboardState.IsKeyUp(Keys::A)
                && currentKeyboardState.IsKeyDown(Keys::A))
            || (lastGamePadState.getButtonsProperty().getAProperty() == ButtonState::Released
                && currentGamePadState.getButtonsProperty().getAProperty()
                    == ButtonState::Pressed)
            || touchTopLeft)
        {
            cameraSpringEnabled = !cameraSpringEnabled;
        }

        if (currentKeyboardState.IsKeyDown(Keys::R)
            || currentGamePadState.getButtonsProperty().getRightStickProperty()
                == ButtonState::Pressed)
        {
            ship->Reset();
            camera->Reset();
        }

        ship->Update(gameTime);

        UpdateCameraChaseTarget();

        if (cameraSpringEnabled)
        {
            camera->Update(gameTime);
        }
        else
        {
            camera->Reset();
        }

        Game::Update(gameTime);
    }

    void ChaseCameraGame::UpdateCameraChaseTarget()
    {
        camera->setChasePositionProperty(ship->Position);
        camera->setChaseDirectionProperty(ship->Direction);
        camera->setUpProperty(ship->Up);
    }

    void ChaseCameraGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

        device.Clear(Color::CornflowerBlue);

        device.setBlendStateProperty(BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.getSamplerStatesProperty()[0] = SamplerState::LinearWrap;

        DrawModel(*shipModel, ship->getWorldProperty());
        DrawModel(*groundModel, Matrix::getIdentityProperty());

        DrawOverlayText();

        Game::Draw(gameTime);
    }

    void ChaseCameraGame::DrawModel(Model& model, const Matrix& world)
    {
        std::vector<Matrix> transforms(
            static_cast<std::size_t>(model.getBonesProperty().getCountProperty()));
        model.CopyAbsoluteBoneTransformsTo(transforms);

        for (ModelMesh* mesh : model.getMeshesProperty())
        {
            for (Effect* meshEffect : mesh->getEffectsProperty())
            {
                auto* effect = dynamic_cast<BasicEffect*>(meshEffect);
                if (effect == nullptr)
                {
                    throw System::InvalidCastException(
                        "ChaseCamera: a mesh effect is not a BasicEffect.");
                }

                effect->EnableDefaultLighting();
                effect->setWorldProperty(
                    transforms[static_cast<std::size_t>(
                        mesh->getParentBoneProperty()->getIndexProperty())] * world);
                effect->setViewProperty(camera->getViewProperty());
                effect->setProjectionProperty(camera->getProjectionProperty());
            }

            mesh->Draw();
        }
    }

    void ChaseCameraGame::DrawOverlayText()
    {
        spriteBatch->Begin();

        const std::string text =
            "-Touch, Right Trigger, or Spacebar = thrust\n"
            "-Screen edges, Left Thumb Stick,\n  or Arrow keys = steer\n"
            "-Press A or touch the top left corner\n  to toggle camera spring ("
            + std::string(cameraSpringEnabled ? "on" : "off") + ")";

        spriteBatch->DrawString(*spriteFont, text, Vector2(65.0f, 65.0f), Color::Black);
        spriteBatch->DrawString(*spriteFont, text, Vector2(64.0f, 64.0f), Color::White);

        spriteBatch->End();
    }
}
