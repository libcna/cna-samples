// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// VertexLighting.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "VertexLightingGame.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPassCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameterCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace VertexLightingSample
{
    using namespace Microsoft::Xna::Framework::Input;

    VertexLighting::VertexLighting()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");
    }

    const std::string& VertexLighting::GetTypeName() const
    {
        static const std::string name = "VertexLightingSample.VertexLighting";
        return name;
    }

    void VertexLighting::Initialize()
    {
        Game::Initialize();
    }

    void VertexLighting::LoadContent()
    {
        // Set up the reference grid and sample camera
        grid.emplace();
        grid->setGridColorProperty(Color::LimeGreen);
        grid->setGridScaleProperty(1.0f);
        grid->setGridSizeProperty(32);
        grid->LoadGraphicsContent(*graphics.getGraphicsDeviceProperty());

        camera.emplace(SampleArcBallCameraMode::RollConstrained);
        camera->setDistanceProperty(3);
        // orbit the camera so we're looking down the z=-1 axis
        // the acr-ball camera is traditionally oriented to look
        // at the "front" of an object
        camera->OrbitRight(MathHelper::Pi);
        // orbit up a bit for perspective
        camera->OrbitUp(.2f);

        // load meshes
        sampleMeshes[0] = getContentProperty().Load<Model>("Cube");
        sampleMeshes[1] = getContentProperty().Load<Model>("SphereHighPoly");
        sampleMeshes[2] = getContentProperty().Load<Model>("SphereLowPoly");
        sampleMeshes[3] = getContentProperty().Load<Model>("Cylinder");
        sampleMeshes[4] = getContentProperty().Load<Model>("Cone");

        // Example 1.2
        // create the effect objects that correspond to the effect files
        // that have been imported via the Content Pipeline
        noLightingEffect = getContentProperty().Load<std::shared_ptr<Effect>>("FlatShaded");
        vertexLightingEffect = getContentProperty().Load<std::shared_ptr<Effect>>("VertexLighting");

        GetEffectParameters();

        // Calculate the projection properties first on any
        // load callback.  That way if the window gets resized,
        // the perspective matrix is updated accordingly
        const Viewport& viewport = graphics.getGraphicsDeviceProperty()->getViewportProperty();
        float aspectRatio = (float)viewport.getWidthProperty() /
            (float)viewport.getHeightProperty();
        float fov = MathHelper::PiOver4 * aspectRatio * 3 / 4;
        projection = Matrix::CreatePerspectiveFieldOfView(fov, aspectRatio, .1f, 1000.0f);

        // create a default world matrix
        world = Matrix::getIdentityProperty();

        // grid requires a projection matrix to draw correctly
        grid->setProjectionMatrixProperty(projection);

        // Set the grid to draw on the x/z plane around the origin
        grid->setWorldMatrixProperty(Matrix::getIdentityProperty());
    }

    void VertexLighting::GetEffectParameters()
    {
        // These parameters are used by both vertexLightingEffect and
        // noLightingEffect, so we must take care to look up the correct ones.
        if (enableAdvancedEffect)
        {
            worldParameter = vertexLightingEffect->getParametersProperty()["world"];
            viewParameter = vertexLightingEffect->getParametersProperty()["view"];
            projectionParameter = vertexLightingEffect->getParametersProperty()["projection"];
        }
        else
        {
            worldParameter = noLightingEffect->getParametersProperty()["world"];
            viewParameter = noLightingEffect->getParametersProperty()["view"];
            projectionParameter = noLightingEffect->getParametersProperty()["projection"];
        }

        // These effect parameters are only used by vertexLightingEffect
        // to indicate the lights' colors and direction
        lightColorParameter = vertexLightingEffect->getParametersProperty()["lightColor"];
        lightDirectionParameter = vertexLightingEffect->getParametersProperty()["lightDirection"];
        ambientColorParameter = vertexLightingEffect->getParametersProperty()["ambientColor"];
    }

    void VertexLighting::Update(GameTime& gameTime)
    {
        GamePadState gpState = GamePad::GetState(PlayerIndex::One);
        KeyboardState kbState = Keyboard::GetState();

        // check for exit
        if ((gpState.getButtonsProperty().getBackProperty() == ButtonState::Pressed) ||
            kbState.IsKeyDown(Keys::Escape))
        {
            Exit();
        }

        // Handle inputs for the sample camera
        camera->HandleDefaultGamepadControls(gpState, gameTime);
        camera->HandleDefaultKeyboardControls(kbState, gameTime);

        // handle inputs specific to this sample
        HandleInput(gameTime, gpState, kbState);

        // Set the light direction to a fixed value.
        // This will place the light source behind, to the right, and above the user.
        diffuseLightDirection = Vector3(-1, -1, -1);

        // ensure the light direction is normalized, or
        // the shader will give some weird results
        diffuseLightDirection.Normalize();

        // set the color of the diffuse light
        diffuseLightColor = Color::CornflowerBlue.ToVector4();

        // set the ambient lighting color
        ambientLightColor = Color::DarkSlateGray.ToVector4();

        // The built-in camera class provides the view matrix
        view = camera->getViewMatrixProperty();

        // additionally, the reference grid included in the sample
        // requires a view matrix to draw correctly
        grid->setViewMatrixProperty(camera->getViewMatrixProperty());

        lastGpState = gpState;
        lastKbState = kbState;
        Game::Update(gameTime);
    }

    void VertexLighting::HandleInput(const GameTime& gameTime, const GamePadState& gpState,
                                     const KeyboardState& kbState)
    {
        float elapsedTime = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

        // Handle input for selecting meshes
        if (((gpState.getButtonsProperty().getXProperty() == ButtonState::Pressed) &&
            (lastGpState.getButtonsProperty().getXProperty() == ButtonState::Released)) ||
            (kbState.IsKeyDown(Keys::Tab) && lastKbState.IsKeyUp(Keys::Tab)))
        {
            // switch the active mesh
            activeMesh = (activeMesh + 1) % (int)sampleMeshes.size();
        }

        // Handle input for selecting the active effect
        if (((gpState.getButtonsProperty().getYProperty() == ButtonState::Pressed) &&
            (lastGpState.getButtonsProperty().getYProperty() == ButtonState::Released)) ||
            (kbState.IsKeyDown(Keys::Space) && lastKbState.IsKeyUp(Keys::Space)))
        {
            // toggle the advanced effect
            enableAdvancedEffect = !enableAdvancedEffect;
            GetEffectParameters();
        }

        // handle mesh rotation inputs
        float dx =
            SampleArcBallCamera::ReadKeyboardAxis(kbState, Keys::Left, Keys::Right) +
            gpState.getThumbSticksProperty().getLeftProperty().X;
        float dy =
            SampleArcBallCamera::ReadKeyboardAxis(kbState, Keys::Down, Keys::Up) +
            gpState.getThumbSticksProperty().getLeftProperty().Y;

        // apply mesh rotation to world matrix
        if (dx != 0)
        {
            world = world * Matrix::CreateFromAxisAngle(camera->getUpProperty(),
                elapsedTime * dx);
        }
        if (dy != 0)
        {
            world = world * Matrix::CreateFromAxisAngle(camera->getRightProperty(),
                elapsedTime * -dy);
        }
    }

    void VertexLighting::SetSharedEffectParameters()
    {
        projectionParameter->SetValue(projection);
        viewParameter->SetValue(view);
        worldParameter->SetValue(world);
    }

    void VertexLighting::Draw(const GameTime& gameTime)
    {
        graphics.getGraphicsDeviceProperty()->Clear(Color::Black);

        // draw the reference grid so it's easier to get our bearings
        grid->Draw();

        // always set the shared effects parameters
        SetSharedEffectParameters();

        if (enableAdvancedEffect)
        {
            // Example 1.5
            // Since we're using the advanced effect, we'll be setting the effect
            // parameters for the lighting effect.
            ambientColorParameter->SetValue(ambientLightColor);
            lightColorParameter->SetValue(diffuseLightColor);
            lightDirectionParameter->SetValue(diffuseLightDirection);
        }

        // finally, draw the mesh itself
        DrawSampleMesh(sampleMeshes[static_cast<std::size_t>(activeMesh)]
                           ? &*sampleMeshes[static_cast<std::size_t>(activeMesh)]
                           : nullptr);

        Game::Draw(gameTime);
    }

    void VertexLighting::DrawSampleMesh(Model* sampleMesh)
    {
        if (sampleMesh == nullptr)
            return;

        // our sample meshes only contain a single part, so we don't need to bother
        // looping over the ModelMesh and ModelMeshPart collections. If the meshes
        // were more complex, we would repeat all the following code for each part
        ModelMesh* mesh = sampleMesh->getMeshesProperty()[0];
        ModelMeshPart* meshPart = mesh->getMeshPartsProperty()[0];

        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

        // set the vertex source to the mesh's vertex buffer
        device.SetVertexBuffer(meshPart->getVertexBufferProperty(),
                               meshPart->getVertexOffsetProperty());

        // set the current index buffer to the sample mesh's index buffer
        device.setIndicesProperty(meshPart->getIndexBufferProperty());

        // figure out which effect we're using currently
        Effect* effect;
        if (enableAdvancedEffect) effect = vertexLightingEffect.get();
        else effect = noLightingEffect.get();

        // at this point' we're ready to begin drawing

        // now we loop through the passes in the teqnique, drawing each
        // one in order
        EffectPassCollection& passes = effect->getCurrentTechniqueProperty()->getPassesProperty();
        for (int i = 0; i < passes.getCountProperty(); i++)
        {
            // EffectPass.Apply will update the device to
            // begin using the state information defined in the current pass
            passes[i].Apply();

            // sampleMesh contains all of the information required to draw
            // the current mesh
            device.DrawIndexedPrimitives(
                PrimitiveType::TriangleList, 0, 0,
                meshPart->getNumVerticesProperty(), meshPart->getStartIndexProperty(),
                meshPart->getPrimitiveCountProperty());
        }
    }
}
