// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ShatterEffectGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ShatterEffectGame.hpp"

#include <algorithm>

#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace ShatterSample
{
    ShatterEffectGame::ShatterEffectGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");
    }

    const std::string& ShatterEffectGame::GetTypeName() const
    {
        static const std::string typeName{"ShatterSample.ShatterEffectGame"};
        return typeName;
    }

    void ShatterEffectGame::LoadContent()
    {
        model.emplace(getContentProperty().Load<Model>("tank"));
        font.emplace(getContentProperty().Load<SpriteFont>("font"));
        spriteBatch.emplace(*graphics.getGraphicsDeviceProperty());

        // Calculate View/Projection Matrices.
        view = Matrix::CreateLookAt(cameraPosition, targetPosition, Vector3::Up);
        projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::ToRadians(45.0f),
            graphics.getGraphicsDeviceProperty()->getViewportProperty().getAspectRatioProperty(),
            1.0f, 10000.0f);
    }

    void ShatterEffectGame::Update(GameTime& gameTime)
    {
        // Handle input
        const float elapsedTime =
            (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
        const Input::KeyboardState keyboardState = Input::Keyboard::GetState();
        const Input::GamePadState gamePadState = Input::GamePad::GetState(PlayerIndex::One);

        // Allows the default game to exit on Xbox 360 and Windows
        if (gamePadState.getButtonsProperty().getBackProperty() == Input::ButtonState::Pressed
            || keyboardState.IsKeyDown(Input::Keys::Escape))
            Exit();

        // Pressing the Up arrow or the A button on controller will Shatter the
        // model
        if (keyboardState.IsKeyDown(Input::Keys::Up) ||
            gamePadState.getButtonsProperty().getAProperty() == Input::ButtonState::Pressed)
        {
            time = std::min(duration, time + elapsedTime);
        }

        // Pressing the Down arrow or the B button on controller will reverse the
        // Shatter effect
        if (keyboardState.IsKeyDown(Input::Keys::Down) ||
            gamePadState.getButtonsProperty().getBProperty() == Input::ButtonState::Pressed)
        {
            time = std::max(0.0f, time - elapsedTime);
        }

        Game::Update(gameTime);
    }

    void ShatterEffectGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

        device.Clear(Color::CornflowerBlue);

        device.setBlendStateProperty(BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullNone);

        std::vector<Matrix> transforms(model->getBonesProperty().getCountProperty());
        model->CopyAbsoluteBoneTransformsTo(transforms);
        for (ModelMesh* mesh : model->getMeshesProperty())
        {
            for (ModelMeshPart* part : mesh->getMeshPartsProperty())
            {
                SetupEffect(transforms, *mesh, *part);
            }
            mesh->Draw();
        }

        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);

        // Draw instructions
        spriteBatch->Begin();
        spriteBatch->DrawString(*font,
            "Shatter Model: Hold Arrow Up or A button."
            "\nReverse Shatter: Hold Arrow Down or B button.", Vector2(50, 380),
            Color::White);
        spriteBatch->End();

        Game::Draw(gameTime);
    }

    void ShatterEffectGame::SetupEffect(const std::vector<Matrix>& transforms, ModelMesh& mesh,
                                        ModelMeshPart& part)
    {
        Effect* effect = part.getEffectProperty();
        const int bone = mesh.getParentBoneProperty()->getIndexProperty();

        effect->getParametersProperty()["TranslationAmount"]->SetValue(translationRate * time);
        effect->getParametersProperty()["RotationAmount"]->SetValue(rotationRate * time);
        effect->getParametersProperty()["time"]->SetValue(time);
        effect->getParametersProperty()["WorldViewProjection"]->SetValue(
            transforms[bone] * view * projection);
        effect->getParametersProperty()["World"]->SetValue(transforms[bone]);
        effect->getParametersProperty()["eyePosition"]->SetValue(cameraPosition);
        effect->getParametersProperty()["lightPosition"]->SetValue(lightPosition);
        effect->getParametersProperty()["ambientColor"]->SetValue(ambientColor);
        effect->getParametersProperty()["diffuseColor"]->SetValue(diffuseColor);
        effect->getParametersProperty()["specularColor"]->SetValue(specularColor);
        effect->getParametersProperty()["specularPower"]->SetValue(specularPower);
    }
}
