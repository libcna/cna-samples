// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SplitScreenGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "SplitScreenGame.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/Math.hpp"
#include "System/TimeSpan.hpp"

namespace SplitScreenSample
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::BlendState;
    using Microsoft::Xna::Framework::Graphics::DepthStencilState;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::GamePadState;
    using Microsoft::Xna::Framework::Input::Keyboard;
    using Microsoft::Xna::Framework::Input::KeyboardState;
    using Microsoft::Xna::Framework::Input::Keys;

    SplitScreenGame::SplitScreenGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

#if defined(WINDOWS_PHONE)
        graphics.setIsFullScreenProperty(true);
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
#endif
    }

    const std::string& SplitScreenGame::GetTypeName() const
    {
        static const std::string name = "SplitScreenSample.SplitScreenGame";
        return name;
    }

    void SplitScreenGame::LoadContent()
    {
        tank.Load(getContentProperty());

        spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        blank.emplace(getGraphicsDeviceProperty(), 1, 1);
        const Color blankData[] = {Color::White};
        blank->SetData(blankData, 1);

        const Viewport& fullViewport = getGraphicsDeviceProperty().getViewportProperty();

        playerOneViewport.setMinDepthProperty(0.0f);
        playerOneViewport.setMaxDepthProperty(1.0f);
        playerOneViewport.setXProperty(0);
        playerOneViewport.setYProperty(0);
        playerOneViewport.setWidthProperty(fullViewport.getWidthProperty());
        playerOneViewport.setHeightProperty(fullViewport.getHeightProperty() / 2);

        playerTwoViewport.setMinDepthProperty(0.0f);
        playerTwoViewport.setMaxDepthProperty(1.0f);
        playerTwoViewport.setXProperty(0);
        playerTwoViewport.setYProperty(fullViewport.getHeightProperty() / 2);
        playerTwoViewport.setWidthProperty(fullViewport.getWidthProperty());
        playerTwoViewport.setHeightProperty(fullViewport.getHeightProperty() / 2);

        playerOneView = Matrix::CreateLookAt(
            Vector3(400.0f, 900.0f, 200.0f),
            Vector3(-100.0f, 0.0f, 0.0f),
            Vector3::Up);
        playerOneProjection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            playerOneViewport.getAspectRatioProperty(),
            10.0f,
            5000.0f);

        playerTwoView = Matrix::CreateLookAt(
            Vector3(0.0f, 800.0f, 800.0f),
            Vector3::Zero,
            Vector3::Up);
        playerTwoProjection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            playerTwoViewport.getAspectRatioProperty(),
            10.0f,
            5000.0f);
    }

    void SplitScreenGame::Update(GameTime& gameTime)
    {
        const KeyboardState keyState = Keyboard::GetState();
        const GamePadState gamePadState = GamePad::GetState(PlayerIndex::One);

        if (keyState.IsKeyDown(Keys::Escape)
            || gamePadState.getButtonsProperty().getBackProperty()
                == ButtonState::Pressed)
        {
            Exit();
        }

        const float time = static_cast<float>(
            gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());

        tank.setWheelRotationProperty(time * 5.0f);
        tank.setSteerRotationProperty(
            static_cast<float>(System::Math::Sin(time * 0.75f)) * 0.5f);
        tank.setTurretRotationProperty(
            static_cast<float>(System::Math::Sin(time * 0.333f)) * 1.25f);
        tank.setCannonRotationProperty(
            static_cast<float>(System::Math::Sin(time * 0.25f)) * 0.333f - 0.333f);
        tank.setHatchRotationProperty(MathHelper::Clamp(
            static_cast<float>(System::Math::Sin(time * 2.0f)) * 2.0f,
            -1.0f,
            0.0f));

        playerTwoView = Matrix::CreateLookAt(
            Vector3(
                static_cast<float>(System::Math::Cos(time)),
                1.0f,
                static_cast<float>(System::Math::Sin(time))) * 800.0f,
            Vector3::Zero,
            Vector3::Up);

        Game::Update(gameTime);
    }

    void SplitScreenGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = getGraphicsDeviceProperty();
        device.Clear(Color::CornflowerBlue);

        device.setBlendStateProperty(BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::Default);

        DrawScene(gameTime, playerOneViewport, playerOneView, playerOneProjection);
        DrawScene(gameTime, playerTwoViewport, playerTwoView, playerTwoProjection);

        DrawViewportEdges(playerOneViewport);
        DrawViewportEdges(playerTwoViewport);

        Game::Draw(gameTime);
    }

    void SplitScreenGame::DrawScene(const GameTime& gameTime,
                                    const Viewport& viewport,
                                    const Matrix& view,
                                    const Matrix& projection)
    {
        static_cast<void>(gameTime);

        GraphicsDevice& device = getGraphicsDeviceProperty();
        const Viewport oldViewport = device.getViewportProperty();
        device.setViewportProperty(viewport);

        tank.Draw(Matrix::getIdentityProperty(), view, projection);

        device.setViewportProperty(oldViewport);
    }

    void SplitScreenGame::DrawViewportEdges(const Viewport& viewport)
    {
        constexpr int edgeWidth = 2;

        const Rectangle topEdge(
            viewport.getXProperty() - edgeWidth / 2,
            viewport.getYProperty() - edgeWidth / 2,
            viewport.getWidthProperty() + edgeWidth,
            edgeWidth);
        const Rectangle bottomEdge(
            viewport.getXProperty() - edgeWidth / 2,
            viewport.getYProperty() + viewport.getHeightProperty() - edgeWidth / 2,
            viewport.getWidthProperty() + edgeWidth,
            edgeWidth);
        const Rectangle leftEdge(
            viewport.getXProperty() - edgeWidth / 2,
            viewport.getYProperty() - edgeWidth / 2,
            edgeWidth,
            viewport.getHeightProperty() + edgeWidth);
        const Rectangle rightEdge(
            viewport.getXProperty() + viewport.getWidthProperty() - edgeWidth / 2,
            viewport.getYProperty() - edgeWidth / 2,
            edgeWidth,
            viewport.getHeightProperty() + edgeWidth);

        spriteBatch->Begin();
        spriteBatch->Draw(*blank, topEdge, Color::Black);
        spriteBatch->Draw(*blank, bottomEdge, Color::Black);
        spriteBatch->Draw(*blank, leftEdge, Color::Black);
        spriteBatch->Draw(*blank, rightEdge, Color::Black);
        spriteBatch->End();
    }
}
