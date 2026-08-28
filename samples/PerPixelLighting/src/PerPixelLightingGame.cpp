// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// PerPixelLighting.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "PerPixelLightingGame.hpp"

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPassCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameterCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechniqueCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/Single.hpp"

namespace PerPixelLightingSample
{
    using namespace Microsoft::Xna::Framework::Input;

    PerPixelLighting::PerPixelLighting()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");
    }

    const std::string& PerPixelLighting::GetTypeName() const
    {
        static const std::string name = "PerPixelLightingSample.PerPixelLighting";
        return name;
    }

    void PerPixelLighting::Initialize()
    {
        // create a default world and matrix
        world = Matrix::getIdentityProperty();

        // the mesh array is a fixed-size member here; XNA allocates it at this point

        // Set up the reference grid
        grid.setGridColorProperty(Color::LimeGreen);
        grid.setGridScaleProperty(1.0f);
        grid.setGridSizeProperty(32);
        // Set the grid to draw on the x/z plane around the origin
        grid.setWorldMatrixProperty(Matrix::getIdentityProperty());

        // set up the sample camera
        camera.emplace(SampleArcBallCameraMode::RollConstrained);
        camera->setDistanceProperty(3);
        // orbit the camera so we're looking down the z=-1 axis,
        // at the "front" of the object
        camera->OrbitRight(MathHelper::Pi);
        // orbit up a bit for perspective
        camera->OrbitUp(.2f);

        // set the initial effect, technique, and mesh
        activeMesh = 1;
        activeCombination = 0;
        activeEffect = effectTechniqueCombinations[activeCombination][0];
        activeTechnique = effectTechniqueCombinations[activeCombination][1];

        // set the initial specular values
        specularPower = 16;
        specularIntensity = 1;

        Game::Initialize();
    }

    void PerPixelLighting::LoadContent()
    {
        // Set up the reference grid and sample camera
        grid.LoadGraphicsContent(*graphics.getGraphicsDeviceProperty());

        // create the spritebatch for debug text
        spriteBatch = std::make_unique<SpriteBatch>(*graphics.getGraphicsDeviceProperty());

        // load meshes
        sampleMeshes[0] = getContentProperty().Load<Model>("Cube");
        sampleMeshes[1] = getContentProperty().Load<Model>("SphereHighPoly");
        sampleMeshes[2] = getContentProperty().Load<Model>("SphereLowPoly");
        sampleMeshes[3] = getContentProperty().Load<Model>("Cylinder");
        sampleMeshes[4] = getContentProperty().Load<Model>("Cone");

        // load the sprite font for debug text
        debugTextFont = getContentProperty().Load<SpriteFont>("DebugText");
        debugTextHeight = Vector2(0, (float)debugTextFont->getLineSpacingProperty() + 5);

        // load the effects
        effects[0] = getContentProperty().Load<std::shared_ptr<Effect>>("VertexLighting");
        effects[1] = getContentProperty().Load<std::shared_ptr<Effect>>("PerPixelLighting");

        for (int i = 0; i < 2; i++)
        {
            const std::size_t slot = static_cast<std::size_t>(i);
            EffectParameterCollection& parameters = effects[slot]->getParametersProperty();

            // cache the effect parameters
            worldParameter[slot] = parameters["world"];
            viewParameter[slot] = parameters["view"];
            projectionParameter[slot] = parameters["projection"];
            cameraPositionParameter[slot] = parameters["cameraPosition"];
            specularPowerParameter[slot] = parameters["specularPower"];
            specularIntensityParameter[slot] = parameters["specularIntensity"];
            cameraPositionParameter[slot] = parameters["cameraPosition"];

            //
            // set up some basic effect parameters that do not change during the
            // course of execution
            //

            // set the light colors
            parameters["ambientLightColor"]->SetValue(Color::DarkSlateGray.ToVector4());
            parameters["diffuseLightColor"]->SetValue(Color::CornflowerBlue.ToVector4());
            parameters["specularLightColor"]->SetValue(Color::White.ToVector4());

            // Set the light position to a fixed location.
            // This will place the light source behind, to the right, and above the
            // initial camera position.
            parameters["lightPosition"]->SetValue(Vector3(30.0f, 30.0f, 30.0f));
        }

        // Recalculate the projection properties on every LoadGraphicsContent call.
        // That way, if the window gets resized, then the perspective matrix will be
        // updated accordingly
        const Viewport& viewport = graphics.getGraphicsDeviceProperty()->getViewportProperty();
        float aspectRatio = (float)viewport.getWidthProperty() /
            (float)viewport.getHeightProperty();
        float fieldOfView = aspectRatio * MathHelper::PiOver4 * 3.0f / 4.0f;
        grid.setProjectionMatrixProperty(Matrix::CreatePerspectiveFieldOfView(
            fieldOfView, aspectRatio, .1f, 1000.0f));

        // calculate the safe left and top edges of the screen
        safeBounds = Vector2(
            (float)viewport.getXProperty() + (float)viewport.getWidthProperty() * 0.1f,
            (float)viewport.getYProperty() + (float)viewport.getHeightProperty() * 0.1f
            );
    }

    void PerPixelLighting::Update(GameTime& gameTime)
    {
        GamePadState gpState = GamePad::GetState(PlayerIndex::One);
        KeyboardState kbState = Keyboard::GetState();

        // Check for exit
        if ((gpState.getButtonsProperty().getBackProperty() == ButtonState::Pressed) ||
            kbState.IsKeyDown(Keys::Escape))
        {
            Exit();
        }

        // Handle inputs for the sample camera
        camera->HandleDefaultGamepadControls(gpState, gameTime);
        camera->HandleDefaultKeyboardControls(kbState, gameTime);

        // Handle inputs specific to this sample
        HandleInput(gameTime, gpState, kbState);

        // The built-in camera class provides the view matrix
        grid.setViewMatrixProperty(camera->getViewMatrixProperty());

        // The camera position should also be updated for the
        // Phong specular component to be meaningful
        cameraPositionParameter[static_cast<std::size_t>(activeEffect)]->SetValue(
            camera->getPositionProperty());

        // replace the "last" gamepad and keyboard states
        lastGpState = gpState;
        lastKbState = kbState;

        Game::Update(gameTime);
    }

    void PerPixelLighting::HandleInput(const GameTime& gameTime, const GamePadState& gpState,
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
            activeCombination = (activeCombination + 1) %
                effectTechniqueCombinationCount;
            activeEffect = effectTechniqueCombinations[activeCombination][0];
            activeTechnique = effectTechniqueCombinations[activeCombination][1];
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
            world *= Matrix::CreateFromAxisAngle(camera->getUpProperty(), elapsedTime * dx);
        }
        if (dy != 0)
        {
            world *= Matrix::CreateFromAxisAngle(camera->getRightProperty(), elapsedTime * -dy);
        }

        // handle specular power and intensity inputs
        float dPower = SampleArcBallCamera::ReadKeyboardAxis(kbState,
            Keys::Multiply, Keys::Divide);
        if (gpState.getDPadProperty().getRightProperty() == ButtonState::Pressed)
        {
            dPower = 1;
        }
        if (gpState.getDPadProperty().getLeftProperty() == ButtonState::Pressed)
        {
            dPower = -1;
        }

        float dIntensity = SampleArcBallCamera::ReadKeyboardAxis(kbState,
            Keys::Add, Keys::Subtract);
        if (gpState.getDPadProperty().getUpProperty() == ButtonState::Pressed)
        {
            dIntensity = 1;
        }
        if (gpState.getDPadProperty().getDownProperty() == ButtonState::Pressed)
        {
            dIntensity = -1;
        }

        if (dPower != 0)
        {
            specularPower *= 1 + (elapsedTime * dPower);
            specularPower = MathHelper::Clamp(specularPower,
                specularPowerMinimum, specularPowerMaximum);
        }

        if (dIntensity != 0)
        {
            specularIntensity *= 1 + (elapsedTime * dIntensity);
            specularIntensity = MathHelper::Clamp(specularIntensity,
                specularIntensityMinimum, specularIntensityMaximum);
        }
    }

    void PerPixelLighting::SetSharedEffectParameters()
    {
        const std::size_t slot = static_cast<std::size_t>(activeEffect);
        worldParameter[slot]->SetValue(world);
        viewParameter[slot]->SetValue(grid.getViewMatrixProperty());
        projectionParameter[slot]->SetValue(grid.getProjectionMatrixProperty());
        specularPowerParameter[slot]->SetValue(specularPower);
        specularIntensityParameter[slot]->SetValue(specularIntensity);
    }

    void PerPixelLighting::Draw(const GameTime& gameTime)
    {
        graphics.getGraphicsDeviceProperty()->Clear(Color::Black);

        // the SpriteBatch added below to draw the debug text is changing some
        // needed render states, so they are reset here.
        graphics.getGraphicsDeviceProperty()->setDepthStencilStateProperty(
            DepthStencilState::Default);

        // draw the reference grid so it's easier to get our bearings
        grid.Draw();

        // always set the shared effects parameters
        SetSharedEffectParameters();

        // draw the mesh itself
        DrawSampleMesh(sampleMeshes[static_cast<std::size_t>(activeMesh)]
                           ? &*sampleMeshes[static_cast<std::size_t>(activeMesh)]
                           : nullptr);

        // draw the technique name and specular settings
        spriteBatch->Begin();
        spriteBatch->DrawString(*debugTextFont,
            effects[static_cast<std::size_t>(activeEffect)]
                ->getCurrentTechniqueProperty()->getNameProperty(),
            safeBounds, Color::White);
        spriteBatch->DrawString(*debugTextFont, "Specular Power: " +
            System::Single::ToString(specularPower, "0.00"),
            safeBounds + (1.0f * debugTextHeight), Color::White);
        spriteBatch->DrawString(*debugTextFont, "Specular Intensity: " +
            System::Single::ToString(specularIntensity, "0.00"),
            safeBounds + (2.0f * debugTextHeight), Color::White);
        spriteBatch->End();

        Game::Draw(gameTime);
    }

    void PerPixelLighting::DrawSampleMesh(Model* sampleMesh)
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

        const std::size_t slot = static_cast<std::size_t>(activeEffect);

        // determine the current effect and technique
        effects[slot]->setCurrentTechniqueProperty(
            &effects[slot]->getTechniquesProperty()[activeTechnique]);

        // now we loop through the passes in the teqnique, drawing each
        // one in order
        EffectPassCollection& passes =
            effects[slot]->getCurrentTechniqueProperty()->getPassesProperty();
        int passCount = passes.getCountProperty();
        for (int i = 0; i < passCount; i++)
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
