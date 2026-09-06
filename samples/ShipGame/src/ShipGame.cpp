// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ShipGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ShipGame.hpp"

#include "FontManager.hpp"
#include "GameManager.hpp"
#include "GameOptions.hpp"
#include "Screens/ScreenManager.hpp"

namespace ShipGame {

ShipGameGame* ShipGameGame::instance_ = nullptr;

ShipGameGame::ShipGameGame() : graphics_(this) {
    getContentProperty().setRootDirectoryProperty("Content");

    audioEngine_.emplace("content/sounds/sounds.xgs");
    waveBank_.emplace(&audioEngine_.value(), "content/sounds/Wave Bank.xwb");
    soundBank_.emplace(&audioEngine_.value(), "content/sounds/Sound Bank.xsb");

    game_ = std::make_unique<GameManager>(&soundBank_.value());

    graphics_.setPreferredBackBufferWidthProperty(GameOptions::ScreenWidth);
    graphics_.setPreferredBackBufferHeightProperty(GameOptions::ScreenHeight);

    // graphics.MinimumPixelShaderProfile = ShaderProfile.PS_2_0;
    // graphics.MinimumVertexShaderProfile = ShaderProfile.VS_1_1;

    setIsFixedTimeStepProperty(renderVsync_);
    graphics_.setSynchronizeWithVerticalRetraceProperty(renderVsync_);
}

ShipGameGame::~ShipGameGame() = default;

void ShipGameGame::Initialize() { Game::Initialize(); }

void ShipGameGame::LoadContent() {
    font_ = std::make_unique<FontManager>(&getGraphicsDeviceProperty());
    screen_ = std::make_unique<ScreenManager>(this, font_.get(), game_.get());

    font_->LoadContent(getContentProperty());
    game_->LoadContent(&getGraphicsDeviceProperty(), getContentProperty());
    screen_->LoadContent(&getGraphicsDeviceProperty(), getContentProperty());
}

void ShipGameGame::UnloadContent() {
    screen_->UnloadContent();
    game_->UnloadContent();
    font_->UnloadContent();

    screen_ = nullptr;
    font_ = nullptr;
}

void ShipGameGame::Update(GameTime& gameTime) {
    float ElapsedTimeFloat =
        (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();

    screen_->ProcessInput(ElapsedTimeFloat);
    screen_->Update(ElapsedTimeFloat);

    Game::Update(gameTime);
}

void ShipGameGame::Draw(const GameTime& gameTime) {
    screen_->Draw(&getGraphicsDeviceProperty());

    Game::Draw(gameTime);
}

void ShipGameGame::ToggleFullScreen() { graphics_.ToggleFullScreen(); }

int ShipGameGame::Main() {
    ShipGameGame game;
    {
        instance_ = &game;
        game.Run();
    }
    return 0;
}

ShipGameGame* ShipGameGame::GetInstance() { return instance_; }

} // namespace ShipGame
