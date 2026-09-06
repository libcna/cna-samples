// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ScreenPlayer.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ScreenPlayer.hpp"

#include <cmath>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/CubeMapFace.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/Random.hpp"

#include "../FontManager.hpp"
#include "../GameManager.hpp"
#include "../Graphics/LightList.hpp"
#include "../InputManager.hpp"
#include "../ShipGame.hpp"
#include "ScreenManager.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::CubeMapFace;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::Effect;
using Microsoft::Xna::Framework::Graphics::ModelMesh;
using Microsoft::Xna::Framework::Graphics::SurfaceFormat;
using Microsoft::Xna::Framework::Input::Keys;

std::optional<TextureCube> ScreenPlayer::reflectCube_;

void ScreenPlayer::SetFocus(ContentManager* content, bool focus) {
    // if getting focus
    if (focus == true) {
        // load all resources
        confirmed_[0] = false;
        confirmed_[1] = (gameManager_->getGameMode() == GameMode::SinglePlayer);

        rotation_[0] = Matrix::getIdentityProperty();
        rotation_[1] = Matrix::getIdentityProperty();

        lights_ = LightList::Load("content/screens/player_lights.xml");

        for (int i = 0; i < NumberShips; i++) {
            shipModels_[i] = content->Load<Model>("ships/" + ships_[i]);
            FixupShip(shipModels_[i].value(), "ships/" + ships_[i]);
        }

        padModel_ = content->Load<Model>("ships/pad");
        padHaloModel_ = content->Load<Model>("ships/pad_halo");
        padSelectModel_ = content->Load<Model>("ships/pad_select");

        textureChangeShip_ = content->Load<Texture2D>("screens/change_ship");
        textureRotateShip_ = content->Load<Texture2D>("screens/rotate_ship");
        textureSelectBack_ = content->Load<Texture2D>("screens/select_back");
        textureSelectCancel_ = content->Load<Texture2D>("screens/select_cancel");
        textureInvertYCheck_ = content->Load<Texture2D>("screens/inverty_check");
        textureInvertYUncheck_ = content->Load<Texture2D>("screens/inverty_uncheck");
    } else // loosing focus
    {
        // free all resources
        lights_ = nullptr;

        for (int i = 0; i < NumberShips; i++)
            shipModels_[i].reset();

        padModel_.reset();
        padHaloModel_.reset();
        padSelectModel_.reset();

        textureChangeShip_.reset();
        textureRotateShip_.reset();
        textureSelectBack_.reset();
        textureSelectCancel_.reset();
        textureInvertYCheck_.reset();
        textureInvertYUncheck_.reset();
    }
}

void ScreenPlayer::ProcessInput(float elapsedTime, InputManager* input) {
    if (input == nullptr) {
        throw System::ArgumentNullException("input");
    }

    const float rotationVelocity = 3.0f;

    int i, j = (int)gameManager_->getGameMode();

    for (i = 0; i < j; i++)
        if (confirmed_[i] == false) {
            // change invert Y selection
            if (input->IsKeyPressed(i, Keys::Y) || input->IsButtonPressedY(i)) {
                invertY_ ^= ((std::uint32_t)1 << i);
                gameManager_->PlaySound("menu_change");
            }

            // confirm selection
            if (input->IsKeyPressed(i, Keys::Enter) || input->IsButtonPressedA(i)) {
                confirmed_[i] = true;
                gameManager_->PlaySound("menu_select");
            }

            // cancel and return to intro menu
            if (input->IsKeyPressed(i, Keys::Escape) || input->IsButtonPressedB(i)) {
                gameManager_->SetShips("", "", 0);
                screenManager_->SetNextScreen(ScreenType::ScreenIntro);
                gameManager_->PlaySound("menu_cancel");
            }

            // rotate ship
            float RotX = rotationVelocity * input->LeftStick(i).X * elapsedTime;
            if (input->IsKeyDown(i, Keys::Left))
                RotX -= rotationVelocity * elapsedTime;
            if (input->IsKeyDown(i, Keys::Right))
                RotX += rotationVelocity * elapsedTime;
            if (std::abs(RotX) < 0.001f)
                RotX = -0.5f * elapsedTime;
            rotation_[i] = rotation_[i] * Matrix::CreateRotationY(RotX);

            // change ship (next)
            if (input->IsKeyPressed(i, Keys::Up) || input->IsButtonPressedDPadUp(i) ||
                input->IsButtonPressedLeftStickUp(i)) {
                selection_[i] = (selection_[i] + 1) % NumberShips;
                gameManager_->PlaySound("menu_change");
            }

            // change ship (previous)
            if (input->IsKeyPressed(i, Keys::Down) || input->IsButtonPressedDPadDown(i) ||
                input->IsButtonPressedLeftStickDown(i)) {
                if (selection_[i] == 0)
                    selection_[i] = NumberShips - 1;
                else
                    selection_[i] = selection_[i] - 1;
                gameManager_->PlaySound("menu_change");
            }
        } else {
            // cancel selection
            if (input->IsKeyPressed(i, Keys::Escape) || input->IsButtonPressedB(i)) {
                confirmed_[i] = false;
                gameManager_->PlaySound("menu_cancel");
            }
        }

    // if both ships confirmed, go to game screen
    if (confirmed_[0] && confirmed_[1]) {
        if (gameManager_->getGameMode() == GameMode::SinglePlayer)
            gameManager_->SetShips(ships_[selection_[0]], "", invertY_);
        else
            gameManager_->SetShips(ships_[selection_[0]], ships_[selection_[1]], invertY_);
        screenManager_->SetNextScreen(ScreenType::ScreenLevel);
    }
}

