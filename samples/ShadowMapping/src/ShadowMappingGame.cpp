// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ShadowMapping.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "ShadowMappingGame.hpp"

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameterCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechniqueCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerStateCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/TextureCollection.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace ShadowMapping
{
    using namespace Microsoft::Xna::Framework::Input;

    ShadowMappingGame::ShadowMappingGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        graphics.setPreferredBackBufferWidthProperty(windowWidth);
        graphics.setPreferredBackBufferHeightProperty(windowHeight);

        float aspectRatio = (float)windowWidth / (float)windowHeight;

        projection = Matrix::CreatePerspectiveFieldOfView(MathHelper::PiOver4,
                                                          aspectRatio,
                                                          1.0f, 1000.0f);
    }

    const std::string& ShadowMappingGame::GetTypeName() const
    {
        static const std::string name = "ShadowMapping.ShadowMappingGame";
        return name;
    }

    void ShadowMappingGame::LoadContent()
    {
        spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());

        // Load the two models we will be using in the sample
        gridModel = getContentProperty().Load<Model>("grid");
        dudeModel = getContentProperty().Load<Model>("dude");

        // Create floating point render target
        shadowRenderTarget = std::make_unique<RenderTarget2D>(
            *graphics.getGraphicsDeviceProperty(),
            shadowMapWidthHeight,
            shadowMapWidthHeight,
            false,
            SurfaceFormat::Single,
            DepthFormat::Depth24);
    }

    void ShadowMappingGame::Update(GameTime& gameTime)
    {
        HandleInput(gameTime);

        UpdateCamera(gameTime);

        Game::Update(gameTime);
    }

    void ShadowMappingGame::Draw(const GameTime& gameTime)
    {
        // Update the lights ViewProjection matrix based on the
        // current camera frustum
        lightViewProjection = CreateLightViewProjectionMatrix();

        getGraphicsDeviceProperty().setBlendStateProperty(BlendState::Opaque);
        getGraphicsDeviceProperty().setDepthStencilStateProperty(DepthStencilState::Default);

        // Render the scene to the shadow map
        CreateShadowMap();

        // Draw the scene using the shadow map
        DrawWithShadowMap();

        // Display the shadow map to the screen
        DrawShadowMapToScreen();

        Game::Draw(gameTime);
    }

    Matrix ShadowMappingGame::CreateLightViewProjectionMatrix()
    {
        // Matrix with that will rotate in points the direction of the light
        Matrix lightRotation = Matrix::CreateLookAt(Vector3::Zero,
                                                    -lightDir,
                                                    Vector3::Up);

        // Get the corners of the frustum
        std::vector<Vector3> frustumCorners = cameraFrustum.GetCorners();

        // Transform the positions of the corners into the direction of the light
        for (std::size_t i = 0; i < frustumCorners.size(); i++)
        {
            frustumCorners[i] = Vector3::Transform(frustumCorners[i], lightRotation);
        }

        // Find the smallest box around the points
        BoundingBox lightBox = BoundingBox::CreateFromPoints(frustumCorners);

        Vector3 boxSize = lightBox.Max - lightBox.Min;
        Vector3 halfBoxSize = boxSize * 0.5f;

        // The position of the light should be in the center of the back
        // pannel of the box.
        Vector3 lightPosition = lightBox.Min + halfBoxSize;
        lightPosition.Z = lightBox.Min.Z;

        // We need the position back in world coordinates so we transform
        // the light position by the inverse of the lights rotation
        lightPosition = Vector3::Transform(lightPosition,
                                           Matrix::Invert(lightRotation));

        // Create the view matrix for the light
        Matrix lightView = Matrix::CreateLookAt(lightPosition,
                                                lightPosition - lightDir,
                                                Vector3::Up);

        // Create the projection matrix for the light
        // The projection is orthographic since we are using a directional light
        Matrix lightProjection = Matrix::CreateOrthographic(boxSize.X, boxSize.Y,
                                                            -boxSize.Z, boxSize.Z);

        return lightView * lightProjection;
    }

    void ShadowMappingGame::CreateShadowMap()
    {
        // Set our render target to our floating point render target
        getGraphicsDeviceProperty().SetRenderTarget(shadowRenderTarget.get());

        // Clear the render target to white or all 1's
        // We set the clear to white since that represents the
        // furthest the object could be away
        getGraphicsDeviceProperty().Clear(Color::White);

        // Draw any occluders in our case that is just the dude model

        // Set the models world matrix so it will rotate
        world = Matrix::CreateRotationY(MathHelper::ToRadians(rotateDude));
        // Draw the dude model
        DrawModel(*dudeModel, true);

        // Set render target back to the back buffer
        getGraphicsDeviceProperty().SetRenderTarget(nullptr);
    }

    void ShadowMappingGame::DrawWithShadowMap()
    {
        graphics.getGraphicsDeviceProperty()->Clear(Color::CornflowerBlue);

        getGraphicsDeviceProperty().getSamplerStatesProperty()[1] = SamplerState::PointClamp;

        // Draw the grid
        world = Matrix::getIdentityProperty();
        DrawModel(*gridModel, false);

        // Draw the dude model
        world = Matrix::CreateRotationY(MathHelper::ToRadians(rotateDude));
        DrawModel(*dudeModel, false);
    }

    void ShadowMappingGame::DrawModel(Model& model, bool createShadowMap)
    {
        const std::string techniqueName =
            createShadowMap ? "CreateShadowMap" : "DrawWithShadowMap";

        std::vector<Matrix> transforms(
            (std::size_t)model.getBonesProperty().getCountProperty());
        model.CopyAbsoluteBoneTransformsTo(transforms);

        // Loop over meshs in the model
        for (ModelMesh* mesh : model.getMeshesProperty())
        {
            // Loop over effects in the mesh
            for (Effect* effect : mesh->getEffectsProperty())
            {
                // Set the currest values for the effect
                effect->setCurrentTechniqueProperty(
                    effect->getTechniquesProperty()[techniqueName]);
                effect->getParametersProperty()["World"]->SetValue(world);
                effect->getParametersProperty()["View"]->SetValue(view);
                effect->getParametersProperty()["Projection"]->SetValue(projection);
                effect->getParametersProperty()["LightDirection"]->SetValue(lightDir);
                effect->getParametersProperty()["LightViewProj"]->SetValue(lightViewProjection);

                if (!createShadowMap)
                    effect->getParametersProperty()["ShadowMap"]->SetValue(
                        shadowRenderTarget.get());
            }
            // Draw the mesh
            mesh->Draw();
        }
    }

    void ShadowMappingGame::DrawShadowMapToScreen()
    {
        // XNA's `Begin(0, ...)` is SpriteSortMode.Deferred; CNA takes the sampler, depth and
        // rasterizer states by pointer, so the two nulls the original passes stay null here.
        SamplerState pointClamp = SamplerState::PointClamp;
        spriteBatch->Begin(SpriteSortMode::Deferred, BlendState::Opaque,
                           &pointClamp, nullptr, nullptr);
        spriteBatch->Draw(*shadowRenderTarget, Rectangle(0, 0, 128, 128), Color::White);
        spriteBatch->End();

        getGraphicsDeviceProperty().getTexturesProperty()(0, nullptr);
        getGraphicsDeviceProperty().getSamplerStatesProperty()[0] = SamplerState::LinearWrap;
    }

    void ShadowMappingGame::HandleInput(const GameTime& gameTime)
    {
        float time = (float)gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty();

        currentKeyboardState = Keyboard::GetState();
        currentGamePadState = GamePad::GetState(PlayerIndex::One);

        // Rotate the dude model
        rotateDude += currentGamePadState.getTriggersProperty().getRightProperty() * time * 0.2f;
        rotateDude -= currentGamePadState.getTriggersProperty().getLeftProperty() * time * 0.2f;

        if (currentKeyboardState.IsKeyDown(Keys::Q))
            rotateDude -= time * 0.2f;
        if (currentKeyboardState.IsKeyDown(Keys::E))
            rotateDude += time * 0.2f;

        // Check for exit.
        if (currentKeyboardState.IsKeyDown(Keys::Escape) ||
            currentGamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
        {
            Exit();
        }
    }

    void ShadowMappingGame::UpdateCamera(const GameTime& gameTime)
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

        Vector3 cameraRight = Vector3::Cross(Vector3::Up, cameraForward);
        Vector3 flatFront = Vector3::Cross(cameraRight, Vector3::Up);

        Matrix pitchMatrix = Matrix::CreateFromAxisAngle(cameraRight, pitch);
        Matrix turnMatrix = Matrix::CreateFromAxisAngle(Vector3::Up, turn);

        Vector3 tiltedFront = Vector3::TransformNormal(cameraForward, pitchMatrix *
                                                       turnMatrix);

        // Check angle so we cant flip over
        if (Vector3::Dot(tiltedFront, flatFront) > 0.001f)
        {
            cameraForward = Vector3::Normalize(tiltedFront);
        }

        // Check for input to move the camera around.
        if (currentKeyboardState.IsKeyDown(Keys::W))
            cameraPosition += cameraForward * time * 0.1f;

        if (currentKeyboardState.IsKeyDown(Keys::S))
            cameraPosition -= cameraForward * time * 0.1f;

        if (currentKeyboardState.IsKeyDown(Keys::A))
            cameraPosition += cameraRight * time * 0.1f;

        if (currentKeyboardState.IsKeyDown(Keys::D))
            cameraPosition -= cameraRight * time * 0.1f;

        cameraPosition += cameraForward *
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
            cameraForward = Vector3(0, 0, -1);
        }

        cameraForward.Normalize();

        // Create the new view matrix
        view = Matrix::CreateLookAt(cameraPosition,
                                    cameraPosition + cameraForward,
                                    Vector3::Up);

        // Set the new frustum value
        cameraFrustum.setMatrixProperty(view * projection);
    }
}
