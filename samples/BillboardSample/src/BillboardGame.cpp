// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Billboard.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "BillboardGame.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DirectionalLight.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameterCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerStateCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/InvalidCastException.hpp"

namespace Billboard
{
    using namespace Microsoft::Xna::Framework::Input;

    BillboardGame::BillboardGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");
    }

    const std::string& BillboardGame::GetTypeName() const
    {
        static const std::string name = "Billboard.BillboardGame";
        return name;
    }

    void BillboardGame::LoadContent()
    {
        landscape = getContentProperty().Load<Model>("landscape");
    }

    void BillboardGame::Update(GameTime& gameTime)
    {
        HandleInput();

        UpdateCamera(gameTime);

        Game::Update(gameTime);
    }

    void BillboardGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

        device.Clear(Color::CornflowerBlue);

        // Compute camera matrices.
        Matrix view = Matrix::CreateLookAt(cameraPosition,
                                           cameraPosition + cameraFront,
                                           Vector3::Up);

        Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            device.getViewportProperty().getAspectRatioProperty(),
            1, 10000);

        Vector3 lightDirection = Vector3::Normalize(Vector3(3, -1, 1));
        Vector3 lightColor = Vector3(0.3f, 0.4f, 0.2f);

        // Time is scaled down to make things wave in the wind more slowly.
        float time = (float)gameTime.getTotalGameTimeProperty().getTotalSecondsProperty() * 0.333f;

        // First we draw the ground geometry using BasicEffect.
        for (ModelMesh* mesh : landscape->getMeshesProperty())
        {
            if (mesh->getNameProperty() != "Billboards")
            {
                for (Effect* meshEffect : mesh->getEffectsProperty())
                {
                    // VegetationProcessor leaves every non-billboard mesh on the ModelProcessor's
                    // own BasicEffect material, which is what the original's typed foreach relies
                    // on; a C# cast-per-element loop throws on a mismatch, so this does too.
                    auto* effect = dynamic_cast<BasicEffect*>(meshEffect);
                    if (effect == nullptr)
                        throw System::InvalidCastException(
                            "Billboard: a ground mesh effect is not a BasicEffect.");

                    effect->setViewProperty(view);
                    effect->setProjectionProperty(projection);

                    effect->setLightingEnabledProperty(true);

                    effect->getDirectionalLight0Property().setEnabledProperty(true);
                    effect->getDirectionalLight0Property().setDirectionProperty(lightDirection);
                    effect->getDirectionalLight0Property().setDiffuseColorProperty(lightColor);

                    effect->setAmbientLightColorProperty(Vector3(0.1f, 0.2f, 0.1f));
                }

                device.setBlendStateProperty(BlendState::Opaque);
                device.setDepthStencilStateProperty(DepthStencilState::Default);
                device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);

                mesh->Draw();
            }
        }

        // Then we use a two-pass technique to render alpha blended billboards with
        // almost-correct depth sorting. The only way to make blending truly proper for
        // alpha objects is to draw everything in sorted order, but manually sorting all
        // our billboards would be very expensive. Instead, we draw in two passes.
        //
        // The first pass has alpha blending turned off, alpha testing set to only accept
        // ~95% or more opaque pixels, and the depth buffer turned on. Because this is only
        // rendering the solid parts of each billboard, the depth buffer works as
        // normal to give correct sorting, but obviously only part of each billboard will
        // be rendered.
        //
        // Then in the second pass we enable alpha blending, set alpha test to only accept
        // pixels with fractional alpha values, and set the depth buffer to test against
        // the existing data but not to write new depth values. This means the translucent
        // areas of each billboard will be sorted correctly against the depth buffer
        // information that was previously written while drawing the opaque parts, although
        // there can still be sorting errors between the translucent areas of different
        // billboards.
        //
        // In practice, sorting errors between translucent pixels tend not to be too
        // noticable as long as the opaque pixels are sorted correctly, so this technique
        // often looks ok, and is much faster than trying to sort everything 100%
        // correctly. It is particularly effective for organic textures like grass and
        // trees.
        for (ModelMesh* mesh : landscape->getMeshesProperty())
        {
            if (mesh->getNameProperty() == "Billboards")
            {
                // First pass renders opaque pixels.
                for (Effect* effect : mesh->getEffectsProperty())
                {
                    effect->getParametersProperty()["View"]->SetValue(view);
                    effect->getParametersProperty()["Projection"]->SetValue(projection);
                    effect->getParametersProperty()["LightDirection"]->SetValue(lightDirection);
                    effect->getParametersProperty()["WindTime"]->SetValue(time);
                    effect->getParametersProperty()["AlphaTestDirection"]->SetValue(1.0f);
                }

                device.setBlendStateProperty(BlendState::Opaque);
                device.setDepthStencilStateProperty(DepthStencilState::Default);
                device.setRasterizerStateProperty(RasterizerState::CullNone);
                device.getSamplerStatesProperty()[0] = SamplerState::LinearClamp;

                mesh->Draw();

                // Second pass renders the alpha blended fringe pixels.
                for (Effect* effect : mesh->getEffectsProperty())
                {
                    effect->getParametersProperty()["AlphaTestDirection"]->SetValue(-1.0f);
                }

                device.setBlendStateProperty(BlendState::NonPremultiplied);
                device.setDepthStencilStateProperty(DepthStencilState::DepthRead);

                mesh->Draw();
            }
        }

        Game::Draw(gameTime);
    }

    void BillboardGame::HandleInput()
    {
        currentKeyboardState = Keyboard::GetState();
        currentGamePadState = GamePad::GetState(PlayerIndex::One);

        // Check for exit.
        if (currentKeyboardState.IsKeyDown(Keys::Escape) ||
            currentGamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
        {
            Exit();
        }
    }

    void BillboardGame::UpdateCamera(const GameTime& gameTime)
    {
        float time = (float)gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty();

        // Check for input to rotate the camera.
        float pitch = -currentGamePadState.getThumbSticksProperty().getRightProperty().Y *
                      time * 0.001f;
        float turn = -currentGamePadState.getThumbSticksProperty().getRightProperty().X *
                     time * 0.001f;

        if (currentKeyboardState.IsKeyDown(Keys::Up))
            pitch += time * 0.001f;

        if (currentKeyboardState.IsKeyDown(Keys::Down))
            pitch -= time * 0.001f;

        if (currentKeyboardState.IsKeyDown(Keys::Left))
            turn += time * 0.001f;

        if (currentKeyboardState.IsKeyDown(Keys::Right))
            turn -= time * 0.001f;

        Vector3 cameraRight = Vector3::Cross(Vector3::Up, cameraFront);
        Vector3 flatFront = Vector3::Cross(cameraRight, Vector3::Up);

        Matrix pitchMatrix = Matrix::CreateFromAxisAngle(cameraRight, pitch);
        Matrix turnMatrix = Matrix::CreateFromAxisAngle(Vector3::Up, turn);

        Vector3 tiltedFront = Vector3::TransformNormal(cameraFront, pitchMatrix *
                                                        turnMatrix);

        // Check angle so we cant flip over
        if (Vector3::Dot(tiltedFront, flatFront) > 0.001f)
        {
            cameraFront = Vector3::Normalize(tiltedFront);
        }

        // Check for input to move the camera around.
        if (currentKeyboardState.IsKeyDown(Keys::W))
            cameraPosition += cameraFront * time * 0.1f;

        if (currentKeyboardState.IsKeyDown(Keys::S))
            cameraPosition -= cameraFront * time * 0.1f;

        if (currentKeyboardState.IsKeyDown(Keys::A))
            cameraPosition += cameraRight * time * 0.1f;

        if (currentKeyboardState.IsKeyDown(Keys::D))
            cameraPosition -= cameraRight * time * 0.1f;

        cameraPosition += cameraFront *
                          currentGamePadState.getThumbSticksProperty().getLeftProperty().Y *
                          time * 0.1f;

        cameraPosition -= cameraRight *
                          currentGamePadState.getThumbSticksProperty().getLeftProperty().X *
                          time * 0.1f;

        if (currentGamePadState.getButtonsProperty().getRightStickProperty() ==
                ButtonState::Pressed ||
            currentKeyboardState.IsKeyDown(Keys::R))
        {
            cameraPosition = Vector3(0, 50, 50);
            cameraFront = Vector3(0, 0, -1);
        }
    }
}
