// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ScreenIntro.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ScreenIntro.hpp"

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

void ScreenIntro::SetFocus(ContentManager* content, bool focus) {
    // if getting focus
    if (focus) {
        // load all resources
        gameManager_->setGameMode(GameMode::SinglePlayer);

        textureLogo_ = content->Load<Texture2D>("screens/intro_logo");
        textureLens_ = content->Load<Texture2D>("screens/intro_lens");

        textureCursorAnim_ = content->Load<Texture2D>("screens/cursor_anim");
        textureCursorArrow_ = content->Load<Texture2D>("screens/cursor_arrow");
        textureCursorBullet_ = content->Load<Texture2D>("screens/cursor_bullet");

        for (int i = 0; i < NumberMenuItems; i++) {
            textureMenu_[i] = content->Load<Texture2D>("screens/" + menuNames_[i]);
            textureMenuHover_[i] = content->Load<Texture2D>("screens/" + menuNames_[i] + "_hover");
        }
    } else // loosing focus
    {
        // free all resources
        textureLogo_.reset();
        textureLens_.reset();
        textureCursorAnim_.reset();
        textureCursorArrow_.reset();
        textureCursorBullet_.reset();

        for (int i = 0; i < NumberMenuItems; i++) {
            textureMenu_[i].reset();
            textureMenuHover_[i].reset();
        }
    }
}

void ScreenIntro::ProcessInput(float elapsedTime, InputManager* input) {
    (void)elapsedTime;

    if (input == nullptr) {
        throw System::ArgumentNullException("input");
    }

    for (int i = 0; i < 2; i++) {
        // A button or enter to select menu option
        if (input->IsButtonPressedA(i) || input->IsButtonPressedStart(i) ||
            input->IsKeyPressed(i, Keys::Enter) || input->IsKeyPressed(i, Keys::Space)) {
            switch (menuSelection_) {
            case 0:
                // single player
                gameManager_->setGameMode(GameMode::SinglePlayer);
                screenManager_->SetNextScreen(ScreenType::ScreenPlayer);
                break;
            case 1:
                // multi player
                gameManager_->setGameMode(GameMode::MultiPlayer);
                screenManager_->SetNextScreen(ScreenType::ScreenPlayer);
                break;
            case 2:
                // help
                screenManager_->SetNextScreen(ScreenType::ScreenHelp);
                break;
            case 3:
                // exit game
                screenManager_->Exit();
                break;
            }
            gameManager_->PlaySound("menu_select");
        }

        // up/down keys change menu sel
        if (input->IsKeyPressed(i, Keys::Up) || input->IsButtonPressedDPadUp(i) ||
            input->IsButtonPressedLeftStickUp(i)) {
            menuSelection_ = (menuSelection_ == 0 ? NumberMenuItems - 1 : menuSelection_ - 1);
            gameManager_->PlaySound("menu_change");
        }
        if (input->IsKeyPressed(i, Keys::Down) || input->IsButtonPressedDPadDown(i) ||
            input->IsButtonPressedLeftStickDown(i)) {
            menuSelection_ = (menuSelection_ + 1) % NumberMenuItems;
            gameManager_->PlaySound("menu_change");
        }
    }
}

void ScreenIntro::Update(float elapsedTime) {
    // accumulate elapsed time
    menuTime_ += elapsedTime;
}

void ScreenIntro::Draw3D(GraphicsDevice* gd) {
    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    // clear background
    gd->Clear(Color::Black);

    // draw background animation
    screenManager_->DrawBackground(gd);
}

void ScreenIntro::DrawCursor(int x, int y) {
    Rectangle rect(0, 0, 0, 0);

    float rotation = menuTime_ * 2;

    // draw animated cursor texture
    rect.X = x - textureCursorAnim_->getWidthProperty() / 2;
    rect.Y = y - textureCursorAnim_->getHeightProperty() / 2;
    rect.Width = textureCursorAnim_->getWidthProperty();
    rect.Height = textureCursorAnim_->getHeightProperty();
    screenManager_->DrawTexture(&textureCursorAnim_.value(), rect, rotation, Color::White,
                                BlendState::AlphaBlend);

    // draw bullet cursor texture
    rect.X = x - textureCursorBullet_->getWidthProperty() / 2;
    rect.Y = y - textureCursorBullet_->getHeightProperty() / 2;
    rect.Width = textureCursorBullet_->getWidthProperty();
    rect.Height = textureCursorBullet_->getHeightProperty();
    screenManager_->DrawTexture(&textureCursorBullet_.value(), rect, Color::White,
                                BlendState::AlphaBlend);

    // draw arrow cursor texture
    rect.X = x - textureCursorArrow_->getWidthProperty() / 2 + 32;
    rect.Y = y - textureCursorArrow_->getHeightProperty() / 2;
    rect.Width = textureCursorArrow_->getWidthProperty();
    rect.Height = textureCursorArrow_->getHeightProperty();
    screenManager_->DrawTexture(&textureCursorArrow_.value(), rect, Color::White,
                                BlendState::AlphaBlend);
}

void ScreenIntro::Draw2D(GraphicsDevice* gd, FontManager* font) {
    (void)font;

    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    // screen rect
    Rectangle rect(gd->getViewportProperty().getXProperty(),
                   gd->getViewportProperty().getYProperty(),
                   gd->getViewportProperty().getWidthProperty(),
                   gd->getViewportProperty().getHeightProperty());

    // draw lens flare texture
    screenManager_->DrawTexture(&textureLens_.value(), rect, Color::White, BlendState::Additive);

    // draw logo texture
    screenManager_->DrawTexture(&textureLogo_.value(), rect, Color::White,
                                BlendState::AlphaBlend);

    // draw menu itens
    int Y = rect.Height - 200;
    for (int i = 0; i < NumberMenuItems; i++) {
        // if item selected
        if (i == menuSelection_) {
            rect.X = 540;
            rect.Y = Y;
            rect.Width = textureMenuHover_[i]->getWidthProperty();
            rect.Height = textureMenuHover_[i]->getHeightProperty();
            screenManager_->DrawTexture(&textureMenuHover_[i].value(), rect, Color::White,
                                        BlendState::AlphaBlend);

            // draw cursor left of selected item
            DrawCursor(rect.X - 60, rect.Y + 19);

            Y += 50;
        } else // item not selected
        {
            rect.X = 540;
            rect.Y = Y;
            rect.Width = textureMenu_[i]->getWidthProperty();
            rect.Height = textureMenu_[i]->getHeightProperty();

            screenManager_->DrawTexture(&textureMenu_[i].value(), rect, Color::White,
                                        BlendState::AlphaBlend);

            Y += 40;
        }
    }
}

} // namespace ShipGame
