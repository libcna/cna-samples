// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// HeightmapCollision.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "HeightmapCollisionGame.hpp"

#include <memory>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"
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
#include "System/InvalidCastException.hpp"
#include "System/InvalidOperationException.hpp"

namespace HeightmapCollision
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

    const Vector3 HeightmapCollisionGame::CameraPositionOffset(0, 40, 150);
    const Vector3 HeightmapCollisionGame::CameraTargetOffset(0, 30, 0);

    HeightmapCollisionGame::HeightmapCollisionGame()
        : graphics(this)
    {
        getContentProperty().setRootDirectoryProperty("Content");

        // CNAEXT — not a line the original needs. The terrain .xnb names its Tag's reader as
        // "HeightmapCollision.HeightMapInfoReader", and XNA finds a ContentTypeReader by that name
        // by reflecting over the game assembly. C++ has no reflection, so the game says which
        // reader answers to the name. The reader itself is a faithful translation of the original's
        // own HeightMapInfoReader; only the way it is found differs. See diff.md.
        CNAEXT ContentTypeReaderManager::AddTypeCreator(
            "HeightmapCollision.HeightMapInfoReader",
            [] { return std::make_unique<HeightMapInfoReader>(); });
    }

    const std::string& HeightmapCollisionGame::GetTypeName() const
    {
        static const std::string name = "HeightmapCollision.HeightmapCollisionGame";
        return name;
    }

    void HeightmapCollisionGame::Initialize()
    {
        projectionMatrix = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::ToRadians(45.0f),
            getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(),
            1.0f, 10000);

        Game::Initialize();
    }

    void HeightmapCollisionGame::LoadContent()
    {
        terrain = getContentProperty().Load<Model>("terrain");

        // The terrain processor attached a HeightMapInfo to the model's Tag. `as` is a checked
        // cast that yields null on a mismatch, which dynamic_cast does too.
        heightMapInfo = dynamic_cast<HeightMapInfo*>(terrain->getTagProperty());
        if (heightMapInfo == nullptr)
        {
            throw System::InvalidOperationException(
                "The terrain model did not have a HeightMapInfo object attached. "
                "Are you sure you are using the TerrainProcessor?");
        }

        sphere = getContentProperty().Load<Model>("sphere");
    }

    void HeightmapCollisionGame::Update(GameTime& gameTime)
    {
        HandleInput();
        UpdateCamera();

        Game::Update(gameTime);
    }

    void HeightmapCollisionGame::UpdateCamera()
    {
        // The camera's position depends on the sphere's facing direction: when the sphere turns,
        // the camera needs to stay behind it. So, we'll calculate a rotation matrix using the
        // sphere's facing direction, and use it to transform the two offset vectors that control
        // the camera.
        const Matrix cameraFacingMatrix = Matrix::CreateRotationY(sphereFacingDirection);
        const Vector3 positionOffset = Vector3::Transform(CameraPositionOffset, cameraFacingMatrix);
        const Vector3 targetOffset = Vector3::Transform(CameraTargetOffset, cameraFacingMatrix);

        // once we've transformed the camera's position offset vector, it's easy to
        // figure out where we think the camera should be.
        Vector3 cameraPosition = spherePosition + positionOffset;

        // We don't want the camera to go beneath the heightmap, so if the camera is
        // over the terrain, we'll move it up.
        if (heightMapInfo->IsOnHeightmap(cameraPosition))
        {
            // we don't want the camera to go beneath the terrain's height +
            // a small offset.
            const float minimumHeight =
                heightMapInfo->GetHeight(cameraPosition) + CameraPositionOffset.Y;

            if (cameraPosition.Y < minimumHeight)
            {
                cameraPosition.Y = minimumHeight;
            }
        }

        // next, we need to calculate the point that the camera is aiming it. That's
        // simple enough - the camera is aiming at the sphere, and has to take the
        // targetOffset into account.
        const Vector3 cameraTarget = spherePosition + targetOffset;

        // with those values, we'll calculate the viewMatrix.
        viewMatrix = Matrix::CreateLookAt(cameraPosition, cameraTarget, Vector3::Up);
    }

    void HeightmapCollisionGame::Draw(const GameTime& gameTime)
    {
        GraphicsDevice& device = *graphics.getGraphicsDeviceProperty();

        device.Clear(Color::Black);

        DrawModel(*terrain, Matrix::getIdentityProperty());
        DrawModel(*sphere, sphereRollingMatrix * Matrix::CreateTranslation(spherePosition));

        Game::Draw(gameTime);
    }

    void HeightmapCollisionGame::DrawModel(Model& model, const Matrix& worldMatrix)
    {
        std::vector<Matrix> boneTransforms(
            static_cast<std::size_t>(model.getBonesProperty().getCountProperty()));
        model.CopyAbsoluteBoneTransformsTo(boneTransforms);

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
                        "HeightmapCollision: a mesh effect is not a BasicEffect.");
                }

                effect->setWorldProperty(
                    boneTransforms[static_cast<std::size_t>(
                        mesh->getParentBoneProperty()->getIndexProperty())]
                    * worldMatrix);
                effect->setViewProperty(viewMatrix);
                effect->setProjectionProperty(projectionMatrix);

                effect->EnableDefaultLighting();
                effect->setPreferPerPixelLightingProperty(true);

                // Set the fog to match the black background color
                effect->setFogEnabledProperty(true);
                effect->setFogColorProperty(Vector3::Zero);
                effect->setFogStartProperty(1000);
                effect->setFogEndProperty(3200);
            }

            mesh->Draw();
        }
    }

    void HeightmapCollisionGame::HandleInput()
    {
        const KeyboardState currentKeyboardState = Keyboard::GetState();
        const GamePadState currentGamePadState = GamePad::GetState(PlayerIndex::One);

        // Check for exit.
        if (currentKeyboardState.IsKeyDown(Keys::Escape)
            || currentGamePadState.getButtonsProperty().getBackProperty() == ButtonState::Pressed)
        {
            Exit();
        }

        // Now move the sphere. First, we want to check to see if the sphere should
        // turn. turnAmount will be an accumulation of all the different possible
        // inputs.
        float turnAmount = -currentGamePadState.getThumbSticksProperty().getLeftProperty().X;
        if (currentKeyboardState.IsKeyDown(Keys::A)
            || currentKeyboardState.IsKeyDown(Keys::Left)
            || currentGamePadState.getDPadProperty().getLeftProperty() == ButtonState::Pressed)
        {
            turnAmount += 1;
        }
        if (currentKeyboardState.IsKeyDown(Keys::D)
            || currentKeyboardState.IsKeyDown(Keys::Right)
            || currentGamePadState.getDPadProperty().getRightProperty() == ButtonState::Pressed)
        {
            turnAmount -= 1;
        }

        // clamp the turn amount between -1 and 1, and then use the finished
        // value to turn the sphere.
        turnAmount = MathHelper::Clamp(turnAmount, -1.0f, 1.0f);
        sphereFacingDirection += turnAmount * SphereTurnSpeed;

        // Next, we want to move the sphere forward or back. to do this,
        // we'll create a Vector3 and modify use the user's input to modify the Z
        // component, which corresponds to the forward direction.
        Vector3 movement = Vector3::Zero;
        movement.Z = -currentGamePadState.getThumbSticksProperty().getLeftProperty().Y;

        if (currentKeyboardState.IsKeyDown(Keys::W)
            || currentKeyboardState.IsKeyDown(Keys::Up)
            || currentGamePadState.getDPadProperty().getUpProperty() == ButtonState::Pressed)
        {
            movement.Z = -1;
        }
        if (currentKeyboardState.IsKeyDown(Keys::S)
            || currentKeyboardState.IsKeyDown(Keys::Down)
            || currentGamePadState.getDPadProperty().getDownProperty() == ButtonState::Pressed)
        {
            movement.Z = 1;
        }

        const Matrix sphereFacingMatrix = Matrix::CreateRotationY(sphereFacingDirection);
        Vector3 velocity = Vector3::Transform(movement, sphereFacingMatrix);
        velocity *= SphereVelocity;

        // Now we know how much the user wants to move. We'll construct a temporary
        // vector, newSpherePosition, which will represent where the user wants to
        // go. If that value is on the heightmap, we'll allow the move.
        Vector3 newSpherePosition = spherePosition + velocity;
        if (heightMapInfo->IsOnHeightmap(newSpherePosition))
        {
            // finally, we need to see how high the terrain is at the sphere's new
            // position. GetHeight will give us that information, which is offset by
            // the radius of the sphere. If we didn't offset by the radius, the
            // sphere would be half buried in the terrain.
            newSpherePosition.Y = heightMapInfo->GetHeight(newSpherePosition) + SphereRadius;
        }
        else
        {
            // If the new position is outside the heightmap, don't allow the move.
            newSpherePosition = spherePosition;
        }

        // now we need to roll the ball "forward". to do this, we first calculate
        // how far it has moved.
        const float distanceMoved = Vector3::Distance(spherePosition, newSpherePosition);

        // The length of an arc on a circle or sphere is defined as L = theta * r,
        // where theta is the angle that defines the arc, and r is the radius of
        // the circle.
        // we know L, that's the distance the sphere has moved. we know r, that's
        // our constant "sphereRadius". We want to know theta - that will tell us
        // how much to rotate the sphere.
        const float theta = distanceMoved / SphereRadius;
        const int rollDirection = movement.Z > 0 ? 1 : -1;

        // create a matrix that will rotate the sphere the correct amount.
        sphereRollingMatrix *= Matrix::CreateFromAxisAngle(
            sphereFacingMatrix.getRightProperty(), theta * static_cast<float>(rollDirection));

        // and then move the sphere.
        spherePosition = newSpherePosition;
    }
}

/**
 * @brief The main entry point for the application.
 *
 * The original declares this as a `static class Program` at the bottom of HeightmapCollision.cs,
 * with no platform guard around it.
 *
 * @return The process exit code.
 */
int main()
{
    HeightmapCollision::HeightmapCollisionGame game;
    game.Run();
    return 0;
}
