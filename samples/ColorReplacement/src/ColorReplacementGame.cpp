// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "ColorReplacementGame.hpp"

#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/Single.hpp"

namespace ColorReplacement
{
    using namespace Microsoft::Xna::Framework::Input;

    ColorReplacementGame::ColorReplacementGame()
        : graphics(this), targetColor(Color::Green.ToVector3())
    {
        getContentProperty().setRootDirectoryProperty("Content");

        // Create view matrix
        view = Matrix::CreateLookAt(
            Vector3(0, 2.75f, 5), Vector3(0, 0.25f, 0), Vector3::Up);
    }

    const std::string& ColorReplacementGame::GetTypeName() const
    {
        static const std::string name = "ColorReplacement.ColorReplacementGame";
        return name;
    }

    void ColorReplacementGame::Initialize()
    {
        Game::Initialize();

        // Create projection matrix
        projection = Matrix::CreatePerspectiveFieldOfView(
            1, getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(),
            1.0f, 10000.0f);
    }

    void ColorReplacementGame::LoadContent()
    {
        spriteBatch = std::make_unique<SpriteBatch>(*graphics.getGraphicsDeviceProperty());
        spriteFont = getContentProperty().Load<SpriteFont>("SpriteFont");
        model = getContentProperty().Load<Model>("Car");
    }

    void ColorReplacementGame::Update(GameTime& gameTime)
    {
        HandleInput();

        Game::Update(gameTime);
    }

    void ColorReplacementGame::Draw(const GameTime& gameTime)
    {
        graphics.getGraphicsDeviceProperty()->Clear(Color::CornflowerBlue);

        // Draw a spinning model
        getGraphicsDeviceProperty().setBlendStateProperty(BlendState::Opaque);
        getGraphicsDeviceProperty().setDepthStencilStateProperty(DepthStencilState::Default);

        Matrix world = Matrix::CreateRotationY(
            (float)gameTime.getTotalGameTimeProperty().getTotalSecondsProperty() * 0.2f);

        DrawModel(world);

        DrawOverlayText();

        Game::Draw(gameTime);
    }

    void ColorReplacementGame::DrawModel(const Matrix& world)
    {
        std::vector<Matrix> transforms(
            (std::size_t)model->getBonesProperty().getCountProperty());
        model->CopyAbsoluteBoneTransformsTo(transforms);

        for (ModelMesh* mesh : model->getMeshesProperty())
        {
            Matrix boneWorld =
                transforms[(std::size_t)mesh->getParentBoneProperty()->getIndexProperty()] * world;

            for (Effect* effect : mesh->getEffectsProperty())
            {
                // The car model has been modified to reference ReplaceColor.fx
                // The parameters need to be set differently for that effect
                BasicEffect* basicEffect = dynamic_cast<BasicEffect*>(effect);
                if (basicEffect != nullptr)
                {
                    // Set parameters on a BasicEffect
                    basicEffect->EnableDefaultLighting();
                    basicEffect->setWorldProperty(boneWorld);
                    basicEffect->setViewProperty(view);
                    basicEffect->setProjectionProperty(projection);
                }
                else
                {
                    // Set parameters on a color replacement effect
                    effect->getParametersProperty()["WorldViewProjection"]->SetValue(
                        boneWorld * view * projection);
                    effect->getParametersProperty()["World"]->SetValue(boneWorld);
                    effect->getParametersProperty()["TargetColor"]->SetValue(targetColor);
                }
            }
            mesh->Draw();
        }
    }

    void ColorReplacementGame::DrawOverlayText()
    {
        spriteBatch->Begin();

        spriteBatch->DrawString(*spriteFont,
            "Hold R/G/B key or X/A/B button and press Up/Down to change color",
            Vector2(50, 30), Color::Black);

        spriteBatch->DrawString(*spriteFont,
            "Red (R key, B button): " + System::Single::ToString(targetColor.X, "0.000"),
            Vector2(50, 50), Color::Red);
        spriteBatch->DrawString(*spriteFont,
            "Green (G key, A button): " + System::Single::ToString(targetColor.Y, "0.000"),
            Vector2(50, 70), Color::Lime);
        spriteBatch->DrawString(*spriteFont,
            "Blue (B key, X button): " + System::Single::ToString(targetColor.Z, "0.000"),
            Vector2(50, 90), Color::Blue);

        spriteBatch->End();
    }

    void ColorReplacementGame::HandleInput()
    {
        KeyboardState keyboardState = Keyboard::GetState();
        GamePadState gamePadState = GamePad::GetState(PlayerIndex::One);

        // Check for exit.
        if (keyboardState.IsKeyDown(Keys::Escape) ||
            gamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
        {
            Exit();
        }

        // Calculate how much to change the color with up/down
        float colorChange =
            gamePadState.getThumbSticksProperty().getLeftProperty().Y * ColorChangeRate;
        if (keyboardState.IsKeyDown(Keys::Up) ||
            gamePadState.getDPadProperty().getUpProperty() == ButtonState::Pressed)
        {
            colorChange = ColorChangeRate;
        }
        else if (keyboardState.IsKeyDown(Keys::Down) ||
            gamePadState.getDPadProperty().getDownProperty() == ButtonState::Pressed)
        {
            colorChange = -ColorChangeRate;
        }

        // Change red
        if (keyboardState.IsKeyDown(Keys::R) ||
            gamePadState.getButtonsProperty().getBProperty() == ButtonState::Pressed)
        {
            targetColor.X = MathHelper::Clamp(targetColor.X + colorChange, 0.0f, 1.0f);
        }

        // Change green
        if (keyboardState.IsKeyDown(Keys::G) ||
            gamePadState.getButtonsProperty().getAProperty() == ButtonState::Pressed)
        {
            targetColor.Y = MathHelper::Clamp(targetColor.Y + colorChange, 0.0f, 1.0f);
        }

        // Change blue
        if (keyboardState.IsKeyDown(Keys::B) ||
            gamePadState.getButtonsProperty().getXProperty() == ButtonState::Pressed)
        {
            targetColor.Z = MathHelper::Clamp(targetColor.Z + colorChange, 0.0f, 1.0f);
        }
    }
}
