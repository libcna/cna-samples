// SPDX-License-Identifier: MS-PL
// Copyright (C) Microsoft Corporation. All rights reserved.
#pragma once
#include "CustomAvatarAnimation/CustomAvatarAnimationPlayer.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarAnimation.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarDescription.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInEventArgs.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "System/Random.hpp"
#include <array>
#include <optional>
namespace CustomAvatarAnimationSample {
/** @brief All nine loaded animations, including the fourth idle excluded by original random selection. */
enum class AnimationType { Idle1, Idle2, Idle3, Idle4, Walk, Jump, Kick, Punch, Faint };
/** @brief Demonstrates custom FBX animation and expression playback on an avatar. */
class CustomAvatarAnimationSampleGame : public Microsoft::Xna::Framework::Game {
public:
    /** @brief Creates the graphics, Gamer Services and sign-in subscription. */
    CustomAvatarAnimationSampleGame();
    /** @brief Releases the C++ sign-in subscription after the title stops. */
    ~CustomAvatarAnimationSampleGame() override;
    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;
protected:
    /** @brief Loads four idles, five custom clips and the ground. */
    void LoadContent() override;
    /** @brief Handles input and advances animation. @param gameTime Timing. */
    void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
    /** @brief Draws ground before the avatar. @param gameTime Timing. */
    void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;
private:
    std::unique_ptr<Microsoft::Xna::Framework::GraphicsDeviceManager> graphics;
    std::optional<Microsoft::Xna::Framework::Graphics::Model> groundModel;
    Microsoft::Xna::Framework::Matrix world, view, projection;
    static constexpr float CameraDefaultArc = 3.14159265358979323846f / 10;
    static constexpr float CameraDefaultRotation = 3.14159265358979323846f;
    static constexpr float CameraDefaultDistance = 2.5f;
    float cameraArc = CameraDefaultArc, cameraRotation = CameraDefaultRotation, cameraDistance = CameraDefaultDistance;
    std::unique_ptr<Microsoft::Xna::Framework::GamerServices::AvatarRenderer> avatarRenderer;
    std::optional<Microsoft::Xna::Framework::GamerServices::AvatarDescription> avatarDescription;
    std::array<std::unique_ptr<Microsoft::Xna::Framework::GamerServices::IAvatarAnimation>,9> animations;
    AnimationType currentType = AnimationType::Idle1;
    Microsoft::Xna::Framework::Input::GamePadState currentGamePadState, lastGamePadState;
    System::Random random;
    System::EventHandler<Microsoft::Xna::Framework::GamerServices::SignedInEventArgs>::Token signedInToken;
    std::vector<std::unique_ptr<System::IAsyncResult>> avatarRequests;
    void SignedInGamer_SignedIn(System::Object* sender, const Microsoft::Xna::Framework::GamerServices::SignedInEventArgs& e);
    void HandleAvatarInput(const Microsoft::Xna::Framework::GameTime& gameTime);
    void UpdateAvatarMovement(const Microsoft::Xna::Framework::GameTime& gameTime);
    void HandleCameraInput();
    void PlayRandomIdle();
    void PlayAnimation(AnimationType animation);
    void LoadAvatar(Microsoft::Xna::Framework::GamerServices::Gamer* gamer);
    void LoadAvatarDescription(System::IAsyncResult* result);
    void LoadRandomAvatar();
    void UnloadAvatar();
};
}
