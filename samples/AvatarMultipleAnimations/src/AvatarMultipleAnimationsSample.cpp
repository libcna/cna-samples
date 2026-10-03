// SPDX-License-Identifier: MS-PL
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
#include "AvatarMultipleAnimationsSample.hpp"
#include "CNA/Platform/Entrypoint.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"

namespace AvatarMultipleAnimationsSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::GamerServices;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    AvatarMultipleAnimationsGame::AvatarMultipleAnimationsGame()
    {
        finalBoneTransforms.EnsureCapacity(AvatarRenderer::BoneCount);
        graphics = std::make_unique<GraphicsDeviceManager>(this);
        getContentProperty().setRootDirectoryProperty("Content");
        graphics->setPreferredBackBufferWidthProperty(1280);
        graphics->setPreferredBackBufferHeightProperty(720);
        graphics->setPreferMultiSamplingProperty(true);
        getComponentsProperty().Add(std::make_shared<GamerServicesComponent>(*this));
        CNAEXT GamePad::setKeyboardEmulationEnabledEXT(true);
    }

    const std::string& AvatarMultipleAnimationsGame::GetTypeName() const
    {
        static const std::string name = "AvatarMultipleAnimationsSample.AvatarMultipleAnimationsGame";
        return name;
    }

    void AvatarMultipleAnimationsGame::LoadContent()
    {
        spriteBatch.emplace(getGraphicsDeviceProperty());
        font.emplace(getContentProperty().Load<SpriteFont>("Font"));
        avatarDescription = AvatarDescription::CreateRandom();
        avatarRenderer = std::make_unique<AvatarRenderer>(&*avatarDescription);
        waveAnimation = std::make_unique<AvatarAnimation>(AvatarAnimationPreset::Wave);
        celebrateAnimation = std::make_unique<AvatarAnimation>(AvatarAnimationPreset::Celebrate);
        rightArmBones = FindInfluencedBones(AvatarBone::ShoulderRight, avatarRenderer->getParentBonesProperty());
        for (SharpRuntime::intcs i = 0; i < AvatarRenderer::BoneCount; ++i)
            finalBoneTransforms.Add(Matrix::getIdentityProperty());
        world = Matrix::CreateRotationY(MathHelper::Pi);
        projection = Matrix::CreatePerspectiveFieldOfView(MathHelper::PiOver4,
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(), .01f, 200.0f);
    }

    void AvatarMultipleAnimationsGame::Update(GameTime& gameTime)
    {
        HandleInput();
        UpdateCamera(gameTime);
        avatarRenderer->setWorldProperty(world);
        avatarRenderer->setViewProperty(view);
        avatarRenderer->setProjectionProperty(projection);
        if (avatarRenderer->getStateProperty() == AvatarRendererState::Ready)
        {
            waveAnimation->Update(gameTime.getElapsedGameTimeProperty(), true);
            celebrateAnimation->Update(gameTime.getElapsedGameTimeProperty(), true);
            UpdateTransforms();
        }
        Game::Update(gameTime);
    }

    void AvatarMultipleAnimationsGame::Draw(const GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);
        SharpRuntime::String message = "Playing: ";
        if (animationPlaybackMode == AnimationPlaybackMode::All) message += "Celebrate + Wave";
        else if (animationPlaybackMode == AnimationPlaybackMode::Celebrate) message += "Celebrate";
        else if (animationPlaybackMode == AnimationPlaybackMode::Wave) message += "Wave";
        message += "\n(Left Shoulder Button)";
        spriteBatch->Begin();
        spriteBatch->DrawString(*font, message, Vector2(161, 161), Color::Black);
        spriteBatch->DrawString(*font, message, Vector2(160, 160), Color::White);
        spriteBatch->End();
        avatarRenderer->Draw(finalBoneTransforms.ToVector(), celebrateAnimation->getExpressionProperty());
        Game::Draw(gameTime);
    }

    void AvatarMultipleAnimationsGame::UpdateTransforms()
    {
        const auto celebrateTransforms = celebrateAnimation->getBoneTransformsProperty();
        const auto waveTransforms = waveAnimation->getBoneTransformsProperty();
        if (animationPlaybackMode == AnimationPlaybackMode::All)
        {
            for (SharpRuntime::intcs i = 0; i < finalBoneTransforms.getCountProperty(); ++i)
                finalBoneTransforms[i] = celebrateTransforms[i];
            for (SharpRuntime::intcs i = 0; i < rightArmBones.getCountProperty(); ++i)
                finalBoneTransforms[rightArmBones[i]] = waveTransforms[rightArmBones[i]];
        }
        else if (animationPlaybackMode == AnimationPlaybackMode::Celebrate)
        {
            for (SharpRuntime::intcs i = 0; i < finalBoneTransforms.getCountProperty(); ++i)
                finalBoneTransforms[i] = celebrateTransforms[i];
        }
        else if (animationPlaybackMode == AnimationPlaybackMode::Wave)
        {
            for (SharpRuntime::intcs i = 0; i < finalBoneTransforms.getCountProperty(); ++i)
                finalBoneTransforms[i] = waveTransforms[i];
        }
    }

    System::Collections::Generic::List<SharpRuntime::intcs> AvatarMultipleAnimationsGame::FindInfluencedBones(
        AvatarBone avatarBone, System::Collections::ObjectModel::ReadOnlyCollection<SharpRuntime::intcs> parentBones)
    {
        System::Collections::Generic::List<SharpRuntime::intcs> influencedList;
        influencedList.Add(static_cast<SharpRuntime::intcs>(avatarBone));
        SharpRuntime::intcs currentBoneID = influencedList[0] + 1;
        while (currentBoneID < parentBones.getCountProperty())
        {
            if (influencedList.Contains(parentBones.getItem(currentBoneID))) influencedList.Add(currentBoneID);
            currentBoneID++;
        }
        return influencedList;
    }

    void AvatarMultipleAnimationsGame::HandleInput()
    {
        lastGamePadState = currentGamePadState;
        currentGamePadState = GamePad::GetState(PlayerIndex::One);
        const auto& buttons = currentGamePadState.getButtonsProperty();
        const auto& previous = lastGamePadState.getButtonsProperty();
        if (buttons.getBackProperty() == ButtonState::Pressed) Exit();
        if (buttons.getLeftShoulderProperty() == ButtonState::Pressed &&
            previous.getLeftShoulderProperty() != ButtonState::Pressed)
        {
            animationPlaybackMode = static_cast<AnimationPlaybackMode>(static_cast<SharpRuntime::intcs>(animationPlaybackMode) + 1);
            if (animationPlaybackMode > AnimationPlaybackMode::Wave) animationPlaybackMode = AnimationPlaybackMode::All;
        }
        if (buttons.getRightShoulderProperty() == ButtonState::Pressed &&
            previous.getRightShoulderProperty() != ButtonState::Pressed)
        {
            avatarDescription = AvatarDescription::CreateRandom();
            avatarRenderer = std::make_unique<AvatarRenderer>(&*avatarDescription);
        }
    }

    void AvatarMultipleAnimationsGame::UpdateCamera(const GameTime& gameTime)
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
    AvatarMultipleAnimationsSample::AvatarMultipleAnimationsGame game;
    game.Run();
    return 0;
}
