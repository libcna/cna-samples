// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "NonPhotoRealisticGame.hpp"

#include <map>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameterCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechniqueCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "Microsoft/Xna/Framework/Graphics/PresentationParameters.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace NonPhotoRealistic
{
    using namespace Microsoft::Xna::Framework::Input;

    NonPhotoRealisticGame::NonPhotoRealisticGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");
    }

    const std::string& NonPhotoRealisticGame::GetTypeName() const
    {
        static const std::string name = "NonPhotoRealistic.NonPhotoRealisticGame";
        return name;
    }

    const NonPhotoRealisticSettings& NonPhotoRealisticGame::getSettingsProperty() const
    {
        return NonPhotoRealisticSettings::PresetSettings[(std::size_t)settingsIndex];
    }

    void NonPhotoRealisticGame::LoadContent()
    {
        spriteBatch = std::make_unique<SpriteBatch>(*graphics.getGraphicsDeviceProperty());
        spriteFont = getContentProperty().Load<SpriteFont>("hudFont");
        model = getContentProperty().Load<Model>("Ship");
        postprocessEffect =
            getContentProperty().Load<std::shared_ptr<Effect>>("PostprocessEffect");
        sketchTexture = getContentProperty().Load<Texture2D>("SketchTexture");

        // Change the model to use our custom cartoon shading effect.
        std::shared_ptr<Effect> cartoonEffect =
            getContentProperty().Load<std::shared_ptr<Effect>>("CartoonEffect");

        ChangeEffectUsedByModel(*model, *cartoonEffect);

        // Create two custom rendertargets.
        const PresentationParameters& pp =
            graphics.getGraphicsDeviceProperty()->getPresentationParametersProperty();

        sceneRenderTarget = std::make_unique<RenderTarget2D>(
            *graphics.getGraphicsDeviceProperty(),
            pp.getBackBufferWidthProperty(), pp.getBackBufferHeightProperty(), false,
            pp.getBackBufferFormatProperty(), pp.getDepthStencilFormatProperty());

        normalDepthRenderTarget = std::make_unique<RenderTarget2D>(
            *graphics.getGraphicsDeviceProperty(),
            pp.getBackBufferWidthProperty(), pp.getBackBufferHeightProperty(), false,
            pp.getBackBufferFormatProperty(), pp.getDepthStencilFormatProperty());
    }

    void NonPhotoRealisticGame::UnloadContent()
    {
        if (sceneRenderTarget != nullptr)
        {
            sceneRenderTarget->Dispose();
            sceneRenderTarget = nullptr;
        }

        if (normalDepthRenderTarget != nullptr)
        {
            normalDepthRenderTarget->Dispose();
            normalDepthRenderTarget = nullptr;
        }
    }

    void NonPhotoRealisticGame::ChangeEffectUsedByModel(
        Model& model_, Effect& replacementEffect)
    {
        // Table mapping the original effects to our replacement versions.
        std::map<Effect*, Effect*> effectMapping;

        for (ModelMesh* mesh : model_.getMeshesProperty())
        {
            // Scan over all the effects currently on the mesh.
            for (Effect* meshEffect : mesh->getEffectsProperty())
            {
                // The original's foreach declares the loop variable as BasicEffect, which is
                // a cast that throws if a mesh ever carried another effect type.
                BasicEffect* oldEffect = dynamic_cast<BasicEffect*>(meshEffect);

                // If we haven't already seen this effect...
                if (effectMapping.find(meshEffect) == effectMapping.end())
                {
                    // Make a clone of our replacement effect. We can't just use
                    // it directly, because the same effect might need to be
                    // applied several times to different parts of the model using
                    // a different texture each time, so we need a fresh copy each
                    // time we want to set a different texture into it.
                    //
                    // Clone() hands back an owning raw pointer and ModelMeshPart keeps a
                    // non-owning one, so the clone is parked in a member that outlives the
                    // model -- XNA's own Dictionary does the same job by staying reachable.
                    replacementEffects.emplace_back(replacementEffect.Clone());
                    Effect* newEffect = replacementEffects.back().get();

                    // Copy across the texture from the original effect.
                    newEffect->getParametersProperty()["Texture"]->SetValue(
                        oldEffect->getTextureProperty());
                    newEffect->getParametersProperty()["TextureEnabled"]->SetValue(
                        oldEffect->getTextureEnabledProperty());

                    effectMapping.emplace(meshEffect, newEffect);
                }
            }

            // Now that we've found all the effects in use on this mesh,
            // update it to use our new replacement versions.
            for (ModelMeshPart* meshPart : mesh->getMeshPartsProperty())
            {
                meshPart->setEffectProperty(effectMapping[meshPart->getEffectProperty()]);
            }
        }
    }

    void NonPhotoRealisticGame::Update(GameTime& gameTime)
    {
        HandleInput();

        // Update the sketch overlay texture jitter animation.
        if (getSettingsProperty().SketchJitterSpeed > 0)
        {
            timeToNextJitter -= gameTime.getElapsedGameTimeProperty();

            if (timeToNextJitter <= System::TimeSpan::Zero)
            {
                sketchJitter.X = (float)random.NextDouble();
                sketchJitter.Y = (float)random.NextDouble();

                timeToNextJitter +=
                    System::TimeSpan::FromSeconds(getSettingsProperty().SketchJitterSpeed);
            }
        }

        Game::Update(gameTime);
    }

    void NonPhotoRealisticGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

        // Calculate the camera matrices.
        const float time = (float)gameTime.getTotalGameTimeProperty().getTotalSecondsProperty();

        const Matrix rotation = Matrix::CreateRotationY(time * 0.5f);

        const Matrix view = Matrix::CreateLookAt(Vector3(3000, 1500, 0),
                                                 Vector3::Zero,
                                                 Vector3::Up);

        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::PiOver4,
            device.getViewportProperty().getAspectRatioProperty(),
            1000, 10000);

        // If we are doing edge detection, first off we need to render the
        // normals and depth of our model into a special rendertarget.
        if (getSettingsProperty().EnableEdgeDetect)
        {
            device.SetRenderTarget(normalDepthRenderTarget.get());

            device.Clear(Color::Black);

            DrawModel(rotation, view, projection, "NormalDepth");
        }

        // If we are doing edge detection and/or pencil sketch processing, we
        // need to draw the model into a special rendertarget which can then be
        // fed into the postprocessing shader. Otherwise can just draw it
        // directly onto the backbuffer.
        if (getSettingsProperty().EnableEdgeDetect || getSettingsProperty().EnableSketch)
            device.SetRenderTarget(sceneRenderTarget.get());
        else
            device.SetRenderTarget(nullptr);

        device.Clear(Color::CornflowerBlue);

        // Draw the model, using either the cartoon or lambert shading technique.
        String effectTechniqueName;

        if (getSettingsProperty().EnableToonShading)
            effectTechniqueName = "Toon";
        else
            effectTechniqueName = "Lambert";

        DrawModel(rotation, view, projection, effectTechniqueName);

        // Run the postprocessing filter over the scene that we just rendered.
        if (getSettingsProperty().EnableEdgeDetect || getSettingsProperty().EnableSketch)
        {
            device.SetRenderTarget(nullptr);

            ApplyPostprocess();
        }

        // Display some text over the top. Note how we draw this after the
        // postprocessing, because we don't want the text to be affected by it.
        DrawOverlayText();

        Game::Draw(gameTime);
    }

    void NonPhotoRealisticGame::DrawModel(const Matrix& world, const Matrix& view,
                                          const Matrix& projection,
                                          const String& effectTechniqueName)
    {
        // Set suitable renderstates for drawing a 3D model.
        getGraphicsDeviceProperty().setBlendStateProperty(BlendState::Opaque);
        getGraphicsDeviceProperty().setDepthStencilStateProperty(DepthStencilState::Default);

        // Look up the bone transform matrices.
        std::vector<Matrix> transforms(
            (std::size_t)model->getBonesProperty().getCountProperty());

        model->CopyAbsoluteBoneTransformsTo(transforms);

        // Draw the model.
        for (ModelMesh* mesh : model->getMeshesProperty())
        {
            for (Effect* effect : mesh->getEffectsProperty())
            {
                // Specify which effect technique to use.
                effect->setCurrentTechniqueProperty(
                    effect->getTechniquesProperty()[effectTechniqueName]);

                const Matrix localWorld =
                    transforms[(std::size_t)mesh->getParentBoneProperty()->getIndexProperty()] *
                    world;

                effect->getParametersProperty()["World"]->SetValue(localWorld);
                effect->getParametersProperty()["View"]->SetValue(view);
                effect->getParametersProperty()["Projection"]->SetValue(projection);
            }

            mesh->Draw();
        }
    }

    void NonPhotoRealisticGame::ApplyPostprocess()
    {
        EffectParameterCollection& parameters = postprocessEffect->getParametersProperty();
        String effectTechniqueName;

        // Set effect parameters controlling the pencil sketch effect.
        if (getSettingsProperty().EnableSketch)
        {
            parameters["SketchThreshold"]->SetValue(getSettingsProperty().SketchThreshold);
            parameters["SketchBrightness"]->SetValue(getSettingsProperty().SketchBrightness);
            parameters["SketchJitter"]->SetValue(sketchJitter);
            parameters["SketchTexture"]->SetValue(&*sketchTexture);
        }

        // Set effect parameters controlling the edge detection effect.
        if (getSettingsProperty().EnableEdgeDetect)
        {
            const Vector2 resolution((float)sceneRenderTarget->getWidthProperty(),
                                     (float)sceneRenderTarget->getHeightProperty());

            Texture2D* const normalDepthTexture = normalDepthRenderTarget.get();

            parameters["EdgeWidth"]->SetValue(getSettingsProperty().EdgeWidth);
            parameters["EdgeIntensity"]->SetValue(getSettingsProperty().EdgeIntensity);
            parameters["ScreenResolution"]->SetValue(resolution);
            parameters["NormalDepthTexture"]->SetValue(normalDepthTexture);

            // Choose which effect technique to use.
            if (getSettingsProperty().EnableSketch)
            {
                if (getSettingsProperty().SketchInColor)
                    effectTechniqueName = "EdgeDetectColorSketch";
                else
                    effectTechniqueName = "EdgeDetectMonoSketch";
            }
            else
                effectTechniqueName = "EdgeDetect";
        }
        else
        {
            // If edge detection is off, just pick one of the sketch techniques.
            if (getSettingsProperty().SketchInColor)
                effectTechniqueName = "ColorSketch";
            else
                effectTechniqueName = "MonoSketch";
        }

        // Activate the appropriate effect technique.
        postprocessEffect->setCurrentTechniqueProperty(
            postprocessEffect->getTechniquesProperty()[effectTechniqueName]);

        // Draw a fullscreen sprite to apply the postprocessing effect.
        spriteBatch->Begin(SpriteSortMode::Deferred, BlendState::Opaque,
                           nullptr, nullptr, nullptr, postprocessEffect.get());
        spriteBatch->Draw(*sceneRenderTarget, Vector2::Zero, Color::White);
        spriteBatch->End();
    }

    void NonPhotoRealisticGame::DrawOverlayText()
    {
        const String text = "A = settings (" + getSettingsProperty().Name + ")";

        spriteBatch->Begin();

        // Draw the string twice to create a drop shadow, first colored black
        // and offset one pixel to the bottom right, then again in white at the
        // intended position. This makes text easier to read over the background.
        spriteBatch->DrawString(*spriteFont, text, Vector2(65, 65), Color::Black);
        spriteBatch->DrawString(*spriteFont, text, Vector2(64, 64), Color::White);

        spriteBatch->End();
    }

    void NonPhotoRealisticGame::HandleInput()
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

        // Switch to the next settings preset?
        if ((currentGamePadState.getButtonsProperty().getAProperty() == ButtonState::Pressed &&
             lastGamePadState.getButtonsProperty().getAProperty() != ButtonState::Pressed) ||
            (currentKeyboardState.IsKeyDown(Keys::A) &&
             lastKeyboardState.IsKeyUp(Keys::A)))
        {
            settingsIndex = (settingsIndex + 1) %
                            (int)NonPhotoRealisticSettings::PresetSettings.size();
        }
    }
}
