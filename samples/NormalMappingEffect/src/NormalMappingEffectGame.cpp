// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// NormalMappingEffect.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "NormalMappingEffectGame.hpp"

#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameterCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace NormalMappingEffect
{
    using namespace Microsoft::Xna::Framework::Input;

    NormalMappingEffectGame::NormalMappingEffectGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");
    }

    const std::string& NormalMappingEffectGame::GetTypeName() const
    {
        static const std::string name = "NormalMappingEffect.NormalMappingEffectGame";
        return name;
    }

    void NormalMappingEffectGame::LoadContent()
    {
        model = getContentProperty().Load<Model>("lizard");
        for (ModelMesh* mesh : model->getMeshesProperty())
        {
            for (Effect* effect : mesh->getEffectsProperty())
            {
                effect->getParametersProperty()["LightColor"]->SetValue(lightColor);
                effect->getParametersProperty()["AmbientLightColor"]->SetValue(
                    ambientLightColor);

                effect->getParametersProperty()["Shininess"]->SetValue(shininess);
                effect->getParametersProperty()["SpecularPower"]->SetValue(specularPower);
            }
        }
    }

    void NormalMappingEffectGame::Update(GameTime& gameTime)
    {
        HandleInput();

        UpdateCamera(gameTime);

        // Turn on the rotating light
        if ((currentGamePadState.getButtonsProperty().getAProperty() == ButtonState::Pressed &&
            lastGamePadState.getButtonsProperty().getAProperty() != ButtonState::Pressed) ||
            (currentKeyboardState.IsKeyUp(Keys::Space) &&
            lastKeyboardState.IsKeyDown(Keys::Space)))
        {
            rotateLight = !rotateLight;
        }

        Game::Update(gameTime);
    }

    void NormalMappingEffectGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

        device.Clear(Color::CornflowerBlue);

        // Compute camera matrices.
        Matrix unrotatedView = Matrix::CreateLookAt(
            Vector3(0, 0, -cameraDistance), Vector3::Zero, Vector3::Up);

        Matrix view = Matrix::CreateRotationY(MathHelper::ToRadians(cameraRotation)) *
                      Matrix::CreateRotationX(MathHelper::ToRadians(cameraArc)) *
                      unrotatedView;


        Matrix projection = Matrix::CreatePerspectiveFieldOfView(MathHelper::PiOver4,
                                                                 device.getViewportProperty()
                                                                     .getAspectRatioProperty(),
                                                                 1,
                                                                 10000);

        if (rotateLight)
        {
            lightRotation +=
                (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() *
                LightRotationSpeed;
        }

        Matrix lightRotationMatrix = Matrix::CreateRotationY(lightRotation);
        Vector3 lightPosition = Vector3(LightRotationRadius, LightHeight, 0);
        lightPosition = Vector3::Transform(lightPosition, lightRotationMatrix);

        // Draw the model.
        std::vector<Matrix> transforms(
            (std::size_t)model->getBonesProperty().getCountProperty());

        model->CopyAbsoluteBoneTransformsTo(transforms);

        for (ModelMesh* mesh : model->getMeshesProperty())
        {
            for (Effect* effect : mesh->getEffectsProperty())
            {
                Matrix world =
                    transforms[(std::size_t)mesh->getParentBoneProperty()->getIndexProperty()];

                effect->getParametersProperty()["World"]->SetValue(world);
                effect->getParametersProperty()["View"]->SetValue(view);
                effect->getParametersProperty()["Projection"]->SetValue(projection);
                effect->getParametersProperty()["LightPosition"]->SetValue(lightPosition);
            }

            mesh->Draw();
        }

        Game::Draw(gameTime);
    }

    void NormalMappingEffectGame::HandleInput()
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
    }

    void NormalMappingEffectGame::UpdateCamera(const GameTime& gameTime)
    {
        float time = (float)gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty();

        // Check for input to rotate the camera up and down around the model.
        if (currentKeyboardState.IsKeyDown(Keys::Up) ||
            currentKeyboardState.IsKeyDown(Keys::W))
        {
            cameraArc += time * 0.1f;
        }

        if (currentKeyboardState.IsKeyDown(Keys::Down) ||
            currentKeyboardState.IsKeyDown(Keys::S))
        {
            cameraArc -= time * 0.1f;
        }

        cameraArc += currentGamePadState.getThumbSticksProperty().getRightProperty().Y *
                     time * 0.25f;

        // Limit the arc movement.
        if (cameraArc > 90.0f)
            cameraArc = 90.0f;
        else if (cameraArc < -90.0f)
            cameraArc = -90.0f;

        // Check for input to rotate the camera around the model.
        if (currentKeyboardState.IsKeyDown(Keys::Right) ||
            currentKeyboardState.IsKeyDown(Keys::D))
        {
            cameraRotation += time * 0.1f;
        }

        if (currentKeyboardState.IsKeyDown(Keys::Left) ||
            currentKeyboardState.IsKeyDown(Keys::A))
        {
            cameraRotation -= time * 0.1f;
        }

        cameraRotation += currentGamePadState.getThumbSticksProperty().getRightProperty().X *
                          time * 0.25f;

        // Check for input to zoom camera in and out.
        if (currentKeyboardState.IsKeyDown(Keys::Z))
            cameraDistance += time * 0.5f;

        if (currentKeyboardState.IsKeyDown(Keys::X))
            cameraDistance -= time * 0.5f;

        cameraDistance += currentGamePadState.getTriggersProperty().getLeftProperty() * time;
        cameraDistance -= currentGamePadState.getTriggersProperty().getRightProperty() * time;

        // Limit the camera distance.
        if (cameraDistance > 5000.0f)
            cameraDistance = 5000.0f;
        else if (cameraDistance < 350.0f)
            cameraDistance = 350.0f;

        if (currentGamePadState.getButtonsProperty().getRightStickProperty() ==
                ButtonState::Pressed ||
            currentKeyboardState.IsKeyDown(Keys::R))
        {
            cameraArc = 0;
            cameraRotation = 45;
            cameraDistance = 1500;
        }
    }
}