void ScreenPlayer::Update(float elapsedTime) {
    // accumulate elapsed time
    this->elapsedTime_ += elapsedTime;
}

void ScreenPlayer::Draw3D(GraphicsDevice* gd) {
    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    // clear backgournd
    gd->Clear(Color::Black);

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

    // translation matrix
    Matrix transform = Matrix::CreateTranslation(0, -40, 0);

    // if single player mode
    if (gameManager_->getGameMode() == GameMode::SinglePlayer) {
        // draw ship model
        gameManager_->DrawModel(gd, &shipModels_[selection_[0]].value(),
                                RenderTechnique::NormalMapping, cameraPosition, rotation_[0],
                                viewProjection, lights_.get());
        // draw pad model
        gameManager_->DrawModel(gd, &padModel_.value(), RenderTechnique::NormalMapping,
                                cameraPosition, transform, viewProjection, lights_.get());

        // set additive blend
        gd->setDepthStencilStateProperty(DepthStencilState::DepthRead);
        gd->setBlendStateProperty(BlendState::Additive);

        // disable glow (zero in alpha)
        // gd.RenderState.SeparateAlphaBlendEnabled = true;
        // gd.RenderState.AlphaBlendOperation = BlendFunction.Add;
        // gd.RenderState.AlphaSourceBlend = Blend.Zero;
        // gd.RenderState.AlphaDestinationBlend = Blend.Zero;

        // draw pad halo model
        gameManager_->DrawModel(gd, &padHaloModel_.value(), RenderTechnique::PlainMapping,
                                cameraPosition, transform, viewProjection, nullptr);

        // enable glow (alpha not zero)
        // gd.RenderState.SeparateAlphaBlendEnabled = false;
        gd->setBlendStateProperty(BlendState::AlphaBlend);

        // if not confirmed, draw animated selection circle
        if (confirmed_[0] == false) {
            transform = Matrix::CreateRotationY(elapsedTime_);
            float scale = 1.0f + 0.03f * (float)std::cos(elapsedTime_ * 7);
            transform = transform * Matrix::CreateScale(scale);
            transform.M42 = -10;
            gameManager_->DrawModel(gd, &padSelectModel_.value(), RenderTechnique::PlainMapping,
                                    cameraPosition, transform, viewProjection, nullptr);
        }

        // restore blend modes
        gd->setDepthStencilStateProperty(DepthStencilState::Default);
        gd->setBlendStateProperty(BlendState::Opaque);
    } else // if multi player mode
    {
        Matrix transform1 = rotation_[0] * Matrix::CreateTranslation(90, 0, 0);
        Matrix transform2 = rotation_[1] * Matrix::CreateTranslation(-90, 0, 0);

        // draw ship model for player 1
        gameManager_->DrawModel(gd, &shipModels_[selection_[0]].value(),
                                RenderTechnique::NormalMapping, cameraPosition, transform1,
                                viewProjection, lights_.get());
        // draw ship model for player 2
        gameManager_->DrawModel(gd, &shipModels_[selection_[1]].value(),
                                RenderTechnique::NormalMapping, cameraPosition, transform2,
                                viewProjection, lights_.get());

        // draw pad model for player 1
        transform.M41 = 90;
        gameManager_->DrawModel(gd, &padModel_.value(), RenderTechnique::NormalMapping,
                                cameraPosition, transform, viewProjection, lights_.get());

        // draw pad model for player 2
        transform.M41 = -90;
        gameManager_->DrawModel(gd, &padModel_.value(), RenderTechnique::NormalMapping,
                                cameraPosition, transform, viewProjection, lights_.get());

        // set additive blend
        gd->setDepthStencilStateProperty(DepthStencilState::DepthRead);
        gd->setBlendStateProperty(BlendState::Additive);

        // disable glow (zero in alpha)
        // gd.RenderState.SeparateAlphaBlendEnabled = true;
        // gd.RenderState.AlphaBlendOperation = BlendFunction.Add;
        // gd.RenderState.AlphaSourceBlend = Blend.Zero;
        // gd.RenderState.AlphaDestinationBlend = Blend.Zero;

        // draw pad halo model for player 1
        transform.M41 = 90;
        gameManager_->DrawModel(gd, &padHaloModel_.value(), RenderTechnique::NormalMapping,
                                cameraPosition, transform, viewProjection, nullptr);

        // draw pad halo model for player 2
        transform.M41 = -90;
        gameManager_->DrawModel(gd, &padHaloModel_.value(), RenderTechnique::NormalMapping,
                                cameraPosition, transform, viewProjection, nullptr);

        // enable glow (alpha not zero)
        // gd.RenderState.SeparateAlphaBlendEnabled = false;
        gd->setBlendStateProperty(BlendState::AlphaBlend);

        // if not confirmed, draw animated selection circle for player 1
        if (confirmed_[0] == false) {
            transform = Matrix::CreateRotationY(elapsedTime_);
            float scale = 0.9f + 0.03f * (float)std::cos(elapsedTime_ * 7);
            transform = transform * Matrix::CreateScale(scale);
            transform.M41 = 90;
            transform.M42 = -10;
            gameManager_->DrawModel(gd, &padSelectModel_.value(), RenderTechnique::PlainMapping,
                                    cameraPosition, transform, viewProjection, nullptr);
        }

        // if not confirmed, draw animated selection circle for player 2
        if (confirmed_[1] == false) {
            transform = Matrix::CreateRotationY(elapsedTime_);
            float scale = 0.9f + 0.03f * (float)std::cos(elapsedTime_ * 7);
            transform = transform * Matrix::CreateScale(scale);
            transform.M41 = -90;
            transform.M42 = -10;
            gameManager_->DrawModel(gd, &padSelectModel_.value(), RenderTechnique::PlainMapping,
                                    cameraPosition, transform, viewProjection, nullptr);
        }

        // restore blend modes
        gd->setDepthStencilStateProperty(DepthStencilState::Default);
        gd->setBlendStateProperty(BlendState::Opaque);
    }
}

