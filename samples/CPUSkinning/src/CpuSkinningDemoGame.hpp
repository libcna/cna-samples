// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CpuSkinningDemoGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CpuSkinningDataTypes/Animation/AnimationPlayer.hpp"
#include "CpuSkinningDataTypes/CpuSkinnedModel.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Input/MouseState.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class IEffectLights;
    class IEffectMatrices;
}

namespace CpuSkinningDemo
{
    /** @brief Demonstrates equivalent GPU and per-frame CPU vertex skinning. */
    class CpuSkinningDemoGame : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game, configures 30 Hz timing, and adds the FPS component. */
        CpuSkinningDemoGame();

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `CpuSkinningDemo.CpuSkinningDemoGame`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads both models and starts the shared `Take 001` animation. */
        void LoadContent() override;

        /**
         * @brief Advances animation and handles touch or mouse camera input.
         * @param gameTime Timing information for the current frame.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Draws either the GPU- or CPU-skinned model and the active-mode label.
         * @param gameTime Timing information for the current frame.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        Microsoft::Xna::Framework::GraphicsDeviceManager graphics;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> font;

        static constexpr const char* gpuSkinningOn = "GPU skinning";
        static constexpr const char* cpuSkinningOn = "CPU skinning";

        std::optional<Microsoft::Xna::Framework::Graphics::Model> gpuDude;
        std::shared_ptr<CpuSkinningDataTypes::CpuSkinnedModel> cpuDude;
        std::unique_ptr<CpuSkinningDataTypes::AnimationPlayer> animationPlayer;

        bool displayCpuModel = false;
        float cameraRotation = 0.0f;
        float cameraArc = 0.0f;

#if !defined(WINDOWS_PHONE)
        Microsoft::Xna::Framework::Input::MouseState mouse;
        Microsoft::Xna::Framework::Input::MouseState mousePrev;
#endif

        /**
         * @brief Applies drag displacement to yaw and pitch.
         * @param delta Pointer or gesture displacement.
         */
        void HandleDrag(const Microsoft::Xna::Framework::Vector2& delta);

        /**
         * @brief Sets the three standard transform properties on an effect interface.
         * @param effect Effect matrix interface.
         * @param world Model-to-world transform.
         * @param view World-to-view transform.
         * @param projection View-to-projection transform.
         */
        static void ConfigureEffectMatrices(
            Microsoft::Xna::Framework::Graphics::IEffectMatrices& effect,
            const Microsoft::Xna::Framework::Matrix& world,
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection);

        /**
         * @brief Enables the sample's single backward-facing default light.
         * @param effect Effect lighting interface.
         */
        static void ConfigureEffectLighting(
            Microsoft::Xna::Framework::Graphics::IEffectLights& effect);
    };
}
