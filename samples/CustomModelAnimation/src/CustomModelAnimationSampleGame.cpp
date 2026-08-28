// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CustomModelAnimationSample.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CustomModelAnimationSampleGame.hpp"

#include <cmath>
#include <vector>

#include "ModelData.hpp"
#include "ModelDataContentReaders.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/SkinnedEffect.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "System/InvalidCastException.hpp"
#include "System/TimeSpan.hpp"

namespace CustomAvatarAnimationSample
{
    using CustomModelAnimation::ModelContentReaderRegistrationEXT;
    using CustomModelAnimation::ModelData;
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Graphics::BasicEffect;
    using Microsoft::Xna::Framework::Graphics::BlendState;
    using Microsoft::Xna::Framework::Graphics::DepthStencilState;
    using Microsoft::Xna::Framework::Graphics::Effect;
    using Microsoft::Xna::Framework::Graphics::ModelMesh;
    using Microsoft::Xna::Framework::Graphics::SkinnedEffect;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::Keyboard;

    CustomAvatarAnimationSampleGame::CustomAvatarAnimationSampleGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        // CNAEXT -- not a line the original needs. Both models' .xnb files were serialized
        // reflectively, and XNA rebuilds the readers for ModelData/ModelAnimationClip/
        // ModelKeyframe from the types themselves. C++ has no reflection, so the game declares
        // the field lists once and CNA builds the same readers. See diff.md.
        CNAEXT ModelContentReaderRegistrationEXT::RegisterEXT();
    }

    const std::string& CustomAvatarAnimationSampleGame::GetTypeName() const
    {
        static const std::string name =
            "CustomAvatarAnimationSample.CustomAvatarAnimationSampleGame";
        return name;
    }

    void CustomAvatarAnimationSampleGame::LoadContent()
    {
        // Load the rigid model
        rigidModel = getContentProperty().Load<Model>("AnimatedCube");
        rigidWorld = Matrix::CreateScale(.05f, .05f, .05f);

        // Create animation players/clips for the rigid model
        auto* modelData = dynamic_cast<ModelData*>(rigidModel->getTagProperty());
        if (modelData != nullptr)
        {
            const auto& rootClips = modelData->getRootAnimationClipsProperty();
            if (rootClips.find("Take 001") != rootClips.end())
            {
                rigidRootClip = rootClips.at("Take 001");

                rigidRootPlayer = std::make_unique<RootAnimationPlayer>();
                rigidRootPlayer->Completed +=
                    [this](System::Object*, const System::EventArgs&) { rigidPlayer_Completed(); };
                rigidRootPlayer->StartClip(rigidRootClip, 1, System::TimeSpan::Zero);
            }

            const auto& modelClips = modelData->getModelAnimationClipsProperty();
            if (modelClips.find("Take 001") != modelClips.end())
            {
                rigidClip = modelClips.at("Take 001");

                rigidPlayer = std::make_unique<RigidAnimationPlayer>(
                    rigidModel->getBonesProperty().getCountProperty());
                rigidPlayer->Completed +=
                    [this](System::Object*, const System::EventArgs&) { rigidPlayer_Completed(); };
                rigidPlayer->StartClip(rigidClip, 1, System::TimeSpan::Zero);
            }
        }

        // Load the skinned model
        skinnedModel = getContentProperty().Load<Model>("DudeWalk");
        skinnedWorld = Matrix::CreateScale(.025f, .025f, .025f) *
                       Matrix::CreateRotationY(static_cast<float>(-M_PI / 2));

        // Create animation players for the skinned model
        modelData = dynamic_cast<ModelData*>(skinnedModel->getTagProperty());
        if (modelData != nullptr)
        {
            const auto& rootClips = modelData->getRootAnimationClipsProperty();
            if (rootClips.find("Take 001") != rootClips.end())
            {
                skinnedRootClip = rootClips.at("Take 001");

                skinnedRootPlayer = std::make_unique<RootAnimationPlayer>();
                skinnedRootPlayer->Completed +=
                    [this](System::Object*, const System::EventArgs&) { skinnedPlayer_Completed(); };
            }

            const auto& modelClips = modelData->getModelAnimationClipsProperty();
            if (modelClips.find("Take 001") != modelClips.end())
            {
                skinnedClip = modelClips.at("Take 001");

                skinnedPlayer = std::make_unique<SkinnedAnimationPlayer>(
                    modelData->getBindPoseProperty(),
                    modelData->getInverseBindPoseProperty(),
                    modelData->getSkeletonHierarchyProperty());
                skinnedPlayer->Completed +=
                    [this](System::Object*, const System::EventArgs&) { skinnedPlayer_Completed(); };
            }
        }

        // Create the projection/view matrix we'll use for rendering
        projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::ToRadians(45.0f),
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(),
            .01f, 200.0f);
        view = Matrix::CreateLookAt(Vector3(0, 1, 4), Vector3(0, 1, 0), Vector3::Up);

        spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        font = getContentProperty().Load<SpriteFont>("font");
    }

    void CustomAvatarAnimationSampleGame::skinnedPlayer_Completed()
    {
        playingSkinned = false;
    }

    void CustomAvatarAnimationSampleGame::rigidPlayer_Completed()
    {
        playingRigid = false;
    }

    void CustomAvatarAnimationSampleGame::Update(GameTime& gameTime)
    {
        // Get the current gamepad state and store the old
        lastGamePadState = currentGamePadState;
        currentGamePadState = GamePad::GetState(PlayerIndex::One);

        lastKeyboardState = currentKeyboardState;
        currentKeyboardState = Keyboard::GetState();

        // Allows the game to exit
        if (currentGamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed ||
            currentKeyboardState.IsKeyDown(Keys::Escape))
            Exit();

        // When the A button is pressed and we aren't playing the rigid animations, play them
        if ((IsNewButtonPress(Buttons::A) || IsNewKeyPress(Keys::A)) && playingRigid == false)
        {
            if (rigidPlayer != nullptr && rigidClip != nullptr)
            {
                rigidPlayer->StartClip(rigidClip, 1, System::TimeSpan::Zero);
                playingRigid = true;
            }

            if (rigidRootPlayer != nullptr && rigidRootClip != nullptr)
            {
                rigidRootPlayer->StartClip(rigidRootClip, 1, System::TimeSpan::Zero);
                playingRigid = true;
            }
        }

        // When the B button is pressed and we aren't playing the skinned animations, play them
        if ((IsNewButtonPress(Buttons::B) || IsNewKeyPress(Keys::B)) && playingSkinned == false)
        {
            if (skinnedPlayer != nullptr && skinnedClip != nullptr)
            {
                skinnedPlayer->StartClip(skinnedClip, 1, System::TimeSpan::Zero);
                playingSkinned = true;
            }

            if (skinnedRootPlayer != nullptr && skinnedRootClip != nullptr)
            {
                skinnedRootPlayer->StartClip(skinnedRootClip, 1, System::TimeSpan::Zero);
                playingSkinned = true;
            }
        }

        // If we are playing rigid animations, update the players
        if (playingRigid)
        {
            if (rigidRootPlayer != nullptr)
                rigidRootPlayer->Update(gameTime);

            if (rigidPlayer != nullptr)
                rigidPlayer->Update(gameTime);
        }

        // If we are playing skinned animations, update the players
        if (playingSkinned)
        {
            if (skinnedRootPlayer != nullptr)
                skinnedRootPlayer->Update(gameTime);

            if (skinnedPlayer != nullptr)
                skinnedPlayer->Update(gameTime);
        }

        Game::Update(gameTime);
    }

    bool CustomAvatarAnimationSampleGame::IsNewButtonPress(Buttons button) const
    {
        return currentGamePadState.IsButtonDown(button) && lastGamePadState.IsButtonUp(button);
    }

    bool CustomAvatarAnimationSampleGame::IsNewKeyPress(Keys key) const
    {
        return currentKeyboardState.IsKeyDown(key) && lastKeyboardState.IsKeyUp(key);
    }

    void CustomAvatarAnimationSampleGame::Draw(const GameTime& gameTime)
    {
        // Reset the rendering states changed by spriteBatch
        getGraphicsDeviceProperty().setBlendStateProperty(BlendState::Opaque);
        getGraphicsDeviceProperty().setDepthStencilStateProperty(DepthStencilState::Default);

        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

        if (playingRigid)
            DrawRigidModel(*rigidModel, rigidPlayer.get(), rigidRootPlayer.get());

        if (playingSkinned)
            DrawSkinnedModel(*skinnedModel, skinnedPlayer.get(), skinnedRootPlayer.get());

        Game::Draw(gameTime);

        DrawHUD();
    }

    void CustomAvatarAnimationSampleGame::DrawHUD()
    {
        std::string controls = "Controls: \n";
        controls += "Press A to Play the Rigid Animation\n";
        controls += "Press B to Play the Skinned Animation\n";

        std::string status = "Animation Status:\n";
        status += "Rigid: ";
        if (playingRigid == false)
            status += "Stopped\n";
        else
            status += "Playing\n";
        status += "Skinned: ";
        if (playingSkinned == false)
            status += "Stopped\n";
        else
            status += "Playing\n";

        spriteBatch->Begin();
        spriteBatch->DrawString(*font, controls, Vector2(100, 80), Color::White);
        spriteBatch->DrawString(*font, status, Vector2(100, 200), Color::White);
        spriteBatch->End();
    }

    void CustomAvatarAnimationSampleGame::DrawSkinnedModel(
        Model& model, SkinnedAnimationPlayer* skinnedAnimationPlayer,
        RootAnimationPlayer* rootAnimationPlayer)
    {
        const std::vector<Matrix>* boneTransforms = nullptr;
        if (skinnedAnimationPlayer != nullptr)
            boneTransforms = &skinnedAnimationPlayer->GetSkinTransforms();

        Matrix rootTransform = Matrix::getIdentityProperty();
        if (rootAnimationPlayer != nullptr)
            rootTransform = rootAnimationPlayer->GetCurrentTransform();

        for (ModelMesh* mesh : model.getMeshesProperty())
        {
            for (Effect* meshEffect : mesh->getEffectsProperty())
            {
                // The original's foreach is typed `SkinnedEffect`, and a C# cast-per-element loop
                // throws on a mismatch, so this does too.
                auto* effect = dynamic_cast<SkinnedEffect*>(meshEffect);
                if (effect == nullptr)
                {
                    throw System::InvalidCastException(
                        "CustomModelAnimation: a skinned mesh effect is not a SkinnedEffect.");
                }

                effect->EnableDefaultLighting();
                effect->setProjectionProperty(projection);
                effect->setViewProperty(view);
                if (boneTransforms != nullptr)
                    effect->SetBoneTransforms(*boneTransforms);
                effect->setWorldProperty(rootTransform * skinnedWorld);
                effect->setSpecularColorProperty(Vector3::Zero);
            }

            mesh->Draw();
        }
    }

    void CustomAvatarAnimationSampleGame::DrawRigidModel(
        Model& model, RigidAnimationPlayer* rigidAnimationPlayer,
        RootAnimationPlayer* rootAnimationPlayer)
    {
        const std::vector<Matrix>* boneTransforms = nullptr;
        if (rigidAnimationPlayer != nullptr)
            boneTransforms = &rigidAnimationPlayer->GetBoneTransforms();

        Matrix rootTransform = Matrix::getIdentityProperty();
        if (rootAnimationPlayer != nullptr)
            rootTransform = rootAnimationPlayer->GetCurrentTransform();

        for (ModelMesh* mesh : model.getMeshesProperty())
        {
            for (Effect* meshEffect : mesh->getEffectsProperty())
            {
                // The original's foreach is typed `BasicEffect`, and a C# cast-per-element loop
                // throws on a mismatch, so this does too.
                auto* effect = dynamic_cast<BasicEffect*>(meshEffect);
                if (effect == nullptr)
                {
                    throw System::InvalidCastException(
                        "CustomModelAnimation: a rigid mesh effect is not a BasicEffect.");
                }

                effect->EnableDefaultLighting();
                effect->setProjectionProperty(projection);
                effect->setViewProperty(view);
                if (boneTransforms != nullptr)
                    effect->setWorldProperty(
                        (*boneTransforms)[static_cast<std::size_t>(
                            mesh->getParentBoneProperty()->getIndexProperty())] *
                        rootTransform * rigidWorld);
                else
                    effect->setWorldProperty(rootTransform * rigidWorld);
            }

            mesh->Draw();
        }
    }
}

/**
 * @brief The main entry point for the application.
 *
 * The original declares this as `static void Main(string[] args)` at the bottom of
 * CustomModelAnimationSample.cs.
 *
 * @return The process exit code.
 */
int main()
{
    CustomAvatarAnimationSample::CustomAvatarAnimationSampleGame game;
    game.Run();
    return 0;
}
