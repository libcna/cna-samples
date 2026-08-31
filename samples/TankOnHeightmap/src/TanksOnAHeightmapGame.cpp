// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// TanksOnAHeightmap.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "TanksOnAHeightmapGame.hpp"

#include <cstddef>
#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/InvalidCastException.hpp"
#include "System/InvalidOperationException.hpp"

namespace TanksOnAHeightmap
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::PlayerIndex;
    using Microsoft::Xna::Framework::Content::ContentTypeReaderManager;
    using Microsoft::Xna::Framework::Graphics::BasicEffect;
    using Microsoft::Xna::Framework::Graphics::Effect;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Graphics::ModelMesh;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::GamePad;
    using Microsoft::Xna::Framework::Input::GamePadState;
    using Microsoft::Xna::Framework::Input::Keyboard;
    using Microsoft::Xna::Framework::Input::KeyboardState;
    using Microsoft::Xna::Framework::Input::Keys;

    const Vector3 TanksOnAHeightmapGame::CameraPositionOffset(0, 40, 150);
    const Vector3 TanksOnAHeightmapGame::CameraTargetOffset(0, 30, 0);

    TanksOnAHeightmapGame::TanksOnAHeightmapGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        // CNAEXT — XNA discovers the sample's ContentTypeReader through assembly reflection.
        // CNA deliberately has no reflection, so the sample registers the faithfully ported
        // reader under the exact name carried by the authentic XNB.
        CNAEXT ContentTypeReaderManager::AddTypeCreator(
            "TanksOnAHeightmap.HeightMapInfoReader",
            [] { return std::make_unique<HeightMapInfoReader>(); });
    }

    const std::string& TanksOnAHeightmapGame::GetTypeName() const
    {
        static const std::string name =
            "TanksOnAHeightmap.TanksOnAHeightmapGame";
        return name;
    }

    void TanksOnAHeightmapGame::Initialize()
    {
        projectionMatrix = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::ToRadians(45.0f),
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(),
            1.0f,
            10000);

        Game::Initialize();
    }

    void TanksOnAHeightmapGame::LoadContent()
    {
        terrain = getContentProperty().Load<Model>("terrain");

        heightMapInfo = dynamic_cast<HeightMapInfo*>(terrain->getTagProperty());
        if (heightMapInfo == nullptr)
        {
            throw System::InvalidOperationException(
                "The terrain model did not have a HeightMapInfo object attached. "
                "Are you sure you are using the TerrainProcessor?");
        }

        tank.LoadContent(getContentProperty());
    }

    void TanksOnAHeightmapGame::Update(GameTime& gameTime)
    {
        HandleInput();
        UpdateCamera();

        Game::Update(gameTime);
    }

    void TanksOnAHeightmapGame::UpdateCamera()
    {
        const Matrix cameraFacingMatrix =
            Matrix::CreateRotationY(tank.getFacingDirectionProperty());
        const Vector3 positionOffset =
            Vector3::Transform(CameraPositionOffset, cameraFacingMatrix);
        const Vector3 targetOffset =
            Vector3::Transform(CameraTargetOffset, cameraFacingMatrix);

        Vector3 cameraPosition = tank.getPositionProperty() + positionOffset;

        if (heightMapInfo->IsOnHeightmap(cameraPosition))
        {
            float minimumHeight;
            Vector3 normal;
            heightMapInfo->GetHeightAndNormal(
                cameraPosition, minimumHeight, normal);

            minimumHeight += CameraPositionOffset.Y;
            if (cameraPosition.Y < minimumHeight)
            {
                cameraPosition.Y = minimumHeight;
            }
        }

        const Vector3 cameraTarget = tank.getPositionProperty() + targetOffset;
        viewMatrix = Matrix::CreateLookAt(
            cameraPosition, cameraTarget, Vector3::Up);
    }

    void TanksOnAHeightmapGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();
        device.Clear(Color::Black);

        DrawModel(*terrain);
        tank.Draw(viewMatrix, projectionMatrix);

        Game::Draw(gameTime);
    }

    void TanksOnAHeightmapGame::DrawModel(Model& model)
    {
        std::vector<Matrix> boneTransforms(
            static_cast<std::size_t>(model.getBonesProperty().getCountProperty()));
        model.CopyAbsoluteBoneTransformsTo(boneTransforms);

        for (ModelMesh* mesh : model.getMeshesProperty())
        {
            for (Effect* meshEffect : mesh->getEffectsProperty())
            {
                auto* effect = dynamic_cast<BasicEffect*>(meshEffect);
                if (effect == nullptr)
                {
                    throw System::InvalidCastException(
                        "TanksOnAHeightmap: a terrain mesh effect is not a BasicEffect.");
                }

                effect->setWorldProperty(
                    boneTransforms[static_cast<std::size_t>(
                        mesh->getParentBoneProperty()->getIndexProperty())]);
                effect->setViewProperty(viewMatrix);
                effect->setProjectionProperty(projectionMatrix);

                effect->EnableDefaultLighting();
                effect->setPreferPerPixelLightingProperty(true);
                effect->setFogEnabledProperty(true);
                effect->setFogColorProperty(Vector3::Zero);
                effect->setFogStartProperty(1000);
                effect->setFogEndProperty(3200);
            }

            mesh->Draw();
        }
    }

    void TanksOnAHeightmapGame::HandleInput()
    {
        const KeyboardState currentKeyboardState = Keyboard::GetState();
        const GamePadState currentGamePadState = GamePad::GetState(PlayerIndex::One);

        if (currentKeyboardState.IsKeyDown(Keys::Escape)
            || currentGamePadState.getButtonsProperty().getBackProperty()
                == ButtonState::Pressed)
        {
            Exit();
        }

        tank.HandleInput(
            currentGamePadState, currentKeyboardState, *heightMapInfo);
    }
}

/**
 * @brief The main entry point for the application.
 * @return The process exit code.
 */
int main()
{
    TanksOnAHeightmap::TanksOnAHeightmapGame game;
    game.Run();
    return 0;
}
