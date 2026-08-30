// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CpuSkinningDemoGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CpuSkinningDemoGame.hpp"

#include "CpuSkinningDataTypes/Animation/AnimationClip.hpp"
#include "CpuSkinningDataTypes/Animation/SkinningData.hpp"
#include "CpuSkinningDataTypes/CpuSkinnedModelPart.hpp"
#include "CpuSkinningDataTypes/CpuSkinningContentReaders.hpp"
#include "FrameRateCounter.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DirectionalLight.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/IEffectLights.hpp"
#include "Microsoft/Xna/Framework/Graphics/IEffectMatrices.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SkinnedEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/InvalidCastException.hpp"
#include "System/TimeSpan.hpp"

namespace CpuSkinningDemo
{
    using CpuSkinningDataTypes::AnimationClip;
    using CpuSkinningDataTypes::AnimationPlayer;
    using CpuSkinningDataTypes::CpuSkinnedModel;
    using CpuSkinningDataTypes::CpuSkinnedModelPart;
    using CpuSkinningDataTypes::CpuSkinningContentReaderRegistrationEXT;
    using CpuSkinningDataTypes::SkinningData;
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::BasicEffect;
    using Microsoft::Xna::Framework::Graphics::BlendState;
    using Microsoft::Xna::Framework::Graphics::DepthStencilState;
    using Microsoft::Xna::Framework::Graphics::Effect;
    using Microsoft::Xna::Framework::Graphics::IEffectLights;
    using Microsoft::Xna::Framework::Graphics::IEffectMatrices;
    using Microsoft::Xna::Framework::Graphics::Model;
    using Microsoft::Xna::Framework::Graphics::ModelMesh;
    using Microsoft::Xna::Framework::Graphics::RasterizerState;
    using Microsoft::Xna::Framework::Graphics::SamplerState;
    using Microsoft::Xna::Framework::Graphics::SkinnedEffect;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::SpriteFont;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Mouse;
    using Microsoft::Xna::Framework::Input::Touch::GestureSample;
    using Microsoft::Xna::Framework::Input::Touch::GestureType;
    using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

