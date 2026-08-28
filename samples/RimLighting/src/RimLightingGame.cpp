// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game1.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "RimLightingGame.hpp"


#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/EnvironmentMapEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerStateCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/InvalidCastException.hpp"
#include "System/TimeSpan.hpp"
#include "System/String.hpp"

namespace RimLighting
{
    using namespace Microsoft::Xna::Framework::Input;
    using Microsoft::Xna::Framework::Input::Touch::TouchCollection;
    using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

    const Vector3 SampleGame::CameraInitPosition = Vector3(0, 0, -60);

    namespace
    {
        /// C#'s `string.Format("{0}", float)`: the shortest round-trippable decimal.
        /// `%g` was not that -- it stops at six significant digits, so 3.1415927f printed as
        /// "3.14159" where .NET prints "3.1415927".
        String FormatFloat(float value)
        {
            return System::String::Format("{0}", value);
        }
    }

    SampleGame::SampleGame()
        : graphics(this)
        , modelViewerCamera(CameraInitPosition, Vector3::Up, 0, 0, 480, 800)
        , matrixView(Matrix::CreateLookAt(CameraInitPosition, Vector3::Zero, Vector3::Up))
    {
        getContentProperty().setRootDirectoryProperty("Content");

        // Frame rate is 30 fps by default for Windows Phone.
        setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));

        // Pre-autoscale settings.
        graphics.setPreferredBackBufferWidthProperty(480);
        graphics.setPreferredBackBufferHeightProperty(800);

        graphics.setIsFullScreenProperty(true);

        // The original is a Windows Phone project and is driven entirely through TouchPanel:
        // the button, both slidebars and the arcball read nothing else. On a desktop host there
        // is no digitizer, so this opt-in makes the left mouse button arrive as a touch through
        // the same path a finger would take. It is off by default framework-wide; SAMPLE-021
        // PathDrawing established it for exactly this case.
        CNAEXT TouchPanel::setMouseTouchEmulationEnabledEXT(true);
    }

    const std::string& SampleGame::GetTypeName() const
    {
        static const std::string name = "RimLighting.SampleGame";
        return name;
    }

    void SampleGame::LoadContent()
    {
        // Create a new SpriteBatch, which can be used to draw textures.
        spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        spriteFont = getContentProperty().Load<SpriteFont>("Font");

        // This is the mesh we want to render
        model = getContentProperty().Load<Model>("head");

        // An empty white texture used as default texture for the effect
        texure2D = getContentProperty().Load<Texture2D>("blankTex");

        // The cubemap used for generating rim light
        texureRimlightingCube = getContentProperty().Load<TextureCube>("OutputCube");

        // Add UI controls
        buttonToggleWorldCamera = std::make_unique<Button>(
            getGraphicsDeviceProperty(), *spriteFont, "Rotating World");
        buttonToggleWorldCamera->setPositionProperty(
            Vector2(0, (float)graphics.getPreferredBackBufferHeightProperty() - 31));
        buttonToggleWorldCamera->setSizeProperty(Vector2(160, 30));
        buttonToggleWorldCamera->setIsVisibleProperty(true);
        buttonToggleWorldCamera->OnClick += [this](System::Object*, const System::EventArgs&)
        {
            ButtonToggleWorldCameraOnClick();
        };
        uiElementList.push_back(buttonToggleWorldCamera.get());

        slideBarEnvironmentMapAmount = std::make_unique<Slidebar>(*this, *spriteFont, 0, 5);
        slideBarEnvironmentMapAmount->setIsVisibleProperty(true);
        slideBarEnvironmentMapAmount->setPositionProperty(
            Vector2(0, (float)graphics.getPreferredBackBufferHeightProperty() - 180));
        slideBarEnvironmentMapAmount->OnValueChanged +=
            [this](System::Object*, const System::EventArgs&)
        {
            SlideBarEnvironmentMapAmountOnValueChanged();
        };
        // Note setting .Value should be before the next line because this makes .TextSize valid
        // which is used below
        slideBarEnvironmentMapAmount->setValueProperty(2.5f);
        slideBarEnvironmentMapAmount->SetBarOffsetSize(
            10, slideBarEnvironmentMapAmount->getTextSizeProperty().Y,
            (float)graphics.getPreferredBackBufferWidthProperty() - 20, 4);
        uiElementList.push_back(slideBarEnvironmentMapAmount.get());

        slideBarFresnelFactor = std::make_unique<Slidebar>(*this, *spriteFont, 0, 10);
        slideBarFresnelFactor->setIsVisibleProperty(true);
        slideBarFresnelFactor->setPositionProperty(
            Vector2(0, (float)graphics.getPreferredBackBufferHeightProperty() - 100));
        slideBarFresnelFactor->OnValueChanged +=
            [this](System::Object*, const System::EventArgs&)
        {
            SlideBarFresnelFactorOnValueChanged();
        };
        slideBarFresnelFactor->setValueProperty(6);
        slideBarFresnelFactor->SetBarOffsetSize(
            10, slideBarFresnelFactor->getTextSizeProperty().Y,
            (float)graphics.getPreferredBackBufferWidthProperty() - 20, 4);
        uiElementList.push_back(slideBarFresnelFactor.get());

        Game::LoadContent();
    }

    void SampleGame::SlideBarEnvironmentMapAmountOnValueChanged()
    {
        slideBarEnvironmentMapAmount->setTextProperty(
            "Amount: " + FormatFloat(slideBarEnvironmentMapAmount->getValueProperty()));
    }

    void SampleGame::SlideBarFresnelFactorOnValueChanged()
    {
        slideBarFresnelFactor->setTextProperty(
            "Thickness (FresnelFactor): " +
            FormatFloat(slideBarFresnelFactor->getValueProperty()));
    }

    void SampleGame::ButtonToggleWorldCameraOnClick()
    {
        if (rotatingMode == RotatingMode::RotatingWorld)
        {
            rotatingMode = RotatingMode::RotatingCamera;
            buttonToggleWorldCamera->setTextProperty("Rotating Camera");
        }
        else
        {
            rotatingMode = RotatingMode::RotatingWorld;
            buttonToggleWorldCamera->setTextProperty("Rotating World");
        }
    }

    void SampleGame::Update(GameTime& gameTime)
    {
        // Allows the game to exit
        if (GamePad::GetState(PlayerIndex::One).getButtonsProperty().getBackProperty() ==
            ButtonState::Pressed)
            this->Exit();

        TouchCollection tc = TouchPanel::GetState();

        // Update our UI elements
        for (int t = 0; t < tc.getCountProperty(); t++)
        {
            for (std::size_t u = 0; u < uiElementList.size(); ++u)
                uiElementList[u]->HandleTouch(tc[t]);
        }

        // Update World or View matrices according to the user's drag on screen
        if (!slideBarEnvironmentMapAmount->IsDragging && !slideBarFresnelFactor->IsDragging)
        {
            modelViewerCamera.setIsRotatingWorldProperty(
                rotatingMode == RotatingMode::RotatingWorld);
            for (int t = 0; t < tc.getCountProperty(); ++t)
                modelViewerCamera.HandleTouch(tc[t]);
        }

        Game::Update(gameTime);
    }

    void SampleGame::Draw(const GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(Color(0.3f, 0.3f, 0.3f));

        // Please refer to the sample doc on why the matrix should be set like this for
        // implementing RimLighting
        Matrix world = modelViewerCamera.GetWorldMatrix() * modelViewerCamera.GetViewMatrix();
        Matrix view = Matrix::getIdentityProperty();
        Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(),
            1, 10000);
        std::vector<Matrix> transforms(
            (std::size_t)model->getBonesProperty().getCountProperty());

        model->CopyAbsoluteBoneTransformsTo(transforms);

        getGraphicsDeviceProperty().setBlendStateProperty(BlendState::Opaque);
        getGraphicsDeviceProperty().setRasterizerStateProperty(
            RasterizerState::CullCounterClockwise);
        getGraphicsDeviceProperty().setDepthStencilStateProperty(DepthStencilState::Default);
        getGraphicsDeviceProperty().getSamplerStatesProperty()[0] = SamplerState::LinearWrap;

        // Draw the model.
        for (ModelMesh* mesh : model->getMeshesProperty())
        {
            for (Effect* meshEffect : mesh->getEffectsProperty())
            {
                // The content project builds head.fbx with
                // <ProcessorParameters_DefaultEffect>EnvironmentMapEffect</...>, so every mesh
                // effect really is one; the original's `foreach (EnvironmentMapEffect effect in
                // mesh.Effects)` is C#'s own cast-per-element loop, which throws on a mismatch.
                auto* effect = dynamic_cast<EnvironmentMapEffect*>(meshEffect);
                if (effect == nullptr)
                    throw System::InvalidCastException(
                        "RimLighting: a mesh effect is not an EnvironmentMapEffect.");

                effect->setEnvironmentMapProperty(&texureRimlightingCube.value());

                effect->setWorldProperty(
                    transforms[(std::size_t)mesh->getParentBoneProperty()->getIndexProperty()] *
                    world);
                effect->setViewProperty(view);
                effect->setProjectionProperty(projection);

                effect->getDirectionalLight0Property().setEnabledProperty(true);
                // Please refer to the sample doc
                effect->getDirectionalLight0Property().setDirectionProperty(
                    Vector3::TransformNormal(Vector3::Left, modelViewerCamera.GetViewMatrix()));
                effect->getDirectionalLight1Property().setEnabledProperty(false);
                effect->getDirectionalLight2Property().setEnabledProperty(false);

                effect->setTextureProperty(&texure2D.value());

                effect->setDiffuseColorProperty(Color::White.ToVector3());

                effect->setFresnelFactorProperty(slideBarFresnelFactor->getValueProperty());
                effect->setEnvironmentMapAmountProperty(
                    slideBarEnvironmentMapAmount->getValueProperty());
            }

            mesh->Draw();
        }

        // Draw our UI elements
        for (std::size_t i = 0; i < uiElementList.size(); ++i)
        {
            uiElementList[i]->Draw(*spriteBatch);
        }

        Game::Draw(gameTime);
    }
}
