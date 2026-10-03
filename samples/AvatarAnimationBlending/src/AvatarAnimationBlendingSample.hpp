// SPDX-License-Identifier: MS-PL
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
#pragma once

#include "AvatarBlendedAnimation.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarDescription.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesComponent.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include <array>
#include <optional>

namespace AvatarAnimationBlendingSample
{
    /** @brief Demonstrates smooth transitions between four avatar animations. */
    class AvatarAnimationBlendingGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Sets the original 1280x720, multisampling and Gamer Services settings. */
        AvatarAnimationBlendingGame();
        /** @brief Gets the original logical type name. @return Fully qualified type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads the font, avatar and four preset animations. */
        void LoadContent() override;
        /** @brief Reads input and advances the camera and animation. @param gameTime Update timing. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Draws the blending text and avatar. @param gameTime Frame timing. */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        std::unique_ptr<Microsoft::Xna::Framework::GraphicsDeviceManager> graphics;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> font;
        std::optional<Microsoft::Xna::Framework::GamerServices::AvatarDescription> avatarDescription;
        std::unique_ptr<Microsoft::Xna::Framework::GamerServices::AvatarRenderer> avatarRenderer;
        std::unique_ptr<AvatarBlendedAnimation> blendedAnimation;
        std::array<std::unique_ptr<Microsoft::Xna::Framework::GamerServices::AvatarAnimation>, 4> avatarAnimations;
        Microsoft::Xna::Framework::GamerServices::AvatarAnimation* noBlendingAnimation = nullptr;
        bool useAnimationBlending = true;
        Microsoft::Xna::Framework::Matrix world, view, projection;
        Microsoft::Xna::Framework::Input::GamePadState currentGamePadState, lastGamePadState;
        static constexpr SharpRuntime::Single CameraRotateSpeed = .1f;
        static constexpr SharpRuntime::Single CameraZoomSpeed = .01f;
        static constexpr SharpRuntime::Single CameraMaxDistance = 10.0f;
        static constexpr SharpRuntime::Single CameraMinDistance = 2.0f;
        static constexpr SharpRuntime::Single CameraDefaultArc = 30.0f;
        static constexpr SharpRuntime::Single CameraDefaultRotation = 0;
        static constexpr SharpRuntime::Single CameraDefaultDistance = 3.0f;
        SharpRuntime::Single cameraArc = CameraDefaultArc;
        SharpRuntime::Single cameraRotation = CameraDefaultRotation;
        SharpRuntime::Single cameraDistance = CameraDefaultDistance;
        void HandleInput();
        void UpdateCamera(const Microsoft::Xna::Framework::GameTime& gameTime);
    };
}
