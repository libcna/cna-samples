// SPDX-License-Identifier: MS-PL
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
#include "ObjectPlacementOnAvatar.hpp"
#include "CNA/Platform/Entrypoint.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "System/InvalidCastException.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"

namespace ObjectPlacementOnAvatar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::GamerServices;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    ObjectPlacementOnAvatarGame::ObjectPlacementOnAvatarGame()
    {
        graphics = std::make_unique<GraphicsDeviceManager>(this);
        getContentProperty().setRootDirectoryProperty("Content");
        graphics->setPreferredBackBufferWidthProperty(1280);
        graphics->setPreferredBackBufferHeightProperty(720);
        graphics->setPreferMultiSamplingProperty(true);
        getComponentsProperty().Add(std::make_shared<GamerServicesComponent>(*this));
        CNAEXT GamePad::setKeyboardEmulationEnabledEXT(true);
    }

    const std::string& ObjectPlacementOnAvatarGame::GetTypeName() const
    {
        static const std::string name = "ObjectPlacementOnAvatar.ObjectPlacementOnAvatarGame";
        return name;
    }

    void ObjectPlacementOnAvatarGame::LoadContent()
    {
        avatarDescription = AvatarDescription::CreateRandom();
        avatarRenderer = std::make_unique<AvatarRenderer>(&*avatarDescription);
        avatarAnimations[0] = std::make_unique<AvatarAnimation>(AvatarAnimationPreset::Stand0);
        avatarAnimations[1] = std::make_unique<AvatarAnimation>(AvatarAnimationPreset::Celebrate);
        avatarAnimations[2] = std::make_unique<AvatarAnimation>(AvatarAnimationPreset::Clap);
        avatarAnimations[3] = std::make_unique<AvatarAnimation>(AvatarAnimationPreset::Stand5);
        currentAvatarAnimation = avatarAnimations[0].get();
        baseballBat.emplace(getContentProperty().Load<Model>("baseballbat"));
        world = Matrix::CreateRotationY(MathHelper::Pi);
        projection = Matrix::CreatePerspectiveFieldOfView(MathHelper::PiOver4,
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(), .01f, 200.0f);
        bonesWorldSpace.EnsureCapacity(AvatarRenderer::BoneCount);
        for (SharpRuntime::intcs i = 0; i < AvatarRenderer::BoneCount; ++i)
            bonesWorldSpace.Add(Matrix::getIdentityProperty());
    }

    void ObjectPlacementOnAvatarGame::Update(GameTime& gameTime)
    {
        HandleInput();
        UpdateCamera(gameTime);
        avatarRenderer->setWorldProperty(world);
        avatarRenderer->setViewProperty(view);
        avatarRenderer->setProjectionProperty(projection);
        if (avatarRenderer->getStateProperty() == AvatarRendererState::Ready)
        {
            currentAvatarAnimation->Update(gameTime.getElapsedGameTimeProperty(), true);
            BonesToWorldSpace(*avatarRenderer, *currentAvatarAnimation, bonesWorldSpace);
        }
        Game::Update(gameTime);
    }

    void ObjectPlacementOnAvatarGame::Draw(const GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);
        DrawBaseballBat();
        const auto bones = currentAvatarAnimation->getBoneTransformsProperty();
        avatarRenderer->Draw(std::vector<Matrix>(bones.begin(), bones.end()),
            currentAvatarAnimation->getExpressionProperty());
        Game::Draw(gameTime);
    }

    void ObjectPlacementOnAvatarGame::DrawBaseballBat()
    {
        const Matrix baseballBatOffset = Matrix::CreateRotationY(MathHelper::ToRadians(-20)) *
            Matrix::CreateTranslation(.01f, .05f, 0.0f);
        for (ModelMesh* mesh : baseballBat->getMeshesProperty())
        {
            for (Effect* baseEffect : mesh->getEffectsProperty())
            {
                auto* effect = dynamic_cast<BasicEffect*>(baseEffect);
                if (!effect) throw System::InvalidCastException("ObjectPlacementOnAvatar expects BasicEffect.");
                effect->EnableDefaultLighting();
                effect->setWorldProperty(baseballBatOffset * bonesWorldSpace[static_cast<SharpRuntime::intcs>(AvatarBone::SpecialRight)]);
                effect->setViewProperty(view);
                effect->setProjectionProperty(projection);
            }
            mesh->Draw();
        }
    }

    void ObjectPlacementOnAvatarGame::BonesToWorldSpace(AvatarRenderer& renderer,
        AvatarAnimation& animation, System::Collections::Generic::List<Matrix>& boneToUpdate)
    {
        const auto bindPose = renderer.getBindPoseProperty();
        const auto animationPose = animation.getBoneTransformsProperty();
        const auto parentIndex = renderer.getParentBonesProperty();
        for (SharpRuntime::intcs i = 0; i < AvatarRenderer::BoneCount; ++i)
        {
            const Matrix parentMatrix = parentIndex[i] != -1 ? boneToUpdate[parentIndex[i]] : renderer.getWorldProperty();
            boneToUpdate[i] = Matrix::Multiply(Matrix::Multiply(animationPose[i], bindPose[i]), parentMatrix);
        }
    }

    void ObjectPlacementOnAvatarGame::HandleInput()
    {
        lastGamePadState = currentGamePadState;
        currentGamePadState = GamePad::GetState(PlayerIndex::One);
        const auto& buttons = currentGamePadState.getButtonsProperty();
        const auto& previous = lastGamePadState.getButtonsProperty();
        if (buttons.getBackProperty() == ButtonState::Pressed) Exit();
        if (buttons.getRightShoulderProperty() == ButtonState::Pressed &&
            previous.getRightShoulderProperty() != ButtonState::Pressed)
        {
            avatarDescription = AvatarDescription::CreateRandom();
            avatarRenderer = std::make_unique<AvatarRenderer>(&*avatarDescription);
        }
        if (buttons.getAProperty() == ButtonState::Pressed &&
            previous.getAProperty() != ButtonState::Pressed)
        {
            currentAvatarAnimation = avatarAnimations[1].get();
            currentAvatarAnimation->setCurrentPositionProperty(System::TimeSpan::Zero);
        }
        else if (buttons.getBProperty() == ButtonState::Pressed &&
            previous.getBProperty() != ButtonState::Pressed)
        {
            currentAvatarAnimation = avatarAnimations[2].get();
            currentAvatarAnimation->setCurrentPositionProperty(System::TimeSpan::Zero);
        }
        else if (buttons.getXProperty() == ButtonState::Pressed &&
            previous.getXProperty() != ButtonState::Pressed)
        {
            currentAvatarAnimation = avatarAnimations[3].get();
            currentAvatarAnimation->setCurrentPositionProperty(System::TimeSpan::Zero);
        }
        else if (buttons.getYProperty() == ButtonState::Pressed &&
            previous.getYProperty() != ButtonState::Pressed)
        {
            currentAvatarAnimation = avatarAnimations[0].get();
            currentAvatarAnimation->setCurrentPositionProperty(System::TimeSpan::Zero);
        }
    }

    void ObjectPlacementOnAvatarGame::UpdateCamera(const GameTime& gameTime)
    {
        const SharpRuntime::Single time = static_cast<SharpRuntime::Single>(gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty());
        if (currentGamePadState.getButtonsProperty().getRightStickProperty() == ButtonState::Pressed)
        {
            cameraArc = CameraDefaultArc;
            cameraDistance = CameraDefaultDistance;
            cameraRotation = CameraDefaultRotation;
        }
        cameraArc += currentGamePadState.getThumbSticksProperty().getRightProperty().Y * time * CameraRotateSpeed;
        cameraArc = MathHelper::Clamp(cameraArc, -90.0f, 90.0f);
        cameraRotation += currentGamePadState.getThumbSticksProperty().getRightProperty().X * time * CameraRotateSpeed;
        cameraDistance += currentGamePadState.getTriggersProperty().getLeftProperty() * time * CameraZoomSpeed;
        cameraDistance -= currentGamePadState.getTriggersProperty().getRightProperty() * time * CameraZoomSpeed;
        cameraDistance = MathHelper::Clamp(cameraDistance, CameraMinDistance, CameraMaxDistance);
        const Matrix unrotatedView = Matrix::CreateLookAt(Vector3(0, 0, cameraDistance), Vector3(0, 1, 0), Vector3::Up);
        view = Matrix::CreateRotationY(MathHelper::ToRadians(cameraRotation)) *
            Matrix::CreateRotationX(MathHelper::ToRadians(cameraArc)) * unrotatedView;
    }
}

int main()
{
    ObjectPlacementOnAvatar::ObjectPlacementOnAvatarGame game;
    game.Run();
    return 0;
}
