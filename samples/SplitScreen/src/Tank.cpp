// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Tank.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Tank.hpp"

#include <cstddef>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "System/InvalidCastException.hpp"

namespace SplitScreenSample
{
    using Microsoft::Xna::Framework::Graphics::BasicEffect;
    using Microsoft::Xna::Framework::Graphics::Effect;
    using Microsoft::Xna::Framework::Graphics::ModelMesh;

    float Tank::getWheelRotationProperty() const
    {
        return wheelRotationValue;
    }

    void Tank::setWheelRotationProperty(float value)
    {
        wheelRotationValue = value;
    }

    float Tank::getSteerRotationProperty() const
    {
        return steerRotationValue;
    }

    void Tank::setSteerRotationProperty(float value)
    {
        steerRotationValue = value;
    }

    float Tank::getTurretRotationProperty() const
    {
        return turretRotationValue;
    }

    void Tank::setTurretRotationProperty(float value)
    {
        turretRotationValue = value;
    }

    float Tank::getCannonRotationProperty() const
    {
        return cannonRotationValue;
    }

    void Tank::setCannonRotationProperty(float value)
    {
        cannonRotationValue = value;
    }

    float Tank::getHatchRotationProperty() const
    {
        return hatchRotationValue;
    }

    void Tank::setHatchRotationProperty(float value)
    {
        hatchRotationValue = value;
    }

    void Tank::Load(ContentManager& content)
    {
        tankModel = content.Load<Model>("tank");

        leftBackWheelBone = tankModel->getBonesProperty()["l_back_wheel_geo"];
        rightBackWheelBone = tankModel->getBonesProperty()["r_back_wheel_geo"];
        leftFrontWheelBone = tankModel->getBonesProperty()["l_front_wheel_geo"];
        rightFrontWheelBone = tankModel->getBonesProperty()["r_front_wheel_geo"];
        leftSteerBone = tankModel->getBonesProperty()["l_steer_geo"];
        rightSteerBone = tankModel->getBonesProperty()["r_steer_geo"];
        turretBone = tankModel->getBonesProperty()["turret_geo"];
        cannonBone = tankModel->getBonesProperty()["canon_geo"];
        hatchBone = tankModel->getBonesProperty()["hatch_geo"];

        leftBackWheelTransform = leftBackWheelBone->getTransformProperty();
        rightBackWheelTransform = rightBackWheelBone->getTransformProperty();
        leftFrontWheelTransform = leftFrontWheelBone->getTransformProperty();
        rightFrontWheelTransform = rightFrontWheelBone->getTransformProperty();
        leftSteerTransform = leftSteerBone->getTransformProperty();
        rightSteerTransform = rightSteerBone->getTransformProperty();
        turretTransform = turretBone->getTransformProperty();
        cannonTransform = cannonBone->getTransformProperty();
        hatchTransform = hatchBone->getTransformProperty();

        boneTransforms.resize(static_cast<std::size_t>(
            tankModel->getBonesProperty().getCountProperty()));
    }

    void Tank::Draw(const Matrix& world, const Matrix& view, const Matrix& projection)
    {
        tankModel->getRootProperty()->setTransformProperty(world);

        const Matrix wheelRotation = Matrix::CreateRotationX(wheelRotationValue);
        const Matrix steerRotation = Matrix::CreateRotationY(steerRotationValue);
        const Matrix turretRotation = Matrix::CreateRotationY(turretRotationValue);
        const Matrix cannonRotation = Matrix::CreateRotationX(cannonRotationValue);
        const Matrix hatchRotation = Matrix::CreateRotationX(hatchRotationValue);

        leftBackWheelBone->setTransformProperty(wheelRotation * leftBackWheelTransform);
        rightBackWheelBone->setTransformProperty(wheelRotation * rightBackWheelTransform);
        leftFrontWheelBone->setTransformProperty(wheelRotation * leftFrontWheelTransform);
        rightFrontWheelBone->setTransformProperty(wheelRotation * rightFrontWheelTransform);
        leftSteerBone->setTransformProperty(steerRotation * leftSteerTransform);
        rightSteerBone->setTransformProperty(steerRotation * rightSteerTransform);
        turretBone->setTransformProperty(turretRotation * turretTransform);
        cannonBone->setTransformProperty(cannonRotation * cannonTransform);
        hatchBone->setTransformProperty(hatchRotation * hatchTransform);

        tankModel->CopyAbsoluteBoneTransformsTo(boneTransforms);

        for (ModelMesh* mesh : tankModel->getMeshesProperty())
        {
            for (Effect* meshEffect : mesh->getEffectsProperty())
            {
                auto* effect = dynamic_cast<BasicEffect*>(meshEffect);
                if (effect == nullptr)
                {
                    throw System::InvalidCastException(
                        "SplitScreenSample: a tank mesh effect is not a BasicEffect.");
                }

                effect->setWorldProperty(boneTransforms[static_cast<std::size_t>(
                    mesh->getParentBoneProperty()->getIndexProperty())]);
                effect->setViewProperty(view);
                effect->setProjectionProperty(projection);
                effect->EnableDefaultLighting();
            }

            mesh->Draw();
        }
    }
}
