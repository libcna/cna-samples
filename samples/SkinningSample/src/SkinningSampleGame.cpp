// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SkinningSample.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "SkinningSampleGame.hpp"

#include "SkinnedModel/AnimationClip.hpp"
#include "SkinnedModel/SkinningContentReaders.hpp"
#include "SkinnedModel/SkinningData.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/SkinnedEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/InvalidCastException.hpp"
#include "System/InvalidOperationException.hpp"

namespace SkinningSample
{
    using SkinnedModel::AnimationClip;
    using SkinnedModel::SkinningContentReaderRegistrationEXT;
    using SkinnedModel::SkinningData;
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::Effect;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Graphics::Model;
    using Microsoft::Xna::Framework::Graphics::ModelMesh;
    using Microsoft::Xna::Framework::Graphics::SkinnedEffect;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Keyboard;
    using Microsoft::Xna::Framework::Input::Keys;

    SkinningSampleGame::SkinningSampleGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        // CNAEXT: XNA reflects over the sample-owned serialized classes. C++ has no runtime
        // reflection, so the sample declares the same field graph once for CNA's AOT readers.
        CNAEXT SkinningContentReaderRegistrationEXT::RegisterEXT();
    }

    const std::string& SkinningSampleGame::GetTypeName() const
    {
        static const std::string name = "SkinningSample.SkinningSampleGame";
        return name;
    }

    void SkinningSampleGame::LoadContent()
    {
        currentModel = getContentProperty().Load<Model>("dude");

        auto* skinningData = dynamic_cast<SkinningData*>(currentModel->getTagProperty());
        if (skinningData == nullptr)
        {
            throw System::InvalidOperationException(
                "This model does not contain a SkinningData tag.");
        }

        animationPlayer = std::make_unique<SkinnedModel::AnimationPlayer>(*skinningData);
        const std::shared_ptr<AnimationClip>& clip =
            skinningData->getAnimationClipsProperty().at("Take 001");
        animationPlayer->StartClip(*clip);
    }

    void SkinningSampleGame::Update(GameTime& gameTime)
    {
        HandleInput();
        UpdateCamera(gameTime);
        animationPlayer->Update(
            gameTime.getElapsedGameTimeProperty(), true, Matrix::getIdentityProperty());
        Game::Update(gameTime);
    }

    void SkinningSampleGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice* device = graphics.getGraphicsDeviceProperty();
        device->Clear(Color::CornflowerBlue);

        const std::vector<Matrix>& bones = animationPlayer->GetSkinTransforms();

        const Matrix view = Matrix::CreateTranslation(0.0f, -40.0f, 0.0f)
            * Matrix::CreateRotationY(MathHelper::ToRadians(cameraRotation))
            * Matrix::CreateRotationX(MathHelper::ToRadians(cameraArc))
            * Matrix::CreateLookAt(Vector3(0.0f, 0.0f, -cameraDistance),
                                   Vector3::Zero, Vector3::Up);

        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            device->getViewportProperty().getAspectRatioProperty(),
            1.0f, 10000.0f);

        for (ModelMesh* mesh : currentModel->getMeshesProperty())
        {
            for (Effect* baseEffect : mesh->getEffectsProperty())
            {
                auto* effect = dynamic_cast<SkinnedEffect*>(baseEffect);
                if (effect == nullptr)
                {
                    throw System::InvalidCastException(
                        "SkinningSample expected every model effect to be SkinnedEffect.");
                }

                effect->SetBoneTransforms(bones);
                effect->setViewProperty(view);
                effect->setProjectionProperty(projection);
                effect->EnableDefaultLighting();
                effect->setSpecularColorProperty(Vector3(0.25f));
                effect->setSpecularPowerProperty(16.0f);
            }

            mesh->Draw();
        }

        Game::Draw(gameTime);
    }

    void SkinningSampleGame::HandleInput()
    {
        currentKeyboardState = Keyboard::GetState();
        currentGamePadState = GamePad::GetState(PlayerIndex::One);

        if (currentKeyboardState.IsKeyDown(Keys::Escape)
            || currentGamePadState.getButtonsProperty().getBackProperty()
                == ButtonState::Pressed)
        {
            Exit();
        }
    }

    void SkinningSampleGame::UpdateCamera(const GameTime& gameTime)
    {
        const float time = static_cast<float>(
            gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty());

        if (currentKeyboardState.IsKeyDown(Keys::Up)
            || currentKeyboardState.IsKeyDown(Keys::W))
        {
            cameraArc += time * 0.1f;
        }

        if (currentKeyboardState.IsKeyDown(Keys::Down)
            || currentKeyboardState.IsKeyDown(Keys::S))
        {
            cameraArc -= time * 0.1f;
        }

        cameraArc += currentGamePadState.getThumbSticksProperty()
            .getRightProperty().Y * time * 0.25f;

        if (cameraArc > 90.0f)
            cameraArc = 90.0f;
        else if (cameraArc < -90.0f)
            cameraArc = -90.0f;

        if (currentKeyboardState.IsKeyDown(Keys::Right)
            || currentKeyboardState.IsKeyDown(Keys::D))
        {
            cameraRotation += time * 0.1f;
        }

        if (currentKeyboardState.IsKeyDown(Keys::Left)
            || currentKeyboardState.IsKeyDown(Keys::A))
        {
            cameraRotation -= time * 0.1f;
        }

        cameraRotation += currentGamePadState.getThumbSticksProperty()
            .getRightProperty().X * time * 0.25f;

        if (currentKeyboardState.IsKeyDown(Keys::Z))
            cameraDistance += time * 0.25f;

        if (currentKeyboardState.IsKeyDown(Keys::X))
            cameraDistance -= time * 0.25f;

        cameraDistance += currentGamePadState.getTriggersProperty().getLeftProperty()
            * time * 0.5f;
        cameraDistance -= currentGamePadState.getTriggersProperty().getRightProperty()
            * time * 0.5f;

        if (cameraDistance > 500.0f)
            cameraDistance = 500.0f;
        else if (cameraDistance < 10.0f)
            cameraDistance = 10.0f;

        if (currentGamePadState.getButtonsProperty().getRightStickProperty()
                == ButtonState::Pressed
            || currentKeyboardState.IsKeyDown(Keys::R))
        {
            cameraArc = 0.0f;
            cameraRotation = 0.0f;
            cameraDistance = 100.0f;
        }
    }
}
