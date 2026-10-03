// SPDX-License-Identifier: MS-PL
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
#include "AvatarAnimationBlendingSample.hpp"
#include "CNA/Platform/Entrypoint.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"

namespace AvatarAnimationBlendingSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::GamerServices;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    AvatarAnimationBlendingGame::AvatarAnimationBlendingGame()
    {
        graphics = std::make_unique<GraphicsDeviceManager>(this);
        getContentProperty().setRootDirectoryProperty("Content");
        graphics->setPreferredBackBufferWidthProperty(1280);
        graphics->setPreferredBackBufferHeightProperty(720);
        graphics->setPreferMultiSamplingProperty(true);
        getComponentsProperty().Add(std::make_shared<GamerServicesComponent>(*this));
        CNAEXT GamePad::setKeyboardEmulationEnabledEXT(true);
    }

    const std::string& AvatarAnimationBlendingGame::GetTypeName() const
    {
        static const std::string name = "AvatarAnimationBlendingSample.AvatarAnimationBlendingGame";
        return name;
    }

    void AvatarAnimationBlendingGame::LoadContent()
    {
        spriteBatch.emplace(getGraphicsDeviceProperty());
        font.emplace(getContentProperty().Load<SpriteFont>("Font"));
        avatarDescription = AvatarDescription::CreateRandom();
        avatarRenderer = std::make_unique<AvatarRenderer>(&*avatarDescription);
        avatarAnimations[0] = std::make_unique<AvatarAnimation>(AvatarAnimationPreset::Stand0);
        avatarAnimations[1] = std::make_unique<AvatarAnimation>(AvatarAnimationPreset::Celebrate);
        avatarAnimations[2] = std::make_unique<AvatarAnimation>(AvatarAnimationPreset::Clap);
        avatarAnimations[3] = std::make_unique<AvatarAnimation>(AvatarAnimationPreset::Wave);
        blendedAnimation = std::make_unique<AvatarBlendedAnimation>(avatarAnimations[0].get());
        noBlendingAnimation = avatarAnimations[0].get();
        world = Matrix::CreateRotationY(MathHelper::Pi);
        projection = Matrix::CreatePerspectiveFieldOfView(MathHelper::PiOver4,
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(), .01f, 200.0f);
    }

    void AvatarAnimationBlendingGame::Update(GameTime& gameTime)
    {
        HandleInput();
        UpdateCamera(gameTime);
        avatarRenderer->setWorldProperty(world);
        avatarRenderer->setViewProperty(view);
        avatarRenderer->setProjectionProperty(projection);
        if (avatarRenderer->getStateProperty() == AvatarRendererState::Ready)
        {
            if (useAnimationBlending) blendedAnimation->Update(gameTime.getElapsedGameTimeProperty(), true);
            else noBlendingAnimation->Update(gameTime.getElapsedGameTimeProperty(), true);
        }
        Game::Update(gameTime);
    }

    void AvatarAnimationBlendingGame::Draw(const GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);
        const SharpRuntime::String message = SharpRuntime::String("Blending: ") +
            (useAnimationBlending ? "On" : "Off") + "\n(Left Shoulder Button)";
        spriteBatch->Begin();
        spriteBatch->DrawString(*font, message, Vector2(161, 161), Color::Black);
        spriteBatch->DrawString(*font, message, Vector2(160, 160), Color::White);
        spriteBatch->End();
        if (useAnimationBlending) avatarRenderer->Draw(blendedAnimation.get());
        else avatarRenderer->Draw(noBlendingAnimation);
        Game::Draw(gameTime);
    }

    void AvatarAnimationBlendingGame::HandleInput()
    {
        lastGamePadState = currentGamePadState;
        currentGamePadState = GamePad::GetState(PlayerIndex::One);
        const auto& buttons = currentGamePadState.getButtonsProperty();
        const auto& previous = lastGamePadState.getButtonsProperty();
        if (buttons.getBackProperty() == ButtonState::Pressed) Exit();
        if (buttons.getLeftShoulderProperty() == ButtonState::Pressed &&
            previous.getLeftShoulderProperty() != ButtonState::Pressed)
            useAnimationBlending = !useAnimationBlending;
        if (buttons.getRightShoulderProperty() == ButtonState::Pressed &&
            previous.getRightShoulderProperty() != ButtonState::Pressed)
        {
            avatarDescription = AvatarDescription::CreateRandom();
            avatarRenderer = std::make_unique<AvatarRenderer>(&*avatarDescription);
        }
        if (buttons.getAProperty() == ButtonState::Pressed && previous.getAProperty() != ButtonState::Pressed)
        {
            blendedAnimation->Play(avatarAnimations[1].get());
            noBlendingAnimation = avatarAnimations[1].get();
        }
        else if (buttons.getBProperty() == ButtonState::Pressed && previous.getBProperty() != ButtonState::Pressed)
        {
            blendedAnimation->Play(avatarAnimations[2].get());
            noBlendingAnimation = avatarAnimations[2].get();
        }
        else if (buttons.getXProperty() == ButtonState::Pressed && previous.getXProperty() != ButtonState::Pressed)
        {
            blendedAnimation->Play(avatarAnimations[0].get());
            noBlendingAnimation = avatarAnimations[0].get();
        }
        else if (buttons.getYProperty() == ButtonState::Pressed && previous.getYProperty() != ButtonState::Pressed)
        {
            blendedAnimation->Play(avatarAnimations[3].get());
            noBlendingAnimation = avatarAnimations[3].get();
        }
    }

    void AvatarAnimationBlendingGame::UpdateCamera(const GameTime& gameTime)
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
    AvatarAnimationBlendingSample::AvatarAnimationBlendingGame game;
    game.Run();
    return 0;
}
