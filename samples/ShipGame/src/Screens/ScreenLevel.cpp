// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ScreenLevel.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ScreenLevel.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
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
using Microsoft::Xna::Framework::Input::Keys;

void ScreenLevel::SetFocus(ContentManager* content, bool focus) {
    // if getting focus
    if (focus) {
        // load all resources
        for (int i = 0; i < NumberLevels; i++)
            levelShots_[i] = content->Load<Texture2D>("screens/" + levels_[i] + "_screen");
        selectBack_ = content->Load<Texture2D>("screens/select_back");
        changeLevel_ = content->Load<Texture2D>("screens/change_level");
    } else // loosing focus
    {
        // free all resources
        for (int i = 0; i < NumberLevels; i++)
            levelShots_[i].reset();
        selectBack_.reset();
        changeLevel_.reset();
    }
}

void ScreenLevel::ProcessInput(float elapsedTime, InputManager* input) {
    (void)elapsedTime;

    if (input == nullptr) {
        throw System::ArgumentNullException("input");
    }

    int i, j = (int)gameManager_->getGameMode();
    for (i = 0; i < j; i++) {
        // select
        if (input->IsKeyPressed(i, Keys::Enter) || input->IsButtonPressedA(i)) {
            gameManager_->SetLevel(levels_[selection_]);
            screenManager_->SetNextScreen(ScreenType::ScreenGame);
            gameManager_->PlaySound("menu_select");
        }

        // cancel
        if (input->IsKeyPressed(i, Keys::Escape) || input->IsButtonPressedB(i)) {
            gameManager_->SetLevel("");
            screenManager_->SetNextScreen(ScreenType::ScreenPlayer);
            gameManager_->PlaySound("menu_cancel");
        }

        // change selection (previous)
        if (input->IsKeyPressed(i, Keys::Left) || input->IsButtonPressedDPadLeft(i) ||
            input->IsButtonPressedLeftStickLeft(i)) {
            if (selection_ == 0)
                selection_ = (int)levels_.size() - 1;
            else
                selection_ = selection_ - 1;
            gameManager_->PlaySound("menu_change");
        }

        // change selection (next)
        if (input->IsKeyPressed(i, Keys::Right) || input->IsButtonPressedDPadRight(i) ||
            input->IsButtonPressedLeftStickRight(i)) {
            selection_ = (selection_ + 1) % (int)levels_.size();
            gameManager_->PlaySound("menu_change");
        }
    }
}

void ScreenLevel::Update(float elapsedTime) { (void)elapsedTime; }

void ScreenLevel::Draw3D(GraphicsDevice* gd) {
    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    // clear background
    gd->Clear(Color::Black);

    // draw background animation
    screenManager_->DrawBackground(gd);
}

void ScreenLevel::Draw2D(GraphicsDevice* gd, FontManager* font) {
    (void)font;

    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    int screenSizeX = gd->getViewportProperty().getWidthProperty();
    int screenSizeY = gd->getViewportProperty().getHeightProperty();

    Rectangle rect(0, 0, 0, 0);

    // draw level screen shot
    rect.Width = levelShots_[selection_]->getWidthProperty();
    rect.Height = levelShots_[selection_]->getHeightProperty();
    rect.X = (screenSizeX - rect.Width) / 2;
    rect.Y = (screenSizeY - rect.Height) / 2 + 30;
    screenManager_->DrawTexture(&levelShots_[selection_].value(), rect, Color::White,
                                BlendState::AlphaBlend);

    // draw back and select buttons
    rect.Width = selectBack_->getWidthProperty();
    rect.Height = selectBack_->getHeightProperty();
    rect.X = (screenSizeX - rect.Width) / 2;
    rect.Y = 30;
    screenManager_->DrawTexture(&selectBack_.value(), rect, Color::White,
                                BlendState::AlphaBlend);

    // draw change level text
    rect.Width = changeLevel_->getWidthProperty();
    rect.Height = changeLevel_->getHeightProperty();
    rect.X = (screenSizeX - rect.Width) / 2;
    rect.Y = screenSizeY - rect.Height - 30;
    screenManager_->DrawTexture(&changeLevel_.value(), rect, Color::White,
                                BlendState::AlphaBlend);
}

} // namespace ShipGame