void ScreenPlayer::Draw2D(GraphicsDevice* gd, FontManager* font) {
    (void)font;

    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    Rectangle rect(0, 0, 0, 0);

    int screenSizeX = gd->getViewportProperty().getWidthProperty();
    int screenSizeY = gd->getViewportProperty().getHeightProperty();

    // if single player mode
    if (gameManager_->getGameMode() == GameMode::SinglePlayer) {
        rect.Width = textureSelectBack_->getWidthProperty();
        rect.Height = textureSelectBack_->getHeightProperty();
        rect.X = screenSizeX / 2 - rect.Width / 2;
        rect.Y = 50;
        if (confirmed_[0]) {
            rect.Width = textureSelectCancel_->getWidthProperty();
            rect.Height = textureSelectCancel_->getHeightProperty();
            screenManager_->DrawTexture(&textureSelectCancel_.value(), rect, Color::White,
                                        BlendState::AlphaBlend);
        } else
            screenManager_->DrawTexture(&textureSelectBack_.value(), rect, Color::White,
                                        BlendState::AlphaBlend);

        rect.Width = textureInvertYCheck_->getWidthProperty();
        rect.Height = textureInvertYCheck_->getHeightProperty();
        rect.Y = screenSizeY - rect.Height - 30;
        rect.X = screenSizeX / 2 - rect.Width / 2;
        if ((invertY_ & 1) == 0)
            screenManager_->DrawTexture(&textureInvertYUncheck_.value(), rect, Color::White,
                                        BlendState::AlphaBlend);
        else
            screenManager_->DrawTexture(&textureInvertYCheck_.value(), rect, Color::White,
                                        BlendState::AlphaBlend);

        rect.Width = textureChangeShip_->getWidthProperty();
        rect.Height = textureChangeShip_->getHeightProperty();
        rect.X = screenSizeX / 5 - rect.Width / 2;
        rect.Y = 60;
        screenManager_->DrawTexture(&textureChangeShip_.value(), rect, Color::White,
                                    BlendState::AlphaBlend);

        rect.Width = textureRotateShip_->getWidthProperty();
        rect.Height = textureRotateShip_->getHeightProperty();
        rect.X = screenSizeX * 4 / 5 - rect.Width / 2;
        rect.Y = 60;
        screenManager_->DrawTexture(&textureRotateShip_.value(), rect, Color::White,
                                    BlendState::AlphaBlend);
    } else // if multi player mode
    {
        rect.Width = textureChangeShip_->getWidthProperty();
        rect.Height = textureChangeShip_->getHeightProperty();
        rect.X = (screenSizeX - rect.Width) / 2;
        rect.Y = 40;
        screenManager_->DrawTexture(&textureChangeShip_.value(), rect, Color::White,
                                    BlendState::AlphaBlend);

        rect.Width = textureRotateShip_->getWidthProperty();
        rect.Height = textureRotateShip_->getHeightProperty();
        rect.X = (screenSizeX - rect.Width) / 2;
        rect.Y = 40 + textureChangeShip_->getHeightProperty();
        screenManager_->DrawTexture(&textureRotateShip_.value(), rect, Color::White,
                                    BlendState::AlphaBlend);

        rect.Width = textureInvertYCheck_->getWidthProperty();
        rect.Height = textureInvertYCheck_->getHeightProperty();
        rect.Y = screenSizeY - rect.Height - 30;
        rect.X = screenSizeX / 4 - rect.Width / 2;
        if ((invertY_ & 1) == 0)
            screenManager_->DrawTexture(&textureInvertYUncheck_.value(), rect, Color::White,
                                        BlendState::AlphaBlend);
        else
            screenManager_->DrawTexture(&textureInvertYCheck_.value(), rect, Color::White,
                                        BlendState::AlphaBlend);
        rect.X = screenSizeX * 3 / 4 - rect.Width / 2;
        if ((invertY_ & 2) == 0)
            screenManager_->DrawTexture(&textureInvertYUncheck_.value(), rect, Color::White,
                                        BlendState::AlphaBlend);
        else
            screenManager_->DrawTexture(&textureInvertYCheck_.value(), rect, Color::White,
                                        BlendState::AlphaBlend);

        rect.Width = textureSelectBack_->getWidthProperty();
        rect.Height = textureSelectBack_->getHeightProperty();
        rect.X = screenSizeX / 8 - rect.Width / 2;
        rect.Y = 40;
        if (confirmed_[0]) {
            rect.Width = textureSelectCancel_->getWidthProperty();
            rect.Height = textureSelectCancel_->getHeightProperty();
            screenManager_->DrawTexture(&textureSelectCancel_.value(), rect, Color::White,
                                        BlendState::AlphaBlend);
        } else
            screenManager_->DrawTexture(&textureSelectBack_.value(), rect, Color::White,
                                        BlendState::AlphaBlend);
        rect.Width = textureSelectBack_->getWidthProperty();
        rect.Height = textureSelectBack_->getHeightProperty();
        rect.X = screenSizeX * 7 / 8 - rect.Width / 2;
        rect.Y = 40;
        if (confirmed_[1]) {
            rect.Width = textureSelectCancel_->getWidthProperty();
            rect.Height = textureSelectCancel_->getHeightProperty();
            screenManager_->DrawTexture(&textureSelectCancel_.value(), rect, Color::White,
                                        BlendState::AlphaBlend);
        } else
            screenManager_->DrawTexture(&textureSelectBack_.value(), rect, Color::White,
                                        BlendState::AlphaBlend);
    }
}

