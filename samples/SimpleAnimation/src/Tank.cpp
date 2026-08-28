// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Tank.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Tank.hpp"

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "System/InvalidCastException.hpp"

namespace SimpleAnimation
{
    using Microsoft::Xna::Framework::Graphics::BasicEffect;
    using Microsoft::Xna::Framework::Graphics::Effect;
    using Microsoft::Xna::Framework::Graphics::ModelMesh;

    void Tank::Load(ContentManager& content)
    {
        // Load the tank model from the ContentManager.
        tankModel = content.Load<Model>("tank");

        // Look up shortcut references to the bones we are going to animate.
        leftBackWheelBone = tankModel->getBonesProperty()["l_back_wheel_geo"];
        rightBackWheelBone = tankModel->getBonesProperty()["r_back_wheel_geo"];
        leftFrontWheelBone = tankModel->getBonesProperty()["l_front_wheel_geo"];
        rightFrontWheelBone = tankModel->getBonesProperty()["r_front_wheel_geo"];
        leftSteerBone = tankModel->getBonesProperty()["l_steer_geo"];
        rightSteerBone = tankModel->getBonesProperty()["r_steer_geo"];
        turretBone = tankModel->getBonesProperty()["turret_geo"];
        cannonBone = tankModel->getBonesProperty()["canon_geo"];
        hatchBone = tankModel->getBonesProperty()["hatch_geo"];

        // Store the original transform matrix for each animating bone.
        leftBackWheelTransform = leftBackWheelBone->getTransformProperty();
        rightBackWheelTransform = rightBackWheelBone->getTransformProperty();
        leftFrontWheelTransform = leftFrontWheelBone->getTransformProperty();
        rightFrontWheelTransform = rightFrontWheelBone->getTransformProperty();
        leftSteerTransform = leftSteerBone->getTransformProperty();
        rightSteerTransform = rightSteerBone->getTransformProperty();
        turretTransform = turretBone->getTransformProperty();
        cannonTransform = cannonBone->getTransformProperty();
        hatchTransform = hatchBone->getTransformProperty();

        // Allocate the transform matrix array.
        boneTransforms.resize(
            static_cast<std::size_t>(tankModel->getBonesProperty().getCountProperty()));
    }

    void Tank::Draw(const Matrix& world, const Matrix& view, const Matrix& projection)
    {
        // Set the world matrix as the root transform of the model.
        tankModel->getRootProperty()->setTransformProperty(world);

        // Calculate matrices based on the current animation position.
        const Matrix wheelRotation = Matrix::CreateRotationX(wheelRotationValue);
        const Matrix steerRotation = Matrix::CreateRotationY(steerRotationValue);
        const Matrix turretRotation = Matrix::CreateRotationY(turretRotationValue);
        const Matrix cannonRotation = Matrix::CreateRotationX(cannonRotationValue);
        const Matrix hatchRotation = Matrix::CreateRotationX(hatchRotationValue);

        // Apply matrices to the relevant bones. See the Simple Animation Sample
        // documentation for a detailed explanation of each step here.
        leftBackWheelBone->setTransformProperty(wheelRotation * leftBackWheelTransform);
        rightBackWheelBone->setTransformProperty(wheelRotation * rightBackWheelTransform);
        leftFrontWheelBone->setTransformProperty(wheelRotation * leftFrontWheelTransform);
        rightFrontWheelBone->setTransformProperty(wheelRotation * rightFrontWheelTransform);
        leftSteerBone->setTransformProperty(steerRotation * leftSteerTransform);
        rightSteerBone->setTransformProperty(steerRotation * rightSteerTransform);
        turretBone->setTransformProperty(turretRotation * turretTransform);
        cannonBone->setTransformProperty(cannonRotation * cannonTransform);
        hatchBone->setTransformProperty(hatchRotation * hatchTransform);

        // Look up combined bone matrices for the entire model.
        tankModel->CopyAbsoluteBoneTransformsTo(boneTransforms);

        // Draw the model.
        for (ModelMesh* mesh : tankModel->getMeshesProperty())
        {
            for (Effect* meshEffect : mesh->getEffectsProperty())
            {
                // The original's foreach is typed `BasicEffect`, and a C# cast-per-element loop
                // throws on a mismatch, so this does too.
                auto* effect = dynamic_cast<BasicEffect*>(meshEffect);
                if (effect == nullptr)
                {
                    throw System::InvalidCastException(
                        "SimpleAnimation: a mesh effect is not a BasicEffect.");
                }

                effect->setWorldProperty(
                    boneTransforms[static_cast<std::size_t>(
                        mesh->getParentBoneProperty()->getIndexProperty())]);
                effect->setViewProperty(view);
                effect->setProjectionProperty(projection);

                effect->EnableDefaultLighting();
            }

            mesh->Draw();
        }
    }
}
