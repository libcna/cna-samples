// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "LensFlareGame.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DirectionalLight.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace LensFlare
{
    LensFlareGame::LensFlareGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        // Create and add the lensflare component.
        lensFlare = std::make_unique<LensFlareComponent>(*this);

        getComponentsProperty().Add(lensFlare.get());
    }

    const std::string& LensFlareGame::GetTypeName() const
    {
        static const std::string typeName{"LensFlare.LensFlareGame"};
        return typeName;
    }

    void LensFlareGame::LoadContent()
    {
        terrain.emplace(getContentProperty().Load<Model>("terrain"));
    }

    void LensFlareGame::Update(GameTime& gameTime)
    {
        HandleInput();

        UpdateCamera(gameTime);

        Game::Update(gameTime);
    }

    void LensFlareGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

        device.Clear(Color::CornflowerBlue);

        // Compute camera matrices.
        const Matrix view = Matrix::CreateLookAt(cameraPosition,
                                                 cameraPosition + cameraFront,
                                                 Vector3::Up);

        const float aspectRatio = device.getViewportProperty().getAspectRatioProperty();

        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(MathHelper::PiOver4,
                                                                       aspectRatio,
                                                                       0.1f, 500);

        // Draw the terrain.
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        for (ModelMesh* mesh : terrain->getMeshesProperty())
        {
            for (Effect* meshEffect : mesh->getEffectsProperty())
            {
                BasicEffect* effect = static_cast<BasicEffect*>(meshEffect);

                effect->setWorldProperty(Matrix::getIdentityProperty());
                effect->setViewProperty(view);
                effect->setProjectionProperty(projection);

                effect->setLightingEnabledProperty(true);
                effect->setDiffuseColorProperty(Vector3(1.0f));
                effect->setAmbientLightColorProperty(Vector3(0.5f));

                effect->getDirectionalLight0Property().setEnabledProperty(true);
                effect->getDirectionalLight0Property().setDiffuseColorProperty(Vector3::One);
                effect->getDirectionalLight0Property().setDirectionProperty(
                    lensFlare->LightDirection);

                effect->setFogEnabledProperty(true);
                effect->setFogStartProperty(200);
                effect->setFogEndProperty(500);
                effect->setFogColorProperty(Color::CornflowerBlue.ToVector3());
            }

            mesh->Draw();
        }

        // Tell the lensflare component where our camera is positioned.
        lensFlare->View = view;
        lensFlare->Projection = projection;

        Game::Draw(gameTime);
    }

    void LensFlareGame::HandleInput()
    {
        currentKeyboardState = Input::Keyboard::GetState();
        currentGamePadState = Input::GamePad::GetState(PlayerIndex::One);

        // Check for exit.
        if (currentKeyboardState.IsKeyDown(Input::Keys::Escape) ||
            currentGamePadState.getButtonsProperty().getBackProperty() ==
                Input::ButtonState::Pressed)
        {
            Exit();
        }
    }

    void LensFlareGame::UpdateCamera(const GameTime& gameTime)
    {
        const float time =
            (float)gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty();

        // Check for input to rotate the camera.
        float pitch = -currentGamePadState.getThumbSticksProperty().getRightProperty().Y *
                      time * 0.001f;
        float turn = -currentGamePadState.getThumbSticksProperty().getRightProperty().X *
                     time * 0.001f;

        if (currentKeyboardState.IsKeyDown(Input::Keys::Up))
            pitch += time * 0.001f;

        if (currentKeyboardState.IsKeyDown(Input::Keys::Down))
            pitch -= time * 0.001f;

        if (currentKeyboardState.IsKeyDown(Input::Keys::Left))
            turn += time * 0.001f;

        if (currentKeyboardState.IsKeyDown(Input::Keys::Right))
            turn -= time * 0.001f;

        const Vector3 cameraRight = Vector3::Cross(Vector3::Up, cameraFront);
        const Vector3 flatFront = Vector3::Cross(cameraRight, Vector3::Up);

        const Matrix pitchMatrix = Matrix::CreateFromAxisAngle(cameraRight, pitch);
        const Matrix turnMatrix = Matrix::CreateFromAxisAngle(Vector3::Up, turn);

        const Vector3 tiltedFront =
            Vector3::TransformNormal(cameraFront, pitchMatrix * turnMatrix);

        // Check angle so we can't flip over.
        if (Vector3::Dot(tiltedFront, flatFront) > 0.001f)
        {
            cameraFront = Vector3::Normalize(tiltedFront);
        }

        // Check for input to move the camera around.
        if (currentKeyboardState.IsKeyDown(Input::Keys::W))
            cameraPosition += cameraFront * time * 0.1f;

        if (currentKeyboardState.IsKeyDown(Input::Keys::S))
            cameraPosition -= cameraFront * time * 0.1f;

        if (currentKeyboardState.IsKeyDown(Input::Keys::A))
            cameraPosition += cameraRight * time * 0.1f;

        if (currentKeyboardState.IsKeyDown(Input::Keys::D))
            cameraPosition -= cameraRight * time * 0.1f;

        cameraPosition += cameraFront *
                          currentGamePadState.getThumbSticksProperty().getLeftProperty().Y *
                          time * 0.1f;

        cameraPosition -= cameraRight *
                          currentGamePadState.getThumbSticksProperty().getLeftProperty().X *
                          time * 0.1f;

        if (currentGamePadState.getButtonsProperty().getRightStickProperty() ==
                Input::ButtonState::Pressed ||
            currentKeyboardState.IsKeyDown(Input::Keys::R))
        {
            cameraPosition = Vector3(-200, 30, 30);
            cameraFront = Vector3(1, 0, 0);
        }
    }
}
