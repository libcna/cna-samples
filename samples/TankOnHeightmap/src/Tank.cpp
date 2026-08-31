// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Tank.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Tank.hpp"

#include <cstddef>
#include <vector>

#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "System/InvalidCastException.hpp"

namespace TanksOnAHeightmap
{
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Graphics::BasicEffect;
    using Microsoft::Xna::Framework::Graphics::Effect;
    using Microsoft::Xna::Framework::Graphics::ModelMesh;
    using Microsoft::Xna::Framework::Input::ButtonState;
    using Microsoft::Xna::Framework::Input::Keys;

    const Vector3& Tank::getPositionProperty() const
    {
        return position;
    }

    float Tank::getFacingDirectionProperty() const
    {
        return facingDirection;
    }

    void Tank::LoadContent(ContentManager& content)
    {
        model = content.Load<Model>("Tank");

        leftBackWheelBone = model->getBonesProperty()["l_back_wheel_geo"];
        rightBackWheelBone = model->getBonesProperty()["r_back_wheel_geo"];
        leftFrontWheelBone = model->getBonesProperty()["l_front_wheel_geo"];
        rightFrontWheelBone = model->getBonesProperty()["r_front_wheel_geo"];

        leftBackWheelTransform = leftBackWheelBone->getTransformProperty();
        rightBackWheelTransform = rightBackWheelBone->getTransformProperty();
        leftFrontWheelTransform = leftFrontWheelBone->getTransformProperty();
        rightFrontWheelTransform = rightFrontWheelBone->getTransformProperty();
    }

    void Tank::HandleInput(
        const GamePadState& currentGamePadState,
        const KeyboardState& currentKeyboardState,
        const HeightMapInfo& heightMapInfo)
    {
        float turnAmount =
            -currentGamePadState.getThumbSticksProperty().getLeftProperty().X;
        if (currentKeyboardState.IsKeyDown(Keys::A)
            || currentKeyboardState.IsKeyDown(Keys::Left)
            || currentGamePadState.getDPadProperty().getLeftProperty()
                == ButtonState::Pressed)
        {
            turnAmount += 1;
        }

        if (currentKeyboardState.IsKeyDown(Keys::D)
            || currentKeyboardState.IsKeyDown(Keys::Right)
            || currentGamePadState.getDPadProperty().getRightProperty()
                == ButtonState::Pressed)
        {
            turnAmount -= 1;
        }

        turnAmount = MathHelper::Clamp(turnAmount, -1.0f, 1.0f);
        facingDirection += turnAmount * TankTurnSpeed;

        Vector3 movement = Vector3::Zero;
        movement.Z =
            -currentGamePadState.getThumbSticksProperty().getLeftProperty().Y;

        if (currentKeyboardState.IsKeyDown(Keys::W)
            || currentKeyboardState.IsKeyDown(Keys::Up)
            || currentGamePadState.getDPadProperty().getUpProperty()
                == ButtonState::Pressed)
        {
            movement.Z = -1;
        }
        if (currentKeyboardState.IsKeyDown(Keys::S)
            || currentKeyboardState.IsKeyDown(Keys::Down)
            || currentGamePadState.getDPadProperty().getDownProperty()
                == ButtonState::Pressed)
        {
            movement.Z = 1;
        }

        orientation = Matrix::CreateRotationY(getFacingDirectionProperty());
        Vector3 velocity = Vector3::Transform(movement, orientation);
        velocity *= TankVelocity;

        Vector3 newPosition = getPositionProperty() + velocity;
        if (heightMapInfo.IsOnHeightmap(newPosition))
        {
            Vector3 normal;
            heightMapInfo.GetHeightAndNormal(newPosition, newPosition.Y, normal);

            orientation.setUpProperty(normal);

            Vector3 right = Vector3::Cross(
                orientation.getForwardProperty(), orientation.getUpProperty());
            right.Normalize();
            orientation.setRightProperty(right);

            Vector3 forward = Vector3::Cross(
                orientation.getUpProperty(), orientation.getRightProperty());
            forward.Normalize();
            orientation.setForwardProperty(forward);

            const float distanceMoved = Vector3::Distance(getPositionProperty(), newPosition);
            const float theta = distanceMoved / TankWheelRadius;
            const int rollDirection = movement.Z > 0 ? 1 : -1;

            wheelRollMatrix *= Matrix::CreateRotationX(
                theta * static_cast<float>(rollDirection));

            position = newPosition;
        }
    }

    void Tank::Draw(const Matrix& viewMatrix, const Matrix& projectionMatrix)
    {
        leftBackWheelBone->setTransformProperty(
            wheelRollMatrix * leftBackWheelTransform);
        rightBackWheelBone->setTransformProperty(
            wheelRollMatrix * rightBackWheelTransform);
        leftFrontWheelBone->setTransformProperty(
            wheelRollMatrix * leftFrontWheelTransform);
        rightFrontWheelBone->setTransformProperty(
            wheelRollMatrix * rightFrontWheelTransform);

        std::vector<Matrix> boneTransforms(
            static_cast<std::size_t>(model->getBonesProperty().getCountProperty()));
        model->CopyAbsoluteBoneTransformsTo(boneTransforms);

        const Matrix worldMatrix =
            orientation * Matrix::CreateTranslation(getPositionProperty());

        for (ModelMesh* mesh : model->getMeshesProperty())
        {
            for (Effect* meshEffect : mesh->getEffectsProperty())
            {
                auto* effect = dynamic_cast<BasicEffect*>(meshEffect);
                if (effect == nullptr)
                {
                    throw System::InvalidCastException(
                        "TanksOnAHeightmap: a mesh effect is not a BasicEffect.");
                }

                effect->setWorldProperty(
                    boneTransforms[static_cast<std::size_t>(
                        mesh->getParentBoneProperty()->getIndexProperty())]
                    * worldMatrix);
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
}
