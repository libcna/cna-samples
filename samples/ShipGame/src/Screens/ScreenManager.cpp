// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ScreenManager.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ScreenManager.hpp"

#include <cmath>
#include <memory>

#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/ArgumentNullException.hpp"

#include "../FontManager.hpp"
#include "../GameManager.hpp"
#include "../GameOptions.hpp"
#include "../Graphics/BlurManager.hpp"
#include "../InputManager.hpp"
#include "../ShipGame.hpp"
#include "ScreenEnd.hpp"
#include "ScreenGame.hpp"
#include "ScreenHelp.hpp"
#include "ScreenIntro.hpp"
#include "ScreenLevel.hpp"
#include "ScreenPlayer.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Graphics::DepthFormat;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::Effect;
using Microsoft::Xna::Framework::Graphics::SurfaceFormat;
using Microsoft::Xna::Framework::Input::Keys;

ScreenManager::ScreenManager(ShipGameGame* shipGame, FontManager* font, GameManager* game) {
    this->shipGame_ = shipGame;
    gameManager_ = game;
    fontManager_ = font;

    inputManager_ = std::make_unique<InputManager>();

    // add all screens
    screens_.push_back(std::make_unique<ScreenIntro>(this, game));
    screens_.push_back(std::make_unique<ScreenHelp>(this, game));
    screens_.push_back(std::make_unique<ScreenPlayer>(this, game));
    screens_.push_back(std::make_unique<ScreenLevel>(this, game));
    screens_.push_back(std::make_unique<ScreenGame>(this, game));
    screens_.push_back(std::make_unique<ScreenEnd>(this, game));

    // fade in to intro screen
    SetNextScreen(ScreenType::ScreenIntro, GameOptions::FadeColor, GameOptions::FadeTime);
    fade_ = fadeTime_ * 0.5f;
}

ScreenManager::~ScreenManager() { Dispose(true); }

void ScreenManager::ProcessInput(float elapsedTime) {
    inputManager_->BeginInputProcessing(gameManager_->getGameMode() == GameMode::SinglePlayer);

    // process input for currently active screen
    if (current_ != nullptr && next_ == nullptr)
        current_->ProcessInput(elapsedTime, inputManager_.get());

    // toggle full screen with F5 key
    if (inputManager_->IsKeyPressed(0, Keys::F5) || inputManager_->IsKeyPressed(1, Keys::F5))
        shipGame_->ToggleFullScreen();

    inputManager_->EndInputProcessing();
}

void ScreenManager::Update(float elapsedTime) {
    // if in a transition
    if (fade_ > 0) {
        // update transition time
        fade_ -= elapsedTime;

        // if time to switch to new screen (fade out finished)
        if (next_ != nullptr && fade_ < 0.5f * fadeTime_) {
            // tell new screen it is getting in focus
            next_->SetFocus(contentManager_, true);

            // tell the old screen it lost its focus
            if (current_ != nullptr)
                current_->SetFocus(contentManager_, false);

            // set new screen as current
            current_ = next_;
            next_ = nullptr;
        }
    }

    // if current screen available, update it
    if (current_ != nullptr)
        current_->Update(elapsedTime);

    // calulate frame rate
    frameRateTime_ += elapsedTime;
    if (frameRateTime_ > 0.5f) {
        frameRate_ = (int)((float)frameRateCount_ / frameRateTime_);
        frameRateCount_ = 0;
        frameRateTime_ = 0;
    }

    // accumulate elapsed time for background animation
    backgroundTime_ += elapsedTime;
}

void ScreenManager::BlurGlowRenterTarget(GraphicsDevice* gd) {
    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    gd->setDepthStencilStateProperty(DepthStencilState::None);

    // if in game screen and split screen mode
    if (current_ == getScreenGame() && gameManager_->getGameMode() == GameMode::MultiPlayer) {
        // blur horizontal with split horizontal blur shader
        gd->SetRenderTarget(&glowRT1_.value());
        blurManager_->RenderScreenQuad(gd, BlurTechnique::BlurHorizontalSplit, &colorRT_.value(),
                                       Vector4::One);
    } else {
        // blur horizontal with regular horizontal blur shader
        gd->SetRenderTarget(&glowRT1_.value());
        blurManager_->RenderScreenQuad(gd, BlurTechnique::BlurHorizontal, &colorRT_.value(),
                                       Vector4::One);
    }

    // blur vertical with regular vertical blur shader
    gd->SetRenderTarget(&glowRT2_.value());
    blurManager_->RenderScreenQuad(gd, BlurTechnique::BlurVertical, &glowRT1_.value(),
                                   Vector4::One);

    gd->setDepthStencilStateProperty(DepthStencilState::Default);

    gd->SetRenderTarget(nullptr);
}