    CpuSkinningDemoGame::CpuSkinningDemoGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

#if defined(WINDOWS_PHONE)
        graphics.setIsFullScreenProperty(true);
        graphics.setPreferredBackBufferWidthProperty(480);
        graphics.setPreferredBackBufferHeightProperty(800);
#else
        setIsMouseVisibleProperty(true);
#endif

        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));

        getComponentsProperty().Add(new FrameRateCounter(*this));

        // CNAEXT: XNA discovers the sample-owned explicit and reflective readers through .NET
        // reflection. The C++ port registers the same canonical reader names and field graph.
        CNAEXT CpuSkinningContentReaderRegistrationEXT::RegisterEXT();
    }

    const std::string& CpuSkinningDemoGame::GetTypeName() const
    {
        static const std::string name = "CpuSkinningDemo.CpuSkinningDemoGame";
        return name;
    }

    void CpuSkinningDemoGame::LoadContent()
    {
        spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        font.emplace(getContentProperty().Load<SpriteFont>("font"));

        gpuDude.emplace(getContentProperty().Load<Model>("dude_gpu"));
        cpuDude = getContentProperty().Load<std::shared_ptr<CpuSkinnedModel>>("dude_cpu");

        SkinningData* skinningData = cpuDude->getSkinningDataProperty();
        animationPlayer = std::make_unique<AnimationPlayer>(*skinningData);
        const std::shared_ptr<AnimationClip>& clip =
            skinningData->getAnimationClipsProperty().at("Take 001");
        animationPlayer->StartClip(*clip);

        TouchPanel::setEnabledGesturesProperty(GestureType::Tap | GestureType::FreeDrag);
    }

    void CpuSkinningDemoGame::Update(GameTime& gameTime)
    {
        if (GamePad::GetState(PlayerIndex::One)
                .getButtonsProperty().getBackProperty() == ButtonState::Pressed)
        {
            Exit();
        }

        animationPlayer->Update(
            gameTime.getElapsedGameTimeProperty(), true, Matrix::getIdentityProperty());

        while (TouchPanel::getIsGestureAvailableProperty())
        {
            const GestureSample gesture = TouchPanel::ReadGesture();
            if (gesture.getGestureTypeProperty() == GestureType::Tap)
            {
                displayCpuModel = !displayCpuModel;
            }
            else if (gesture.getGestureTypeProperty() == GestureType::FreeDrag)
            {
                HandleDrag(gesture.getDeltaProperty());
            }
        }

#if !defined(WINDOWS_PHONE)
        mousePrev = mouse;
        mouse = Mouse::GetState();

        if (mouse.getLeftButtonProperty() == ButtonState::Pressed)
        {
            HandleDrag(Vector2(
                static_cast<float>(mouse.getXProperty() - mousePrev.getXProperty()),
                static_cast<float>(mouse.getYProperty() - mousePrev.getYProperty())));
        }
        else if (mouse.getRightButtonProperty() == ButtonState::Pressed
                 && mousePrev.getRightButtonProperty() == ButtonState::Released)
        {
            displayCpuModel = !displayCpuModel;
        }
#endif

        Game::Update(gameTime);
    }

    void CpuSkinningDemoGame::HandleDrag(const Vector2& delta)
    {
        cameraRotation += delta.X / 4.0f;
        cameraArc = MathHelper::Clamp(cameraArc - delta.Y / 4.0f, -70.0f, 70.0f);
    }

    void CpuSkinningDemoGame::Draw(const GameTime& gameTime)
    {
        const Matrix view = Matrix::CreateTranslation(0.0f, -40.0f, 0.0f)
            * Matrix::CreateRotationY(MathHelper::ToRadians(cameraRotation))
            * Matrix::CreateRotationX(MathHelper::ToRadians(cameraArc))
            * Matrix::CreateLookAt(
                Vector3(0.0f, 0.0f, -100.0f), Vector3::Zero, Vector3::Up);
        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(),
            1.0f,
            1000.0f);

        auto& device = getGraphicsDeviceProperty();
        device.Clear(Color::Black);
        device.setBlendStateProperty(BlendState::Opaque);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.getSamplerStatesProperty()[0] = SamplerState::LinearWrap;

        if (displayCpuModel)
        {
            for (const std::shared_ptr<CpuSkinnedModelPart>& modelPart
                 : cpuDude->getPartsProperty())
            {
                modelPart->SetBones(animationPlayer->getSkinTransformsProperty());

                BasicEffect* effect = modelPart->getEffectProperty();
                effect->setSpecularColorProperty(Vector3::Zero);

                ConfigureEffectMatrices(
                    *effect, Matrix::getIdentityProperty(), view, projection);
                ConfigureEffectLighting(*effect);
                modelPart->Draw();
            }
        }
        else
        {
            for (ModelMesh* mesh : gpuDude->getMeshesProperty())
            {
                for (Effect* baseEffect : mesh->getEffectsProperty())
                {
                    auto* effect = dynamic_cast<SkinnedEffect*>(baseEffect);
                    if (effect == nullptr)
                    {
                        throw System::InvalidCastException(
                            "CPU Skinning expected every GPU model effect to be SkinnedEffect.");
                    }

                    effect->SetBoneTransforms(animationPlayer->getSkinTransformsProperty());
                    effect->setSpecularColorProperty(Vector3::Zero);

                    ConfigureEffectMatrices(
                        *effect, Matrix::getIdentityProperty(), view, projection);
                    ConfigureEffectLighting(*effect);
                }

                mesh->Draw();
            }
        }

        spriteBatch->Begin();
        spriteBatch->DrawString(
            *font,
            displayCpuModel ? cpuSkinningOn : gpuSkinningOn,
            Vector2(32.0f,
                    static_cast<float>(device.getViewportProperty().getHeightProperty() - 100)),
            Color::White);
        spriteBatch->End();

        Game::Draw(gameTime);
    }

    void CpuSkinningDemoGame::ConfigureEffectMatrices(
        IEffectMatrices& effect,
        const Matrix& world,
        const Matrix& view,
        const Matrix& projection)
    {
        effect.setWorldProperty(world);
        effect.setViewProperty(view);
        effect.setProjectionProperty(projection);
    }

    void CpuSkinningDemoGame::ConfigureEffectLighting(IEffectLights& effect)
    {
        effect.EnableDefaultLighting();
        effect.getDirectionalLight0Property().setDirectionProperty(Vector3::Backward);
        effect.getDirectionalLight0Property().setEnabledProperty(true);
        effect.getDirectionalLight1Property().setEnabledProperty(false);
        effect.getDirectionalLight2Property().setEnabledProperty(false);
    }
}
