// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SoccerPitchGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "SoccerPitchGame.hpp"

#include <algorithm>
#include <cmath>

#include "FrameRateCounter.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocationState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/GC.hpp"

#if defined(WINDOWS_PHONE)
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#endif

namespace SoccerPitch
{
    using FrameRateCounterComponent::FrameRateCounter;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Touch::TouchLocationState;
    using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

    SoccerPitchGame::SoccerPitchGame()
        : graphics_(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");
        getComponentsProperty().Add(new FrameRateCounter(*this));

        setIsFixedTimeStepProperty(false);
        graphics_.setSynchronizeWithVerticalRetraceProperty(false);
        graphics_.setPreferredBackBufferWidthProperty(480);
        graphics_.setPreferredBackBufferHeightProperty(800);

#if defined(WINDOWS_PHONE)
        using Microsoft::Xna::Framework::GamerServices::Guide;
        graphics_.setIsFullScreenProperty(true);
        Guide::setIsScreenSaverEnabledProperty(false);
#endif
    }

    const std::string& SoccerPitchGame::GetTypeName() const
    {
        static const std::string name = "SoccerPitch.SoccerPitchGame";
        return name;
    }

    void SoccerPitchGame::LoadContent()
    {
        GraphicsDevice& graphicsDevice = getGraphicsDeviceProperty();
        spriteBatch_ = std::make_unique<SpriteBatch>(graphicsDevice);
        spriteFont_.emplace(getContentProperty().Load<SpriteFont>("Font"));

        pitchBasicEffect_ = std::make_unique<BasicEffect>(graphicsDevice);
        pitchBasicEffect_->setLightingEnabledProperty(false);
        pitchBasicEffect_->setPreferPerPixelLightingProperty(false);
        pitchBasicEffect_->setFogEnabledProperty(false);
        pitchBasicEffect_->setVertexColorEnabledProperty(false);

        const Vector2 tiling1(PlaneTiling / 3.0f, PlaneTiling / 3.0f);
        const Vector2 tiling2(PlaneTiling, PlaneTiling);

        pitchPrimitive_ = std::make_unique<PlanePrimitiveDualTextured>(
            graphicsDevice, PlaneSize, tiling1, tiling2);
        pitchStripePrimitive_ =
            std::make_unique<PlanePrimitiveTextured>(graphicsDevice, PlaneSize);
        pitchDualTextureEffect_ = std::make_unique<DualTextureEffect>(graphicsDevice);
        pitchStripeEffect_ = std::make_unique<AlphaTestEffect>(graphicsDevice);
        spherePrimitive_ = std::make_unique<SpherePrimitiveTextured>(
            graphicsDevice, SoccerBallDiameter, 6);

        pitchBaseTexture_.emplace(getContentProperty().Load<Texture2D>("Base"));
        pitchDetailTexture_.emplace(getContentProperty().Load<Texture2D>("Detail"));
        pitchStripeTexture_.emplace(getContentProperty().Load<Texture2D>("Stripe2"));
        soccerBallTexture_.emplace(getContentProperty().Load<Texture2D>("Soccerball"));

        eyeAtStart_ = Vector3(0.0f, PlaneSize / 10.0f, -PlaneSize * 0.75f);
        eyeAtBall_ =
            Vector3(0.0f, 3.0f * SoccerBallDiameter, -SoccerBallDiameter * 0.75f);

        transparentWhite_ = Color::White;
        transparentWhite_.setAProperty(250);

        shadowRasterizerState_.setDepthBiasProperty(-SoccerBallDepthOffset);

        const float aspect = graphicsDevice.getViewportProperty().getAspectRatioProperty();
        projection_ = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::Pi / 4.0f, aspect, 2.0f, FarClip);