void ScreenManager::DrawRenderTargetTexture(GraphicsDevice* gd, RenderTarget2D* renderTarget,
                                            float intensity, bool additiveBlend) {
    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    // set up render state and blend mode
    gd->setDepthStencilStateProperty(DepthStencilState::None);
    if (additiveBlend) {
        gd->setBlendStateProperty(BlendState::Additive);
    }

    // draw render tareget as fullscreen texture
    blurManager_->RenderScreenQuad(gd, BlurTechnique::ColorTexture, renderTarget,
                                   Vector4(intensity));

    // restore render state and blend mode
    gd->setDepthStencilStateProperty(DepthStencilState::Default);
}

void ScreenManager::DrawTexture(Texture2D* texture, Rectangle rect, Color color,
                                BlendState blend) {
    fontManager_->DrawTexture(texture, rect, color, blend);
}

void ScreenManager::DrawTexture(Texture2D* texture, Rectangle destinationRect,
                                Rectangle sourceRect, Color color, BlendState blend) {
    fontManager_->DrawTexture(texture, destinationRect, sourceRect, color, blend);
}

void ScreenManager::DrawTexture(Texture2D* texture, Rectangle rect, float rotation, Color color,
                                BlendState blend) {
    fontManager_->DrawTexture(texture, rect, rotation, color, blend);
}

void ScreenManager::DrawBackground(GraphicsDevice* gd) {
    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    const float animationTime = 3.0f;
    const float animationLength = 0.4f;
    const int numberLayers = 2;
    const float layerDistance = 1.0f / numberLayers;

    // normalized time
    float normalizedTime = std::fmod(backgroundTime_ / animationTime, 1.0f);

    // set render states
    DepthStencilState ds = gd->getDepthStencilStateProperty();
    BlendState bs = gd->getBlendStateProperty();
    gd->setDepthStencilStateProperty(DepthStencilState::DepthRead);
    gd->setBlendStateProperty(BlendState::AlphaBlend);

    float scale;
    Vector4 color;

    // render all background layers
    for (int i = 0; i < numberLayers; i++) {
        if (normalizedTime > 0.5f)
            scale = 2 - normalizedTime * 2;
        else
            scale = normalizedTime * 2;
        color = Vector4(scale, scale, scale, 0);

        scale = 1 + normalizedTime * animationLength;

        blurManager_->RenderScreenQuad(gd, BlurTechnique::ColorTexture,
                                       &textureBackground_.value(), color, scale);

        normalizedTime = std::fmod(normalizedTime + layerDistance, 1.0f);
    }

    // restore render states
    gd->setDepthStencilStateProperty(ds);
    gd->setBlendStateProperty(bs);
}

void ScreenManager::Draw(GraphicsDevice* gd) {
    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    frameRateCount_++;

    // if a valid current screen is set
    if (current_ != nullptr) {
        // set the color render target
        gd->SetRenderTarget(&colorRT_.value());

        // draw the screen 3D scene
        current_->Draw3D(gd);

        // resolve the color render target
        gd->SetRenderTarget(nullptr);

        // blur the glow render target
        BlurGlowRenterTarget(gd);

        // draw the 3D scene texture
        DrawRenderTargetTexture(gd, &colorRT_.value(), 1.0f, false);

        // draw the glow texture with additive blending
        DrawRenderTargetTexture(gd, &glowRT2_.value(), 2.0f, true);

        // begin text mode
        fontManager_->BeginText();

        // draw the 2D scene
        current_->Draw2D(gd, fontManager_);

        // draw fps
        // fontManager.DrawText(
        //     FontType.ArialSmall,
        //     "FPS: " + frameRate,
        //     new Vector2(gd.Viewport.Width - 80, 0), Color.White);

        // end text mode
        fontManager_->EndText();
    }

    // if in a transition
    if (fade_ > 0) {
        // compute transtition fade intensity
        float size = fadeTime_ * 0.5f;
        fadeColor_.W = 1.25f * (1.0f - std::abs(fade_ - size) / size);

        // set alpha blend and no depth test or write
        gd->setDepthStencilStateProperty(DepthStencilState::None);
        gd->setBlendStateProperty(BlendState::AlphaBlend);

        // draw transition fade color
        blurManager_->RenderScreenQuad(gd, BlurTechnique::Color, nullptr, fadeColor_);

        // restore render states
        gd->setDepthStencilStateProperty(DepthStencilState::Default);
        gd->setBlendStateProperty(BlendState::Opaque);
    }
}

