// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SoccerPitchGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/AlphaTestEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DualTextureEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"

#include "Primitives/PlanePrimitiveDualTextured.hpp"
#include "Primitives/PlanePrimitiveTextured.hpp"
#include "Primitives/SpherePrimitiveTextured.hpp"

namespace SoccerPitch
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using Microsoft::Xna::Framework::Input::Touch::TouchCollection;

    /**
     * @brief Demonstrates DualTextureEffect detail texturing and multipass pitch-line rendering.
     */
    class SoccerPitchGame final : public Game
    {
    public:
        /** @brief Creates the game, frame-rate component, and 480-by-800 presentation settings. */
        SoccerPitchGame();

        /**
         * @brief Returns the fully-qualified .NET type name.
         *
         * @return Fully-qualified type name.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads content, creates effects and primitives, and initializes camera matrices. */
        void LoadContent() override;

        /**
         * @brief Handles exit and touch input before updating game components.
         *
         * @param gameTime Snapshot of elapsed and total game time.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief Draws the textured pitch, stripe pass, ball, shadow, and status text.
         *
         * @param gameTime Snapshot of elapsed and total game time.
         */
        void Draw(const GameTime& gameTime) override;

    private:
        void HandleTouchInput();

        static constexpr float FarClip = 150.0f;
        static constexpr float PlaneSize = 100.0f;
        static constexpr float PlaneTiling = 30.0f;
        static constexpr float SoccerBallDiameter = 2.0f;
        static constexpr float SoccerBallRadius = SoccerBallDiameter * 0.5f;
        static constexpr float SoccerBallDepthOffset = 0.0001f;

        GraphicsDeviceManager graphics_;
        std::unique_ptr<SpriteBatch> spriteBatch_;
        std::optional<SpriteFont> spriteFont_;

        Color transparentWhite_;
        bool useAlphaBlend_ = true;
        static constexpr const char* AlphaTestText = "Alpha-Test\n";
        static constexpr const char* AlphaBlendText = "Alpha-Blend\n";

        std::unique_ptr<PlanePrimitiveDualTextured> pitchPrimitive_;
        std::unique_ptr<PlanePrimitiveTextured> pitchStripePrimitive_;
        std::unique_ptr<SpherePrimitiveTextured> spherePrimitive_;

        std::optional<Texture2D> pitchBaseTexture_;
        std::optional<Texture2D> pitchDetailTexture_;
        std::optional<Texture2D> pitchStripeTexture_;
        std::optional<Texture2D> soccerBallTexture_;

        std::unique_ptr<DualTextureEffect> pitchDualTextureEffect_;
        std::unique_ptr<BasicEffect> pitchBasicEffect_;
        std::unique_ptr<AlphaTestEffect> pitchStripeEffect_;

        RasterizerState shadowRasterizerState_;

        Matrix view_;
        Vector3 eyeAtStart_;
        Vector3 eyeAtBall_;
        Vector3 camera_;
        Matrix projection_;
        Matrix shadowMatrix_;

        TouchCollection currentTouches_;
    };
}
