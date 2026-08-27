// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// InstancedModelSampleGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "InstancedModelSampleGame.hpp"

#include <algorithm>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBufferBinding.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElement.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementUsage.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace InstancedModelSample
{
    const VertexDeclaration InstancedModelSampleGame::instanceVertexDeclaration
    {
        VertexElement(0,  VertexElementFormat::Vector4, VertexElementUsage::BlendWeight, 0),
        VertexElement(16, VertexElementFormat::Vector4, VertexElementUsage::BlendWeight, 1),
        VertexElement(32, VertexElementFormat::Vector4, VertexElementUsage::BlendWeight, 2),
        VertexElement(48, VertexElementFormat::Vector4, VertexElementUsage::BlendWeight, 3)
    };

    namespace
    {
        /// The C# enum's ToString(), which the overlay prints verbatim.
        const char* TechniqueName(InstancingTechnique technique)
        {
            switch (technique)
            {
                case InstancingTechnique::HardwareInstancing: return "HardwareInstancing";
                case InstancingTechnique::NoInstancing: return "NoInstancing";
                case InstancingTechnique::NoInstancingOrStateBatching:
                    return "NoInstancingOrStateBatching";
            }
            return "";
        }
    }

    InstancedModelSampleGame::InstancedModelSampleGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        // Most games will want to leave both these values set to true to ensure
        // smoother updates, but when you are doing performance work it can be
        // useful to set them to false in order to get more accurate measurements.
        setIsFixedTimeStepProperty(false);

        graphics.setSynchronizeWithVerticalRetraceProperty(false);

        // Initialize the list of instances.
        instances.reserve(InitialInstanceCount);

        for (int i = 0; i < InitialInstanceCount; i++)
            instances.emplace_back();
    }

    const std::string& InstancedModelSampleGame::GetTypeName() const
    {
        static const std::string typeName{"InstancedModelSample.InstancedModelSampleGame"};
        return typeName;
    }

    void InstancedModelSampleGame::LoadContent()
    {
        spriteBatch.emplace(*graphics.getGraphicsDeviceProperty());

        spriteFont.emplace(getContentProperty().Load<SpriteFont>("Font"));

        instancedModel.emplace(getContentProperty().Load<Model>("Cats"));
        instancedModelBones.resize(instancedModel->getBonesProperty().getCountProperty());
        instancedModel->CopyAbsoluteBoneTransformsTo(instancedModelBones);
    }

    void InstancedModelSampleGame::Update(GameTime& gameTime)
    {
        HandleInput();

        // Update the position of each spinning instance.
        for (SpinningInstance& instance : instances)
        {
            instance.Update(gameTime);
        }

        // Measure our framerate.
        elapsedTime += gameTime.getElapsedGameTimeProperty();

        if (elapsedTime > System::TimeSpan::FromSeconds(1))
        {
            elapsedTime -= System::TimeSpan::FromSeconds(1);
            frameRate = frameCounter;
            frameCounter = 0;
        }

        Game::Update(gameTime);
    }

    void InstancedModelSampleGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

        device.Clear(Color::CornflowerBlue);

        // Calculate camera matrices.
        const Matrix view = Matrix::CreateLookAt(Vector3(0, 0, 15),
                                                 Vector3::Zero, Vector3::Up);

        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            device.getViewportProperty().getAspectRatioProperty(),
            1,
            100);

        // Set renderstates for drawing 3D models.
        device.setBlendStateProperty(BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::Default);

        // Gather instance transform matrices into a single array.
        instanceTransforms.resize(instances.size());

        for (std::size_t i = 0; i < instances.size(); i++)
        {
            instanceTransforms[i] = instances[i].getTransformProperty();
        }

        // Draw all the instances, using the currently selected rendering technique.
        switch (instancingTechnique)
        {
            case InstancingTechnique::HardwareInstancing:
                DrawModelHardwareInstancing(*instancedModel, instancedModelBones,
                                            instanceTransforms, view, projection);
                break;

            case InstancingTechnique::NoInstancing:
                DrawModelNoInstancing(*instancedModel, instancedModelBones,
                                      instanceTransforms, view, projection);
                break;

            case InstancingTechnique::NoInstancingOrStateBatching:
                DrawModelNoInstancingOrStateBatching(*instancedModel, instancedModelBones,
                                                     instanceTransforms, view, projection);
                break;
        }

        DrawOverlayText();

        // Measure our framerate.
        frameCounter++;

        Game::Draw(gameTime);
    }

    void InstancedModelSampleGame::DrawModelHardwareInstancing(
        Model& model, const std::vector<Matrix>& modelBones,
        const std::vector<Matrix>& instances, const Matrix& view, const Matrix& projection)
    {
        if (instances.empty())
            return;

        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

        // If we have more instances than room in our vertex buffer, grow it to the neccessary size.
        if ((instanceVertexBuffer == nullptr) ||
            ((int)instances.size() > instanceVertexBuffer->getVertexCountProperty()))
        {
            if (instanceVertexBuffer != nullptr)
                instanceVertexBuffer->Dispose();

            instanceVertexBuffer = std::make_unique<DynamicVertexBuffer>(
                device, instanceVertexDeclaration, (int)instances.size(),
                BufferUsage::WriteOnly);
        }

        // Transfer the latest instance transform matrices into the instanceVertexBuffer.
        instanceVertexBuffer->SetData(instances.data(), 0, (int)instances.size(),
                                      SetDataOptions::Discard);

        for (ModelMesh* mesh : model.getMeshesProperty())
        {
            for (ModelMeshPart* meshPart : mesh->getMeshPartsProperty())
            {
                // Tell the GPU to read from both the model vertex buffer plus our instanceVertexBuffer.
                device.SetVertexBuffers({
                    VertexBufferBinding(meshPart->getVertexBufferProperty(),
                                        meshPart->getVertexOffsetProperty(), 0),
                    VertexBufferBinding(instanceVertexBuffer.get(), 0, 1)
                });

                device.setIndicesProperty(meshPart->getIndexBufferProperty());

                // Set up the instance rendering effect.
                Effect* effect = meshPart->getEffectProperty();

                effect->setCurrentTechniqueProperty(
                    effect->getTechniquesProperty()["HardwareInstancing"]);

                effect->getParametersProperty()["World"]->SetValue(
                    modelBones[mesh->getParentBoneProperty()->getIndexProperty()]);
                effect->getParametersProperty()["View"]->SetValue(view);
                effect->getParametersProperty()["Projection"]->SetValue(projection);

                // Draw all the instance copies in a single call.
                for (EffectPass& pass : effect->getCurrentTechniqueProperty()->getPassesProperty())
                {
                    pass.Apply();

                    device.DrawInstancedPrimitives(PrimitiveType::TriangleList, 0, 0,
                                                   meshPart->getNumVerticesProperty(),
                                                   meshPart->getStartIndexProperty(),
                                                   meshPart->getPrimitiveCountProperty(),
                                                   (int)instances.size());
                }
            }
        }
    }

    void InstancedModelSampleGame::DrawModelNoInstancing(
        Model& model, const std::vector<Matrix>& modelBones,
        const std::vector<Matrix>& instances, const Matrix& view, const Matrix& projection)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

        for (ModelMesh* mesh : model.getMeshesProperty())
        {
            for (ModelMeshPart* meshPart : mesh->getMeshPartsProperty())
            {
                device.SetVertexBuffer(meshPart->getVertexBufferProperty(),
                                       meshPart->getVertexOffsetProperty());
                device.setIndicesProperty(meshPart->getIndexBufferProperty());

                // Set up the rendering effect.
                Effect* effect = meshPart->getEffectProperty();

                effect->setCurrentTechniqueProperty(
                    effect->getTechniquesProperty()["NoInstancing"]);

                effect->getParametersProperty()["View"]->SetValue(view);
                effect->getParametersProperty()["Projection"]->SetValue(projection);

                EffectParameter* transformParameter = effect->getParametersProperty()["World"];

                // Draw a single instance copy each time around this loop.
                for (std::size_t i = 0; i < instances.size(); i++)
                {
                    transformParameter->SetValue(
                        modelBones[mesh->getParentBoneProperty()->getIndexProperty()] *
                        instances[i]);

                    for (EffectPass& pass :
                         effect->getCurrentTechniqueProperty()->getPassesProperty())
                    {
                        pass.Apply();

                        device.DrawIndexedPrimitives(PrimitiveType::TriangleList, 0, 0,
                                                     meshPart->getNumVerticesProperty(),
                                                     meshPart->getStartIndexProperty(),
                                                     meshPart->getPrimitiveCountProperty());
                    }
                }
            }
        }
    }

    void InstancedModelSampleGame::DrawModelNoInstancingOrStateBatching(
        Model& model, const std::vector<Matrix>& modelBones,
        const std::vector<Matrix>& instances, const Matrix& view, const Matrix& projection)
    {
        for (std::size_t i = 0; i < instances.size(); i++)
        {
            for (ModelMesh* mesh : model.getMeshesProperty())
            {
                for (Effect* effect : mesh->getEffectsProperty())
                {
                    effect->setCurrentTechniqueProperty(
                        effect->getTechniquesProperty()["NoInstancing"]);

                    effect->getParametersProperty()["World"]->SetValue(
                        modelBones[mesh->getParentBoneProperty()->getIndexProperty()] *
                        instances[i]);
                    effect->getParametersProperty()["View"]->SetValue(view);
                    effect->getParametersProperty()["Projection"]->SetValue(projection);
                }

                mesh->Draw();
            }
        }
    }

    void InstancedModelSampleGame::DrawOverlayText()
    {
        const std::string text =
            "Frames per second: " + std::to_string(frameRate) + "\n" +
            "Instances: " + std::to_string(instances.size()) + "\n" +
            "Technique: " + TechniqueName(instancingTechnique) + "\n\n" +
            "A = Change technique\n" +
            "X = Add instances\n" +
            "Y = Remove instances\n";

        spriteBatch->Begin();

        spriteBatch->DrawString(*spriteFont, text, Vector2(65, 65), Color::Black);
        spriteBatch->DrawString(*spriteFont, text, Vector2(64, 64), Color::White);

        spriteBatch->End();
    }

    void InstancedModelSampleGame::HandleInput()
    {
        lastKeyboardState = currentKeyboardState;
        lastGamePadState = currentGamePadState;

        currentKeyboardState = Input::Keyboard::GetState();
        currentGamePadState = Input::GamePad::GetState(PlayerIndex::One);

        // Check for exit.
        if (currentKeyboardState.IsKeyDown(Input::Keys::Escape) ||
            currentGamePadState.getButtonsProperty().getBackProperty() ==
                Input::ButtonState::Pressed)
        {
            Exit();
        }

        // Change the number of instances more quickly if there are
        // already lots of them. This avoids you having to sit there
        // for hours with your finger on the "increase" button!
        const int instanceChangeRate = std::max((int)instances.size() / 100, 1);

        // Increase the number of instances?
        if (currentKeyboardState.IsKeyDown(Input::Keys::X) ||
            currentGamePadState.getButtonsProperty().getXProperty() ==
                Input::ButtonState::Pressed)
        {
            for (int i = 0; i < instanceChangeRate; i++)
            {
                instances.emplace_back();
            }
        }

        // Decrease the number of instances?
        if (currentKeyboardState.IsKeyDown(Input::Keys::Y) ||
            currentGamePadState.getButtonsProperty().getYProperty() ==
                Input::ButtonState::Pressed)
        {
            for (int i = 0; i < instanceChangeRate; i++)
            {
                if (instances.empty())
                    break;

                instances.pop_back();
            }
        }

        // Change which instancing technique we are using?
        if ((currentKeyboardState.IsKeyDown(Input::Keys::A) &&
             lastKeyboardState.IsKeyUp(Input::Keys::A)) ||
            (currentGamePadState.getButtonsProperty().getAProperty() ==
                 Input::ButtonState::Pressed &&
             lastGamePadState.getButtonsProperty().getAProperty() ==
                 Input::ButtonState::Released))
        {
            instancingTechnique = (InstancingTechnique)((int)instancingTechnique + 1);

            // Wrap if we reach the end of the possible techniques.
            if (instancingTechnique > InstancingTechnique::NoInstancingOrStateBatching)
                instancingTechnique = (InstancingTechnique)0;
        }
    }
}
