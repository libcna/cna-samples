// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ScreenEnd.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ScreenEnd.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/ClearOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/ArgumentNullException.hpp"

#include "../FontManager.hpp"
#include "../GameManager.hpp"
#include "../Graphics/LightList.hpp"
#include "../InputManager.hpp"
#include "ScreenManager.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::ClearOptions;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Input::Keys;

void ScreenEnd::SetFocus(ContentManager* content, bool focus) {
    // if getting focus
    if (focus) {
        // load all resources
        int winner = gameManager_->getPlayerWinner();

        shipModel_ = content->Load<Model>("ships/" + gameManager_->GetPlayerShip(winner));

        padModel_ = content->Load<Model>("ships/pad");
        padHaloModel_ = content->Load<Model>("ships/pad_halo");

        lights_ = LightList::Load("content/screens/end_lights.xml");

        textureContinue_ = content->Load<Texture2D>("screens/continue");
        if (winner == 0)
            texturePlayerWin_ = content->Load<Texture2D>("screens/player1_wins");
        else
            texturePlayerWin_ = content->Load<Texture2D>("screens/player2_wins");
    } else // loosing focus
    {
        // free all resources
        shipModel_.reset();
        padModel_.reset();
        padHaloModel_.reset();

        lights_ = nullptr;

        textureContinue_.reset();
        texturePlayerWin_.reset();
    }
}

void ScreenEnd::ProcessInput(float elapsedTime, InputManager* input) {
    (void)elapsedTime;

    if (input == nullptr) {
        throw System::ArgumentNullException("input");
    }

    int i, j = (int)gameManager_->getGameMode();
    for (i = 0; i < j; i++) {
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

void ScreenEnd::Update(float elapsedTime) {
    // accumulate elapsed time
    this->elapsedTime_ += elapsedTime;
}

void ScreenEnd::Draw3D(GraphicsDevice* gd) {
    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    // clear background
    gd->Clear(ClearOptions::Target | ClearOptions::DepthBuffer, Color::Black, 1, 0);

    // draw background animation
    screenManager_->DrawBackground(gd);

    // screen aspect
    float aspect = (float)gd->getViewportProperty().getWidthProperty() /
                   (float)gd->getViewportProperty().getHeightProperty();

    // camera position
    Vector3 cameraPosition(0, 240, -800);

    // view and projection matrices
    Matrix view = Matrix::CreateLookAt(cameraPosition, Vector3::Zero, Vector3::Up);
    Matrix projection = Matrix::CreatePerspectiveFieldOfView(0.25f, aspect, 1, 1000);
    Matrix viewProjection = view * projection;

    // rotation matrix
    Matrix rotation = Matrix::CreateRotationY(0.5f * elapsedTime_);
    // translation matrix
    Matrix translation = Matrix::CreateTranslation(0, -40, 0);

    // draw ship model
    gameManager_->DrawModel(gd, &shipModel_.value(), RenderTechnique::NormalMapping,
                            cameraPosition, rotation, viewProjection, lights_.get());

    // draw pad model
    gameManager_->DrawModel(gd, &padModel_.value(), RenderTechnique::NormalMapping, cameraPosition,
                            translation, viewProjection, lights_.get());

    // set additive blend with no glow (zero on alpha)
    gd->setDepthStencilStateProperty(DepthStencilState::DepthRead);
    gd->setBlendStateProperty(BlendState::AlphaBlend);

    // disable glow (zero in alpha)
    // gd.RenderState.SeparateAlphaBlendEnabled = true;
    // gd.RenderState.AlphaBlendOperation = BlendFunction.Add;
    // gd.RenderState.AlphaSourceBlend = Blend.Zero;
    // gd.RenderState.AlphaDestinationBlend = Blend.Zero;

    // draw pad halo model
    gameManager_->DrawModel(gd, &padHaloModel_.value(), RenderTechnique::PlainMapping,
                            cameraPosition, translation, viewProjection, nullptr);

    // restore blend modes
    gd->setBlendStateProperty(BlendState::Opaque);
    gd->setDepthStencilStateProperty(DepthStencilState::Default);
}

void ScreenEnd::Draw2D(GraphicsDevice* gd, FontManager* font) {
    (void)font;

    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    Rectangle rect(0, 0, 0, 0);

    int screenSizeX = gd->getViewportProperty().getWidthProperty();
    int screenSizeY = gd->getViewportProperty().getHeightProperty();

    // draw continue message
    rect.Width = textureContinue_->getWidthProperty();
    rect.Height = textureContinue_->getHeightProperty();
    rect.Y = screenSizeY - rect.Height - 60;
    rect.X = screenSizeX / 2 - rect.Width / 2;
    screenManager_->DrawTexture(&textureContinue_.value(), rect, Color::White,
                                BlendState::AlphaBlend);

    // deaw winning player number
    rect.Width = texturePlayerWin_->getWidthProperty();
    rect.Height = texturePlayerWin_->getHeightProperty();
    rect.Y = 20;
    rect.X = screenSizeX / 2 - rect.Width / 2;
    screenManager_->DrawTexture(&texturePlayerWin_.value(), rect, Color::White,
                                BlendState::AlphaBlend);
}

} // namespace ShipGame
