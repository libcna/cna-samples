// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "BloomPostprocessGame.hpp"

#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace BloomPostprocess
{
    using namespace Microsoft::Xna::Framework::Input;

    BloomPostprocessGame::BloomPostprocessGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        bloom = std::make_unique<BloomComponent>(*this);

        getComponentsProperty().Add(bloom.get());
    }

    const std::string& BloomPostprocessGame::GetTypeName() const
    {
        static const std::string name = "BloomPostprocess.BloomPostprocessGame";
        return name;
    }

    void BloomPostprocessGame::LoadContent()
    {
        spriteBatch = std::make_unique<SpriteBatch>(*graphics.getGraphicsDeviceProperty());
        spriteFont = getContentProperty().Load<SpriteFont>("hudFont");
        background = getContentProperty().Load<Texture2D>("sunset");
        model = getContentProperty().Load<Model>("tank");
    }

    void BloomPostprocessGame::Update(GameTime& gameTime)
    {
        HandleInput();

        Game::Update(gameTime);
    }

    void BloomPostprocessGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();
        const Viewport viewport = device.getViewportProperty();

        bloom->BeginDraw();

        device.Clear(Color::Black);

        // Draw the background image.
        spriteBatch->Begin(SpriteSortMode::Deferred, BlendState::Opaque);

        spriteBatch->Draw(*background,
                          Rectangle(0, 0, viewport.getWidthProperty(),
                                    viewport.getHeightProperty()),
                          Color::White);

        spriteBatch->End();

        // Draw the spinning model.
        device.setDepthStencilStateProperty(DepthStencilState::Default);

        DrawModel(gameTime);

        // Draw other components (which includes the bloom).
        Game::Draw(gameTime);

        // Display some text over the top. Note how we draw this after the bloom,
        // because we don't want the text to be affected by the postprocessing.
        DrawOverlayText();
    }

    void BloomPostprocessGame::DrawModel(const GameTime& gameTime)
    {
        const float time = (float)gameTime.getTotalGameTimeProperty().getTotalSecondsProperty();

        const Viewport viewport = graphics.getGraphicsDeviceProperty()->getViewportProperty();
        const float aspectRatio =
            (float)viewport.getWidthProperty() / (float)viewport.getHeightProperty();

        // Create camera matrices.
        const Matrix world = Matrix::CreateRotationY(time * 0.42f);

        const Matrix view = Matrix::CreateLookAt(Vector3(750, 100, 0),
                                                 Vector3(0, 300, 0),
                                                 Vector3::Up);

        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(1, aspectRatio,
                                                                      1, 10000);

        // Look up the bone transform matrices.
        std::vector<Matrix> transforms(
            (std::size_t)model->getBonesProperty().getCountProperty());

        model->CopyAbsoluteBoneTransformsTo(transforms);

        // Draw the model.
        for (ModelMesh* mesh : model->getMeshesProperty())
        {
            for (Effect* meshEffect : mesh->getEffectsProperty())
            {
                // The original's foreach declares the loop variable as BasicEffect, which
                // is a cast that throws if a mesh ever carried another effect type.
                BasicEffect* effect = dynamic_cast<BasicEffect*>(meshEffect);

                effect->setWorldProperty(
                    transforms[(std::size_t)mesh->getParentBoneProperty()->getIndexProperty()] *
                    world);
                effect->setViewProperty(view);
                effect->setProjectionProperty(projection);

                effect->EnableDefaultLighting();

                // Override the default specular color to make it nice and bright,
                // so we'll get some decent glints that the bloom can key off.
                effect->setSpecularColorProperty(Vector3::One);
            }

            mesh->Draw();
        }
    }

    void BloomPostprocessGame::DrawOverlayText()
    {
        const String text = "A = settings (" + bloom->getSettingsProperty().Name + ")\n" +
                            "B = toggle bloom (" +
                            (bloom->getVisibleProperty() ? "on" : "off") + ")\n" +
                            "X = show buffer (" + ToString(bloom->getShowBufferProperty()) + ")";

        spriteBatch->Begin();

        // Draw the string twice to create a drop shadow, first colored black
        // and offset one pixel to the bottom right, then again in white at the
        // intended position. This makes text easier to read over the background.
        spriteBatch->DrawString(*spriteFont, text, Vector2(65, 65), Color::Black);
        spriteBatch->DrawString(*spriteFont, text, Vector2(64, 64), Color::White);

        spriteBatch->End();
    }

    const std::string& BloomPostprocessGame::ToString(BloomComponent::IntermediateBuffer value)
    {
        static const std::string names[] = {
            "PreBloom", "BlurredHorizontally", "BlurredBothWays", "FinalResult"
        };
        return names[(std::size_t)value];
    }

    void BloomPostprocessGame::HandleInput()
    {
        lastKeyboardState = currentKeyboardState;
        lastGamePadState = currentGamePadState;

        currentKeyboardState = Keyboard::GetState();
        currentGamePadState = GamePad::GetState(PlayerIndex::One);

        // Check for exit.
        if (currentKeyboardState.IsKeyDown(Keys::Escape) ||
            currentGamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
        {
            Exit();
        }

        // Switch to the next bloom settings preset?
        if ((currentGamePadState.getButtonsProperty().getAProperty() == ButtonState::Pressed &&
             lastGamePadState.getButtonsProperty().getAProperty() != ButtonState::Pressed) ||
            (currentKeyboardState.IsKeyDown(Keys::A) &&
             lastKeyboardState.IsKeyUp(Keys::A)))
        {
            bloomSettingsIndex = (bloomSettingsIndex + 1) %
                                 (int)BloomSettings::PresetSettings.size();

            bloom->setSettingsProperty(
                BloomSettings::PresetSettings[(std::size_t)bloomSettingsIndex]);
            bloom->setVisibleProperty(true);
        }

        // Toggle bloom on or off?
        if ((currentGamePadState.getButtonsProperty().getBProperty() == ButtonState::Pressed &&
             lastGamePadState.getButtonsProperty().getBProperty() != ButtonState::Pressed) ||
            (currentKeyboardState.IsKeyDown(Keys::B) &&
             lastKeyboardState.IsKeyUp(Keys::B)))
        {
            bloom->setVisibleProperty(!bloom->getVisibleProperty());
        }

        // Cycle through the intermediate buffer debug display modes?
        if ((currentGamePadState.getButtonsProperty().getXProperty() == ButtonState::Pressed &&
             lastGamePadState.getButtonsProperty().getXProperty() != ButtonState::Pressed) ||
            (currentKeyboardState.IsKeyDown(Keys::X) &&
             lastKeyboardState.IsKeyUp(Keys::X)))
        {
            bloom->setVisibleProperty(true);
            bloom->setShowBufferProperty(static_cast<BloomComponent::IntermediateBuffer>(
                (int)bloom->getShowBufferProperty() + 1));

            if (bloom->getShowBufferProperty() > BloomComponent::IntermediateBuffer::FinalResult)
                bloom->setShowBufferProperty(BloomComponent::IntermediateBuffer::PreBloom);
        }
    }
}