void ScreenPlayer::FixupShip(Model& model, const std::string& path) {
    (void)path;

    for (ModelMesh* mesh : model.getMeshesProperty()) {
        // for each mesh part
        for (Effect* effect : mesh->getEffectsProperty()) {
            effect->getParametersProperty()["Reflect"]->SetValue(GetReflectCube());
        }
    }
}

TextureCube* ScreenPlayer::GetReflectCube() {
    if (reflectCube_.has_value())
        return &reflectCube_.value();

    Color cc[] = {
        Color(1.0f, 0.0f, 0.0f), Color(0.9f, 0.0f, 0.1f), Color(0.8f, 0.0f, 0.2f),
        Color(0.7f, 0.0f, 0.3f), Color(0.6f, 0.0f, 0.4f), Color(0.5f, 0.0f, 0.5f),
        Color(0.4f, 0.0f, 0.6f), Color(0.3f, 0.0f, 0.7f), Color(0.2f, 0.0f, 0.8f),
        Color(0.1f, 0.0f, 0.9f), Color(0.1f, 0.0f, 0.9f), Color(0.0f, 0.0f, 1.0f),
    };
    const int ccLength = (int)(sizeof(cc) / sizeof(cc[0]));

    reflectCube_.emplace(ShipGameGame::GetInstance()->getGraphicsDeviceProperty(), 8, true,
                         SurfaceFormat::Color);

    System::Random rand;

    for (int s = 0; s < 6; s++) {
        std::vector<Color> sideData(reflectCube_->getSizeProperty() *
                                    reflectCube_->getSizeProperty());
        for (int i = 0; i < (int)sideData.size(); i++) {
            sideData[i] = cc[rand.Next(ccLength)];
        }
        reflectCube_->SetData((CubeMapFace)s, sideData.data(), (int)sideData.size());
    }

    return &reflectCube_.value();
}

} // namespace ShipGame