void ScreenManager::LoadContent(GraphicsDevice* gd, ContentManager& content) {
    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    contentManager_ = &content;
    textureBackground_ = content.Load<Texture2D>("screens/intro_bg");
    // create blur manager
    blurManager_ = std::make_unique<BlurManager>(gd, content.Load<std::shared_ptr<Effect>>(
                                                         "shaders/Blur"),
                                                 GameOptions::GlowResolution,
                                                 GameOptions::GlowResolution);

    int width = gd->getViewportProperty().getWidthProperty();
    int height = gd->getViewportProperty().getHeightProperty();

    // create render targets
    colorRT_.emplace(*gd, width, height, true, SurfaceFormat::Color, DepthFormat::Depth24);
    glowRT1_.emplace(*gd, GameOptions::GlowResolution, GameOptions::GlowResolution, true,
                     SurfaceFormat::Color, DepthFormat::Depth24);
    glowRT2_.emplace(*gd, GameOptions::GlowResolution, GameOptions::GlowResolution, true,
                     SurfaceFormat::Color, DepthFormat::Depth24);
}

void ScreenManager::UnloadContent() {
    textureBackground_.reset();
    blurManager_ = nullptr;

    colorRT_.reset();
    glowRT1_.reset();
    glowRT2_.reset();
}

bool ScreenManager::SetNextScreen(ScreenType screenType, Vector4 fadeColor, float fadeTime) {
    // if no transition already happening
    if (next_ == nullptr) {
        // set next screen and transition options
        next_ = screens_[(int)screenType].get();
        this->fadeTime_ = fadeTime;
        this->fadeColor_ = fadeColor;
        this->fade_ = this->fadeTime_;
        return true;
    }
    return false;
}

bool ScreenManager::SetNextScreen(ScreenType screenType, Vector4 fadeColor) {
    return SetNextScreen(screenType, fadeColor, 1.0f);
}

bool ScreenManager::SetNextScreen(ScreenType screenType) {
    return SetNextScreen(screenType, Vector4::Zero, 1.0f);
}

Screen* ScreenManager::GetScreen(ScreenType screenType) {
    return screens_[(int)screenType].get();
}

ScreenIntro* ScreenManager::getScreenIntro() {
    return static_cast<ScreenIntro*>(screens_[(int)ScreenType::ScreenIntro].get());
}

ScreenIntro* ScreenManager::getScreenHelp() {
    return dynamic_cast<ScreenIntro*>(screens_[(int)ScreenType::ScreenHelp].get());
}

ScreenPlayer* ScreenManager::getScreenPlayer() {
    return static_cast<ScreenPlayer*>(screens_[(int)ScreenType::ScreenPlayer].get());
}

ScreenLevel* ScreenManager::getScreenLevel() {
    return static_cast<ScreenLevel*>(screens_[(int)ScreenType::ScreenLevel].get());
}

ScreenGame* ScreenManager::getScreenGame() {
    return static_cast<ScreenGame*>(screens_[(int)ScreenType::ScreenGame].get());
}

ScreenEnd* ScreenManager::getScreenEnd() {
    return static_cast<ScreenEnd*>(screens_[(int)ScreenType::ScreenEnd].get());
}

void ScreenManager::Exit() { shipGame_->Exit(); }

void ScreenManager::Dispose() { Dispose(true); }

void ScreenManager::Dispose(bool disposing) {
    if (disposing && !isDisposed_) {
        UnloadContent();
    }
}

} // namespace ShipGame
