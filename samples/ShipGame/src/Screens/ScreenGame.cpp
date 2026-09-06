// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ScreenGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ScreenGame.hpp"

#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/ArgumentNullException.hpp"

#include "../FontManager.hpp"
#include "../GameManager.hpp"
#include "../GameOptions.hpp"
#include "../InputManager.hpp"
#include "../PlayerShip.hpp"
#include "ScreenManager.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Input::Keys;

void ScreenGame::SetFocus(ContentManager* content, bool focus) {
    // if getting focus
    if (focus == true) {
        // load all resources
        gameManager_->LoadFiles(*content);
    } else // loosing focus
    {
        // free all resources
        gameManager_->UnloadFiles();
    }
}

void ScreenGame::ProcessInput(float elapsedTime, InputManager* input) {
    if (input == nullptr) {
        throw System::ArgumentNullException("input");
    }

    gameManager_->ProcessInput(elapsedTime, input);

    int i, j = (int)gameManager_->getGameMode();
    for (i = 0; i < j; i++)
        if (input->IsKeyPressed(i, Keys::Escape) || input->IsButtonPressedBack(i)) {
            gameManager_->GetPlayer(i)->setScore(-1);
            screenManager_->SetNextScreen(ScreenType::ScreenEnd);
            gameManager_->PlaySound("menu_cancel");
        }
}

void ScreenGame::Update(float elapsedTime) {
    // update game
    gameManager_->Update(elapsedTime);

    // check if any player have reached the score limit
    // if so, changes to the end screen
    int i, j = (int)gameManager_->getGameMode();
    for (i = 0; i < j; i++)
        if (gameManager_->GetPlayer(i)->getScore() == GameOptions::MaxPoints)
            screenManager_->SetNextScreen(ScreenType::ScreenEnd, GameOptions::FadeColor,
                                          GameOptions::FadeTime);
}

void ScreenGame::Draw3D(GraphicsDevice* gd) {
    // draw the 3d game scene
    gameManager_->Draw3D(gd);
}

void ScreenGame::Draw2D(GraphicsDevice* gd, FontManager* font) {
    (void)gd;
    // draw 2D game gui
    gameManager_->Draw2D(font);
}

} // namespace ShipGame
