#pragma once

#include <cmath>
#include <optional>
#include <stdexcept>
#include <vector>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace ReachGraphicsDemoSample {

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Content;
using namespace Microsoft::Xna::Framework::Graphics;

enum class LightingMode {
    NoLighting,
    OneVertexLight,
    ThreeVertexLights,
    ThreePixelLights,
};

class Tank {
public:
    float WheelRotation = 0.0f;
    float SteerRotation = 0.0f;
    float TurretRotation = 0.0f;
    float CannonRotation = 0.0f;
    float HatchRotation = 0.0f;

    void Load(ContentManager& content) {
        tankModel_ = content.Load<Model>("tank");

        leftBackWheelBone_ = tankModel_->getBonesProperty()["l_back_wheel_geo"];
        rightBackWheelBone_ = tankModel_->getBonesProperty()["r_back_wheel_geo"];
        leftFrontWheelBone_ = tankModel_->getBonesProperty()["l_front_wheel_geo"];
        rightFrontWheelBone_ = tankModel_->getBonesProperty()["r_front_wheel_geo"];
        leftSteerBone_ = tankModel_->getBonesProperty()["l_steer_geo"];
        rightSteerBone_ = tankModel_->getBonesProperty()["r_steer_geo"];
        turretBone_ = tankModel_->getBonesProperty()["turret_geo"];
        cannonBone_ = tankModel_->getBonesProperty()["canon_geo"];
        hatchBone_ = tankModel_->getBonesProperty()["hatch_geo"];

        leftBackWheelTransform_ = leftBackWheelBone_->getTransformProperty();
        rightBackWheelTransform_ = rightBackWheelBone_->getTransformProperty();
        leftFrontWheelTransform_ = leftFrontWheelBone_->getTransformProperty();
        rightFrontWheelTransform_ = rightFrontWheelBone_->getTransformProperty();
        leftSteerTransform_ = leftSteerBone_->getTransformProperty();
        rightSteerTransform_ = rightSteerBone_->getTransformProperty();
        turretTransform_ = turretBone_->getTransformProperty();
        cannonTransform_ = cannonBone_->getTransformProperty();
        hatchTransform_ = hatchBone_->getTransformProperty();

        boneTransforms_.resize(
            static_cast<std::size_t>(tankModel_->getBonesProperty().getCountProperty()));
    }

    void Animate(const GameTime& gameTime) {
        float time = static_cast<float>(
            gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());

        SteerRotation = std::sin(time * 0.75f) * 0.5f;
        TurretRotation = std::sin(time * 0.333f) * 1.25f;
        CannonRotation = std::sin(time * 0.25f) * 0.333f - 0.333f;
        HatchRotation = MathHelper::Clamp(std::sin(time * 2.0f) * 2.0f, -1.0f, 0.0f);
    }

    void Draw(const Matrix& world, const Matrix& view, const Matrix& projection,
              LightingMode lightMode, bool textureEnable) {
        tankModel_->getRootProperty()->setTransformProperty(world);

        Matrix wheelRotation = Matrix::CreateRotationX(WheelRotation);
        Matrix steerRotation = Matrix::CreateRotationY(SteerRotation);
        Matrix turretRotation = Matrix::CreateRotationY(TurretRotation);
        Matrix cannonRotation = Matrix::CreateRotationX(CannonRotation);
        Matrix hatchRotation = Matrix::CreateRotationX(HatchRotation);

        leftBackWheelBone_->setTransformProperty(wheelRotation * leftBackWheelTransform_);
        rightBackWheelBone_->setTransformProperty(wheelRotation * rightBackWheelTransform_);
        leftFrontWheelBone_->setTransformProperty(wheelRotation * leftFrontWheelTransform_);
        rightFrontWheelBone_->setTransformProperty(wheelRotation * rightFrontWheelTransform_);
        leftSteerBone_->setTransformProperty(steerRotation * leftSteerTransform_);
        rightSteerBone_->setTransformProperty(steerRotation * rightSteerTransform_);
        turretBone_->setTransformProperty(turretRotation * turretTransform_);
        cannonBone_->setTransformProperty(cannonRotation * cannonTransform_);
        hatchBone_->setTransformProperty(hatchRotation * hatchTransform_);

        tankModel_->CopyAbsoluteBoneTransformsTo(boneTransforms_);

        for (ModelMesh* mesh : tankModel_->getMeshesProperty()) {
            for (Effect* effect : mesh->getEffectsPropertyMutable()) {
                auto* basicEffect = dynamic_cast<BasicEffect*>(effect);
                if (!basicEffect) {
                    throw std::runtime_error("Tank model contains a non-BasicEffect mesh.");
                }

                basicEffect->World = boneTransforms_[static_cast<std::size_t>(
                    mesh->getParentBoneProperty()->getIndexProperty())];
                basicEffect->View = view;
                basicEffect->Projection = projection;

                switch (lightMode) {
                    case LightingMode::NoLighting:
                        basicEffect->setLightingEnabledProperty(false);
                        break;
                    case LightingMode::OneVertexLight:
                        basicEffect->EnableDefaultLighting();
                        basicEffect->setPreferPerPixelLightingProperty(false);
                        basicEffect->getDirectionalLight1Property().setEnabledProperty(false);
                        basicEffect->getDirectionalLight2Property().setEnabledProperty(false);
                        break;
                    case LightingMode::ThreeVertexLights:
                        basicEffect->EnableDefaultLighting();
                        basicEffect->setPreferPerPixelLightingProperty(false);
                        break;
                    case LightingMode::ThreePixelLights:
                        basicEffect->EnableDefaultLighting();
                        basicEffect->setPreferPerPixelLightingProperty(true);
                        break;
                }

                basicEffect->setSpecularColorProperty(Vector3(0.8f, 0.8f, 0.6f));
                basicEffect->setSpecularPowerProperty(16.0f);
                basicEffect->setTextureEnabledProperty(textureEnable);
            }

            mesh->Draw();
        }
    }

private:
    std::optional<Model> tankModel_;

    ModelBone* leftBackWheelBone_ = nullptr;
    ModelBone* rightBackWheelBone_ = nullptr;
    ModelBone* leftFrontWheelBone_ = nullptr;
    ModelBone* rightFrontWheelBone_ = nullptr;
    ModelBone* leftSteerBone_ = nullptr;
    ModelBone* rightSteerBone_ = nullptr;
    ModelBone* turretBone_ = nullptr;
    ModelBone* cannonBone_ = nullptr;
    ModelBone* hatchBone_ = nullptr;

    Matrix leftBackWheelTransform_;
    Matrix rightBackWheelTransform_;
    Matrix leftFrontWheelTransform_;
    Matrix rightFrontWheelTransform_;
    Matrix leftSteerTransform_;
    Matrix rightSteerTransform_;
    Matrix turretTransform_;
    Matrix cannonTransform_;
    Matrix hatchTransform_;

    std::vector<Matrix> boneTransforms_;
};

} // namespace ReachGraphicsDemoSample
