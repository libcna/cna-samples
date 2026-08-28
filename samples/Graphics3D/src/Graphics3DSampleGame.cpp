// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// DemoGame.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Graphics3DSampleGame.hpp"

#include <cmath>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DisplayOrientation.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/TimeSpan.hpp"
#include "System/Xml/Linq/XDocument.hpp"
#include "System/Xml/Linq/XElement.hpp"
#include "System/Xml/Linq/XName.hpp"

namespace Graphics3DSample
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::DisplayOrientation;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Point;
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::BlendState;
    using Microsoft::Xna::Framework::Graphics::DepthStencilState;
    using Microsoft::Xna::Framework::Graphics::RasterizerState;
    using Microsoft::Xna::Framework::Graphics::SamplerState;
    using Microsoft::Xna::Framework::Graphics::SpriteEffects;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Touch::GestureSample;
    using Microsoft::Xna::Framework::Input::Touch::GestureType;
    using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

    Graphics3DSampleGame::Graphics3DSampleGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        graphics.setIsFullScreenProperty(true);
        graphics.setSupportedOrientationsProperty(
            DisplayOrientation::LandscapeLeft | DisplayOrientation::LandscapeRight);
        graphics.ApplyChanges();

        // CNAEXT — not in the original. The upstream sample is a Windows Phone title and touch is
        // its only input: the four checkboxes read TouchPanel.GetState() and the camera is driven
        // by FreeDrag and Pinch. On a desktop or a browser without a touch screen there is no way
        // to operate it at all. This opt-in makes CNA report the left mouse button as a touch, so
        // the pointer feeds the same TouchPanel and gesture path the original reads; the game
        // logic below is untouched, and a real touch screen still works exactly as before.
        CNAEXT TouchPanel::setMouseTouchEmulationEnabledEXT(true);
    }

    const std::string& Graphics3DSampleGame::GetTypeName() const
    {
        static const std::string name = "Graphics3DSample.Graphics3DSampleGame";
        return name;
    }

    void Graphics3DSampleGame::Initialize()
    {
        // new DepthStencilState() { DepthBufferEnable = true }
        DepthStencilState depthStencilState;
        depthStencilState.setDepthBufferEnableProperty(true);
        getGraphicsDeviceProperty().setDepthStencilStateProperty(depthStencilState);

        spriteBatch.emplace(getGraphicsDeviceProperty());

        CreateSpaceship();
        CreateLightEnablingButtons();
        CreateBackgroundTextureEnablingButton();
        CreatePerPixelLightingButton();
        CreateAnimationButton();

        // Initialize gestures support - Pinch for Zoom and horizontal drag for rotate
        TouchPanel::setEnabledGesturesProperty(
            GestureType::FreeDrag | GestureType::Pinch | GestureType::PinchComplete);

        Game::Initialize();
    }

    void Graphics3DSampleGame::LoadContent()
    {
        background = getContentProperty().Load<Texture2D>("Textures/spaceBG");

        animation = CreateAnimation();
        spaceship.Load(getContentProperty());
        Game::LoadContent();
    }

    Animation Graphics3DSampleGame::CreateAnimation()
    {
        // Load multiple animations form XML definition
        auto doc = System::Xml::Linq::XDocument::Load("Content/AnimationDef.xml");
        System::Xml::Linq::XName name = System::Xml::Linq::XName::Get("Definition");
        auto definitions = doc->Descendants(name);

        // Get the first (and only in this case) animation from the XML definition
        auto definition = definitions.front();

        Texture2D texture = getContentProperty().Load<Texture2D>(
            definition->Attribute("SheetName")->getValueProperty());

        Point frameSize;
        frameSize.X = std::stoi(definition->Attribute("FrameWidth")->getValueProperty());
        frameSize.Y = std::stoi(definition->Attribute("FrameHeight")->getValueProperty());

        Point sheetSize;
        sheetSize.X = std::stoi(definition->Attribute("SheetColumns")->getValueProperty());
        sheetSize.Y = std::stoi(definition->Attribute("SheetRows")->getValueProperty());

        System::TimeSpan frameInterval = System::TimeSpan::FromSeconds(
            static_cast<float>(1) / std::stoi(definition->Attribute("Speed")->getValueProperty()));

        // Calculate the animation position (in the middle fot he screen)
        animationPosition = Vector2(
            static_cast<float>(graphics.getPreferredBackBufferWidthProperty() / 2 - frameSize.X),
            static_cast<float>(graphics.getPreferredBackBufferHeightProperty() / 2 - frameSize.Y));

        return Animation(texture, frameSize, sheetSize, frameInterval);
    }

    void Graphics3DSampleGame::CreateSpaceship()
    {
        spaceship.setProjectionProperty(Matrix::CreatePerspectiveFieldOfView(
            MathHelper::ToRadians(cameraFOV),
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(), 10, 20000));
    }

    void Graphics3DSampleGame::CreateLightEnablingButtons()
    {
        for (int n = 0; n < static_cast<int>(lightEnablingButtons.size()); n++)
        {
            lightEnablingButtons[static_cast<std::size_t>(n)] = std::make_unique<Checkbox>(
                *this, "Buttons/lamp_60x60",
                Rectangle(getGraphicsDeviceProperty().getViewportProperty().getWidthProperty()
                              - (n + 1) * (buttonWidth + buttonMargin),
                          buttonMargin, buttonWidth, buttonHeight),
                true);
            getComponentsProperty().Add(lightEnablingButtons[static_cast<std::size_t>(n)].get());
        }
    }

    void Graphics3DSampleGame::CreatePerPixelLightingButton()
    {
        perpixelLightingButton = std::make_unique<Checkbox>(
            *this, "Buttons/perPixelLight_60x60",
            Rectangle(getGraphicsDeviceProperty().getViewportProperty().getWidthProperty()
                          - (buttonWidth + buttonMargin),
                      getGraphicsDeviceProperty().getViewportProperty().getHeightProperty()
                          - (buttonHeight + buttonMargin),
                      buttonWidth, buttonHeight),
            false);
        getComponentsProperty().Add(perpixelLightingButton.get());
    }

    void Graphics3DSampleGame::CreateAnimationButton()
    {
        animationButton = std::make_unique<Checkbox>(
            *this, "Buttons/animation_60x60",
            Rectangle(buttonMargin,
                      getGraphicsDeviceProperty().getViewportProperty().getHeightProperty()
                          - (buttonHeight + buttonMargin),
                      buttonWidth, buttonHeight),
            false);
        getComponentsProperty().Add(animationButton.get());
    }

    void Graphics3DSampleGame::CreateBackgroundTextureEnablingButton()
    {
        backgroundTextureEnablingButton = std::make_unique<Checkbox>(
            *this, "Buttons/textureOnOff",
            Rectangle(buttonMargin, buttonMargin, buttonWidth, buttonHeight), false);
        getComponentsProperty().Add(backgroundTextureEnablingButton.get());
    }

    void Graphics3DSampleGame::Update(GameTime& gameTime)
    {
        // Handle touch input first
        HandleInput();

        // Allows the game to exit
        if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getBackProperty()
            == ButtonState::Pressed)
        {
            Exit();
        }

        spaceship.setRotationProperty(GetRotationMatrix());
        spaceship.setViewProperty(GetViewMatrix());
        std::vector<bool> lights;
        for (const auto& button : lightEnablingButtons)
        {
            lights.push_back(button->getIsCheckedProperty());
        }
        spaceship.setLightsProperty(lights);
        spaceship.setIsTextureEnabledProperty(true);
        spaceship.setIsPerPixelLightingEnabledProperty(perpixelLightingButton->getIsCheckedProperty());
        spaceship.setProjectionProperty(Matrix::CreatePerspectiveFieldOfView(
            MathHelper::ToRadians(cameraFOV),
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(), 10, 20000));

        if (animationButton->getIsCheckedProperty())
        {
            animation->Update(gameTime);
        }

        Game::Update(gameTime);
    }

    void Graphics3DSampleGame::HandleInput()
    {
        while (TouchPanel::getIsGestureAvailableProperty())
        {
            GestureSample gestureSample = TouchPanel::ReadGesture();
            switch (gestureSample.getGestureTypeProperty())
            {
                case GestureType::FreeDrag:
                    rotationXAmount += gestureSample.getDeltaProperty().X;
                    rotationYAmount -= gestureSample.getDeltaProperty().Y;
                    break;

                case GestureType::Pinch:
                {
                    float gestureValue = 0;
                    float minFOV = 60;
                    float maxFOV = 30;
                    float gestureLengthToZoomScale = 10;

                    Vector2 gestureDiff =
                        gestureSample.getPositionProperty() - gestureSample.getPosition2Property();
                    gestureValue = gestureDiff.Length() / gestureLengthToZoomScale;

                    if (prevLength.has_value()) // Skip the first pinch event
                    {
                        cameraFOV -= gestureValue - prevLength.value();
                    }

                    cameraFOV = MathHelper::Clamp(cameraFOV, maxFOV, minFOV);

                    prevLength = gestureValue;
                    break;
                }

                case GestureType::PinchComplete:
                    prevLength.reset();
                    break;
                default:
                    break;
            }
        }
    }

    Matrix Graphics3DSampleGame::GetRotationMatrix() const
    {
        Matrix matrix =
            Matrix::CreateWorld(Vector3(0, 250, 0), Vector3::Forward, Vector3::Up)
            * Matrix::CreateFromYawPitchRoll(
                  static_cast<float>(M_PI) + MathHelper::PiOver2 + rotationXAmount / 100,
                  rotationYAmount / 100, 0);
        return matrix;
    }

    Matrix Graphics3DSampleGame::GetViewMatrix() const
    {
        return Matrix::CreateLookAt(
            Vector3(3500, 400, 0) + Vector3(0, 250, 0),
            Vector3(0, 250, 0),
            Vector3::Up);
    }

    void Graphics3DSampleGame::Draw(const GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

        if (backgroundTextureEnablingButton->getIsCheckedProperty())
        {
            spriteBatch->Begin();
            spriteBatch->Draw(*background, Vector2::Zero, Color::White);
            spriteBatch->End();
        }

        // Set render states.
        getGraphicsDeviceProperty().setBlendStateProperty(BlendState::Opaque);
        getGraphicsDeviceProperty().setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
        getGraphicsDeviceProperty().setDepthStencilStateProperty(DepthStencilState::Default);
        getGraphicsDeviceProperty().getSamplerStatesProperty()[0] = SamplerState::LinearWrap;

        // This draws game components, including the currently active menu screen.
        // Draw the spaceship model
        spaceship.Draw();

        if (animationButton->getIsCheckedProperty())
        {
            DrawAnimation();
        }

        Game::Draw(gameTime);
    }

    void Graphics3DSampleGame::DrawAnimation()
    {
        spriteBatch->Begin();
        animation->Draw(*spriteBatch, animationPosition, 2.0f, SpriteEffects::None);
        spriteBatch->End();
    }
}