        System::GC::Collect();
    }

    void SoccerPitchGame::HandleTouchInput()
    {
        currentTouches_ = TouchPanel::GetState();
        for (const auto& location : currentTouches_)
        {
            if (location.getStateProperty() == TouchLocationState::Released)
            {
                useAlphaBlend_ = !useAlphaBlend_;
            }
        }
    }

    void SoccerPitchGame::Update(GameTime& gameTime)
    {
        if (GamePad::GetState(PlayerIndex::One)
                .getButtonsProperty()
                .getBackProperty() == ButtonState::Pressed)
        {
            Exit();
        }

        HandleTouchInput();
        Game::Update(gameTime);
    }

    void SoccerPitchGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& graphicsDevice = getGraphicsDeviceProperty();
        graphicsDevice.Clear(Color::CornflowerBlue);

        const float time =
            static_cast<float>(gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());
        Matrix primitiveOrientation = Matrix::CreateRotationY(time * 0.2f);
        const float t = std::max(
            0.1f, static_cast<float>(std::sin(static_cast<double>(time) * 0.1)));
        camera_ = Vector3::Lerp(eyeAtStart_, eyeAtBall_, t);
        view_ = Matrix::CreateLookAt(camera_, Vector3::Zero, Vector3::Up);

        pitchDualTextureEffect_->setTextureProperty(&*pitchBaseTexture_);
        pitchDualTextureEffect_->setTexture2Property(&*pitchDetailTexture_);
        pitchDualTextureEffect_->setVertexColorEnabledProperty(false);
        graphicsDevice.getSamplerStatesProperty()[0] = SamplerState::LinearWrap;
        graphicsDevice.getSamplerStatesProperty()[1] = SamplerState::LinearWrap;
        pitchPrimitive_->DrawDualTextured(
            *pitchDualTextureEffect_,
            primitiveOrientation,
            view_,
            projection_,
            Color::White);

        if (useAlphaBlend_)
        {
            pitchBasicEffect_->setTextureProperty(&*pitchStripeTexture_);
            pitchBasicEffect_->setTextureEnabledProperty(true);
            pitchBasicEffect_->setLightingEnabledProperty(false);
            pitchStripePrimitive_->Draw(
                *pitchBasicEffect_,
                primitiveOrientation,
                view_,
                projection_,
                transparentWhite_);
        }
        else
        {
            pitchStripeEffect_->setTextureProperty(&*pitchStripeTexture_);
            pitchStripePrimitive_->DrawAlphaTest(
                *pitchStripeEffect_,
                primitiveOrientation,
                view_,
                projection_,
                Color::White);
        }

        shadowMatrix_ = Matrix::getIdentityProperty();
        shadowMatrix_.M12 = 0.0f;
        shadowMatrix_.M22 = 0.0f;
        shadowMatrix_.M23 = 0.0f;
        shadowMatrix_ = primitiveOrientation * shadowMatrix_;
        shadowMatrix_.M42 = 0.0f;

        pitchBasicEffect_->setTextureEnabledProperty(false);
        pitchBasicEffect_->setLightingEnabledProperty(false);
        const RasterizerState oldRasterizerState =
            graphicsDevice.getRasterizerStateProperty();
        graphicsDevice.setRasterizerStateProperty(shadowRasterizerState_);
        spherePrimitive_->Draw(
            *pitchBasicEffect_, shadowMatrix_, view_, projection_, Color::Black);
        graphicsDevice.setRasterizerStateProperty(oldRasterizerState);

        primitiveOrientation.M42 -= -SoccerBallRadius;
        pitchBasicEffect_->setTextureProperty(&*soccerBallTexture_);
        pitchBasicEffect_->setTextureEnabledProperty(true);
        pitchBasicEffect_->EnableDefaultLighting();
        spherePrimitive_->Draw(
            *pitchBasicEffect_, primitiveOrientation, view_, projection_, Color::White);

        spriteBatch_->Begin();
        if (useAlphaBlend_)
        {
            spriteBatch_->DrawString(
                *spriteFont_, AlphaBlendText, Vector2(320.0f, 70.0f), Color::White);
        }
        else
        {
            spriteBatch_->DrawString(
                *spriteFont_, AlphaTestText, Vector2(320.0f, 70.0f), Color::White);
        }
        spriteBatch_->End();

        Game::Draw(gameTime);
    }
}
