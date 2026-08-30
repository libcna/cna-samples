// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CustomModelEffect.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CustomModelEffectGame.hpp"

#include <cstddef>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace CustomModelEffect
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::Effect;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Graphics::ModelMesh;
    using Microsoft::Xna::Framework::Graphics::Viewport;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Keyboard;
    using Microsoft::Xna::Framework::Input::Keys;

    CustomModelEffectGame::CustomModelEffectGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");
    }

    const std::string& CustomModelEffectGame::GetTypeName() const
    {
        static const std::string name = "CustomModelEffect.CustomModelEffectGame";
        return name;
    }

    void CustomModelEffectGame::LoadContent()
    {
        model = getContentProperty().Load<Microsoft::Xna::Framework::Graphics::Model>("saucer");
    }

    void CustomModelEffectGame::Update(GameTime& gameTime)
    {
        HandleInput();
        Game::Update(gameTime);
    }

    void CustomModelEffectGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice* device = graphics.getGraphicsDeviceProperty();
        device->Clear(Color::CornflowerBlue);

        const Viewport viewport = device->getViewportProperty();
        const float aspectRatio = static_cast<float>(viewport.getWidthProperty())
            / static_cast<float>(viewport.getHeightProperty());
        const float time =
            static_cast<float>(gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());

        const Matrix rotation = Matrix::CreateRotationX(time * 0.3f)
            * Matrix::CreateRotationY(time);
        const Matrix view = Matrix::CreateLookAt(
            Vector3(4000.0f, 0.0f, 0.0f), Vector3::Zero, Vector3::Up);
        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4, aspectRatio, 10.0f, 10000.0f);

        std::vector<Matrix> transforms(
            static_cast<std::size_t>(model->getBonesProperty().getCountProperty()));
        model->CopyAbsoluteBoneTransformsTo(transforms);

        for (ModelMesh* mesh : model->getMeshesProperty())
        {
            for (Effect* effect : mesh->getEffectsProperty())
            {
                const Matrix world = transforms[static_cast<std::size_t>(
                    mesh->getParentBoneProperty()->getIndexProperty())] * rotation;

                effect->getParametersProperty()["World"]->SetValue(world);
                effect->getParametersProperty()["View"]->SetValue(view);
                effect->getParametersProperty()["Projection"]->SetValue(projection);
            }

            mesh->Draw();
        }

        Game::Draw(gameTime);
    }

    void CustomModelEffectGame::HandleInput()
    {
        const auto currentKeyboardState = Keyboard::GetState();
        const auto currentGamePadState = GamePad::GetState(PlayerIndex::One);

        if (currentKeyboardState.IsKeyDown(Keys::Escape)
            || currentGamePadState.getButtonsProperty().getBackProperty()
                == ButtonState::Pressed)
        {
            Exit();
        }
    }
}
