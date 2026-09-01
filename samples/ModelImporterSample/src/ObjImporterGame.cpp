// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Game.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ObjImporterGame.hpp"

#include <cstddef>
#include <vector>

#include "CNA/Platform/Entrypoint.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

namespace ObjImporterSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    ObjImporterGame::ObjImporterGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");
    }

    const std::string& ObjImporterGame::GetTypeName() const
    {
        static const std::string name = "ObjImporterSample.ObjImporterGame";
        return name;
    }

    void ObjImporterGame::LoadContent()
    {
        model.emplace(getContentProperty().Load<Model>("Tank"));
    }

    void ObjImporterGame::Update(GameTime& gameTime)
    {
        Game::Update(gameTime);

        const KeyboardState keyboardState = Keyboard::GetState();
        const GamePadState gamePadState = GamePad::GetState(PlayerIndex::One);

        if (keyboardState.IsKeyDown(Keys::Escape)
            || gamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
        {
            Exit();
        }
    }

    void ObjImporterGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();
        device.Clear(Color::CornflowerBlue);

        const float modelRotation = static_cast<float>(
            gameTime.getTotalGameTimeProperty().getTotalSecondsProperty()) / 5.0f;

        const Vector3 cameraPosition(0.0f, 200.0f, 350.0f);
        const Vector3 lookAt(0.0f, 35.0f, 0.0f);

        std::vector<Matrix> transforms(
            static_cast<std::size_t>(model->getBonesProperty().getCountProperty()));
        model->CopyAbsoluteBoneTransformsTo(transforms);

        for (ModelMesh* mesh : model->getMeshesProperty())
        {
            for (Effect* effect : mesh->getEffectsProperty())
            {
                auto* basicEffect = static_cast<BasicEffect*>(effect);
                basicEffect->EnableDefaultLighting();
                basicEffect->setWorldProperty(
                    transforms[static_cast<std::size_t>(
                        mesh->getParentBoneProperty()->getIndexProperty())]
                    * Matrix::CreateRotationY(modelRotation));
                basicEffect->setViewProperty(
                    Matrix::CreateLookAt(cameraPosition, lookAt, Vector3::Up));
                basicEffect->setProjectionProperty(Matrix::CreatePerspectiveFieldOfView(
                    MathHelper::ToRadians(45.0f),
                    device.getViewportProperty().getAspectRatioProperty(),
                    1.0f,
                    10000.0f));
            }

            mesh->Draw();
        }

        Game::Draw(gameTime);
    }
}

/**
 * @brief Runs the custom model importer demonstration.
 *
 * @return The process exit code.
 */
int main()
{
    ObjImporterSample::ObjImporterGame game;
    game.Run();
    return 0;
}
