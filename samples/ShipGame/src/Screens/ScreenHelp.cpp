// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ScreenHelp.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ScreenHelp.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/ClearOptions.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/ArgumentNullException.hpp"

#include "../FontManager.hpp"
#include "../GameManager.hpp"
#include "../InputManager.hpp"
#include "ScreenManager.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::ClearOptions;
using Microsoft::Xna::Framework::Input::Keys;

void ScreenHelp::SetFocus(ContentManager* content, bool focus) {
    // if getting focus
    if (focus) {
        // load all resources
        textureControls_ = content->Load<Texture2D>("screens/controls");
        textureDisplay_ = content->Load<Texture2D>("screens/controls_display");
        textureContinue_ = content->Load<Texture2D>("screens/continue");
    } else // loosing focus
    {
        // free all resources
        textureControls_.reset();
        textureDisplay_.reset();
        textureContinue_.reset();
    }
}

void ScreenHelp::ProcessInput(float elapsedTime, InputManager* input) {
    (void)elapsedTime;

    if (input == nullptr) {
        throw System::ArgumentNullException("input");
    }

    for (int i = 0; i < 2; i++) {
        // Any key/button to go back
        if (input->IsButtonPressedA(i) || input->IsButtonPressedB(i) ||
            input->IsButtonPressedX(i) || input->IsButtonPressedY(i) ||
            input->IsButtonPressedLeftShoulder(i) || input->IsButtonPressedRightShoulder(i) ||
            input->IsButtonPressedLeftStick(i) || input->IsButtonPressedRightStick(i) ||
            input->IsButtonPressedBack(i) || input->IsButtonPressedStart(i) ||
            input->IsKeyPressed(i, Keys::Enter) || input->IsKeyPressed(i, Keys::Escape) ||
            input->IsKeyPressed(i, Keys::Space)) {
            screenManager_->SetNextScreen(ScreenType::ScreenIntro);
            gameManager_->PlaySound("menu_cancel");
        }
    }
}

void ScreenHelp::Update(float elapsedTime) { (void)elapsedTime; }

void ScreenHelp::Draw3D(GraphicsDevice* gd) {
    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    // clear background
    gd->Clear(ClearOptions::Target | ClearOptions::DepthBuffer, Color::Black, 1, 0);

    // draw background animation
    screenManager_->DrawBackground(gd);
}

void ScreenHelp::Draw2D(GraphicsDevice* gd, FontManager* font) {
    (void)font;

    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    Rectangle rect(0, 0, 0, 0);

    int screenSizeX = gd->getViewportProperty().getWidthProperty();
    int screenSizeY = gd->getViewportProperty().getHeightProperty();

    // draw controlls text aligned to top of screen
    rect.Width = textureControls_->getWidthProperty();
    rect.Height = textureControls_->getHeightProperty();
    rect.X = screenSizeX / 2 - rect.Width / 2;
    rect.Y = 40;
    screenManager_->DrawTexture(&textureControls_.value(), rect, Color::White,
                                BlendState::AlphaBlend);

    // draw controller texture centered in screen
    rect.Width = textureDisplay_->getWidthProperty();
    rect.Height = textureDisplay_->getHeightProperty();
    rect.X = screenSizeX / 2 - rect.Width / 2;
    rect.Y = screenSizeY / 2 - rect.Height / 2 + 10;
    screenManager_->DrawTexture(&textureDisplay_.value(), rect, Color::White,
                                BlendState::AlphaBlend);

    // draw continue message aligned to bottom of screen
    rect.Width = textureContinue_->getWidthProperty();
    rect.Height = textureContinue_->getHeightProperty();
    rect.X = screenSizeX / 2 - rect.Width / 2;
    rect.Y = screenSizeY - rect.Height - 60;
    screenManager_->DrawTexture(&textureContinue_.value(), rect, Color::White,
                                BlendState::AlphaBlend);
}

} // namespace ShipGame
