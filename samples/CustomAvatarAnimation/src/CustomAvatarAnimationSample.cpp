// SPDX-License-Identifier: MS-PL
// Copyright (C) Microsoft Corporation. All rights reserved.
#include "CustomAvatarAnimationSample.hpp"
#include "CustomAvatarAnimation/CustomAvatarAnimationContentReaders.hpp"
#include "CNA/Platform/Entrypoint.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesComponent.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamerCollection.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
namespace CustomAvatarAnimationSample {
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::GamerServices;
using namespace Microsoft::Xna::Framework::Graphics;
using namespace Microsoft::Xna::Framework::Input;
using namespace CustomAvatarAnimation;
CustomAvatarAnimationSampleGame::CustomAvatarAnimationSampleGame() {
    graphics = std::make_unique<GraphicsDeviceManager>(this);
    getContentProperty().setRootDirectoryProperty("Content");
    graphics->setPreferredBackBufferWidthProperty(1280);
    graphics->setPreferredBackBufferHeightProperty(720);
    graphics->setPreferMultiSamplingProperty(true);
    getComponentsProperty().Add(std::make_shared<GamerServicesComponent>(*this));
    signedInToken = SignedInGamer::SignedIn.Add([this](System::Object* sender, const SignedInEventArgs& e) { SignedInGamer_SignedIn(sender,e); });
    CNAEXT GamePad::setKeyboardEmulationEnabledEXT(true);
    CNAEXT CustomAvatarAnimationContentReaderRegistrationEXT::RegisterEXT();
}
CustomAvatarAnimationSampleGame::~CustomAvatarAnimationSampleGame() { SignedInGamer::SignedIn.Remove(signedInToken); }
const std::string& CustomAvatarAnimationSampleGame::GetTypeName() const {
    static const std::string name = "CustomAvatarAnimationSample.CustomAvatarAnimationSampleGame"; return name;
}
void CustomAvatarAnimationSampleGame::SignedInGamer_SignedIn(System::Object*, const SignedInEventArgs& e) {
    if (e.getGamerProperty()->getPlayerIndexProperty() == PlayerIndex::One) LoadAvatar(e.getGamerProperty());
}
void CustomAvatarAnimationSampleGame::LoadContent() {
    for (int i=0;i<4;++i) animations[i]=std::make_unique<AvatarAnimation>(static_cast<AvatarAnimationPreset>(static_cast<int>(AvatarAnimationPreset::Stand0)+i));
    {
        auto animationData = getContentProperty().Load<std::shared_ptr<CustomAvatarAnimationData>>("Walk");
        animations[4] = std::make_unique<CustomAvatarAnimationPlayer>(animationData->getNameProperty(), animationData->getLengthProperty(),
            animationData->getKeyframesProperty(), animationData->getExpressionKeyframesProperty());
    }
    {
        auto animationData = getContentProperty().Load<std::shared_ptr<CustomAvatarAnimationData>>("Jump");
        animations[5] = std::make_unique<CustomAvatarAnimationPlayer>(animationData->getNameProperty(), animationData->getLengthProperty(),
            animationData->getKeyframesProperty(), animationData->getExpressionKeyframesProperty());
    }
    {
        auto animationData = getContentProperty().Load<std::shared_ptr<CustomAvatarAnimationData>>("Kick");
        animations[6] = std::make_unique<CustomAvatarAnimationPlayer>(animationData->getNameProperty(), animationData->getLengthProperty(),
            animationData->getKeyframesProperty(), animationData->getExpressionKeyframesProperty());
    }
    {
        auto animationData = getContentProperty().Load<std::shared_ptr<CustomAvatarAnimationData>>("Punch");
        animations[7] = std::make_unique<CustomAvatarAnimationPlayer>(animationData->getNameProperty(), animationData->getLengthProperty(),
            animationData->getKeyframesProperty(), animationData->getExpressionKeyframesProperty());
    }
    {
        auto animationData = getContentProperty().Load<std::shared_ptr<CustomAvatarAnimationData>>("Faint");
        animations[8] = std::make_unique<CustomAvatarAnimationPlayer>(animationData->getNameProperty(), animationData->getLengthProperty(),
            animationData->getKeyframesProperty(), animationData->getExpressionKeyframesProperty());
    }
    groundModel.emplace(getContentProperty().Load<Model>("ground"));
    PlayRandomIdle();
    projection = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(45),
        getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(), .01f, 200.0f);
    world = Matrix::getIdentityProperty();
}
void CustomAvatarAnimationSampleGame::Update(GameTime& gameTime) {
    lastGamePadState=currentGamePadState;
    currentGamePadState=GamePad::GetState(PlayerIndex::One);
    if (currentGamePadState.getButtonsProperty().getBackProperty()==ButtonState::Pressed) Exit();
    HandleAvatarInput(gameTime); HandleCameraInput();
    const bool loopAnimation = currentType == AnimationType::Walk;
    animations[static_cast<int>(currentType)]->Update(gameTime.getElapsedGameTimeProperty(),loopAnimation);
    if (!loopAnimation && animations[static_cast<int>(currentType)]->getCurrentPositionProperty()==animations[static_cast<int>(currentType)]->getLengthProperty()) PlayRandomIdle();
    Game::Update(gameTime);
}
void CustomAvatarAnimationSampleGame::Draw(const GameTime& gameTime) {
    getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);
    groundModel->Draw(Matrix::getIdentityProperty(), view, projection);
    if (avatarRenderer) avatarRenderer->Draw(animations[static_cast<int>(currentType)].get());
    Game::Draw(gameTime);
}
void CustomAvatarAnimationSampleGame::HandleAvatarInput(const GameTime& gameTime) {
    const auto& buttons=currentGamePadState.getButtonsProperty(); const auto& previous=lastGamePadState.getButtonsProperty();
    if (buttons.getRightShoulderProperty()==ButtonState::Pressed && previous.getRightShoulderProperty()!=ButtonState::Pressed) LoadRandomAvatar();
    else if (buttons.getLeftShoulderProperty()==ButtonState::Pressed && previous.getLeftShoulderProperty()!=ButtonState::Pressed) {
        auto* gamers=Gamer::getSignedInGamersProperty();
        if (gamers && (*gamers)[PlayerIndex::One]) LoadAvatar((*gamers)[PlayerIndex::One]);
    }
    if (buttons.getAProperty()==ButtonState::Pressed && previous.getAProperty()!=ButtonState::Pressed) PlayAnimation(AnimationType::Jump);
    if (buttons.getBProperty()==ButtonState::Pressed && previous.getBProperty()!=ButtonState::Pressed) PlayAnimation(AnimationType::Kick);
    if (buttons.getXProperty()==ButtonState::Pressed && previous.getXProperty()!=ButtonState::Pressed) PlayAnimation(AnimationType::Punch);
    if (buttons.getYProperty()==ButtonState::Pressed && previous.getYProperty()!=ButtonState::Pressed) PlayAnimation(AnimationType::Faint);
    UpdateAvatarMovement(gameTime);
}
void CustomAvatarAnimationSampleGame::UpdateAvatarMovement(const GameTime& gameTime) {
    Vector2 leftThumbStick=currentGamePadState.getThumbSticksProperty().getLeftProperty();
    Vector3 avatarForward=world.getForwardProperty(), translate=Vector3::Zero;
    if (leftThumbStick.Length()>.2f) {
        leftThumbStick.Normalize();
        avatarForward.X=leftThumbStick.X;avatarForward.Y=0;avatarForward.Z=-leftThumbStick.Y;
        avatarForward=Vector3::Transform(avatarForward,Matrix::CreateRotationY(cameraRotation));
        avatarForward.Normalize();
        translate=avatarForward*(static_cast<float>(gameTime.getElapsedGameTimeProperty().getTotalMillisecondsProperty())*.0009f);
        currentType=AnimationType::Walk;
    } else if (currentType==AnimationType::Walk) PlayRandomIdle();
    world.setForwardProperty(avatarForward);
    world.setRightProperty(Vector3::Normalize(Vector3::Cross(world.getForwardProperty(),Vector3::Up)));
    world.setUpProperty(Vector3::Normalize(Vector3::Cross(world.getRightProperty(),world.getForwardProperty())));
    world.setTranslationProperty(world.getTranslationProperty()+translate);
    if (avatarRenderer) avatarRenderer->setWorldProperty(world);
}
void CustomAvatarAnimationSampleGame::HandleCameraInput() {
    if (currentGamePadState.getButtonsProperty().getRightStickProperty()==ButtonState::Pressed) {
        cameraArc=CameraDefaultArc;cameraDistance=CameraDefaultDistance;cameraRotation=CameraDefaultRotation;
    }
    cameraArc-=currentGamePadState.getThumbSticksProperty().getRightProperty().Y*.05f;
    cameraRotation+=currentGamePadState.getThumbSticksProperty().getRightProperty().X*.1f;
    cameraDistance+=currentGamePadState.getTriggersProperty().getLeftProperty()*.1f;
    cameraDistance-=currentGamePadState.getTriggersProperty().getRightProperty()*.1f;
    if (cameraDistance>5)cameraDistance=5;else if(cameraDistance<2)cameraDistance=2;
    if (cameraArc>MathHelper::Pi/5)cameraArc=MathHelper::Pi/5;else if(cameraArc<-(MathHelper::Pi/5))cameraArc=-(MathHelper::Pi/5);
    Vector3 cameraPos(0,cameraDistance,cameraDistance);
    cameraPos=Vector3::Transform(cameraPos,Matrix::CreateRotationX(cameraArc));
    cameraPos=Vector3::Transform(cameraPos,Matrix::CreateRotationY(cameraRotation));
    cameraPos+=world.getTranslationProperty();
    view=Matrix::CreateLookAt(cameraPos,world.getTranslationProperty()+Vector3(0,1.2f,0),Vector3::Up);
    if (avatarRenderer)avatarRenderer->setViewProperty(view);
}
void CustomAvatarAnimationSampleGame::PlayRandomIdle() { PlayAnimation(static_cast<AnimationType>(random.Next(static_cast<int>(AnimationType::Idle4)))); }
void CustomAvatarAnimationSampleGame::PlayAnimation(AnimationType animation) {
    animations[static_cast<int>(animation)]->setCurrentPositionProperty(System::TimeSpan::Zero);currentType=animation;
}
void CustomAvatarAnimationSampleGame::LoadAvatar(Gamer* gamer) {
    UnloadAvatar();
    avatarRequests.emplace_back(AvatarDescription::BeginGetFromGamer(gamer,[this](System::IAsyncResult& result){LoadAvatarDescription(&result);},std::any{}));
}
void CustomAvatarAnimationSampleGame::LoadAvatarDescription(System::IAsyncResult* result) {
    avatarDescription=AvatarDescription::EndGetFromGamer(result);
    if (avatarDescription->getIsValidProperty()) {
        avatarRenderer=std::make_unique<AvatarRenderer>(&*avatarDescription);avatarRenderer->setProjectionProperty(projection);
    } else LoadRandomAvatar();
}
void CustomAvatarAnimationSampleGame::LoadRandomAvatar() {
    UnloadAvatar();avatarDescription=AvatarDescription::CreateRandom();
    avatarRenderer=std::make_unique<AvatarRenderer>(&*avatarDescription);avatarRenderer->setProjectionProperty(projection);
}
void CustomAvatarAnimationSampleGame::UnloadAvatar() {
    if (avatarRenderer) {avatarRenderer->Dispose();avatarRenderer.reset();}
}
}
int main() {
    CustomAvatarAnimationSample::CustomAvatarAnimationSampleGame game;game.Run();return 0;
}
