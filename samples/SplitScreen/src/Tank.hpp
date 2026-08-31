// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Tank.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>
#include <vector>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"

namespace SplitScreenSample
{
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using Microsoft::Xna::Framework::Graphics::Model;
    using Microsoft::Xna::Framework::Graphics::ModelBone;

    /** @brief Draws the tank model with animated wheels, steering, turret, cannon and hatch. */
    class Tank
    {
    private:
        std::optional<Model> tankModel;

        ModelBone* leftBackWheelBone = nullptr;
        ModelBone* rightBackWheelBone = nullptr;
        ModelBone* leftFrontWheelBone = nullptr;
        ModelBone* rightFrontWheelBone = nullptr;
        ModelBone* leftSteerBone = nullptr;
        ModelBone* rightSteerBone = nullptr;
        ModelBone* turretBone = nullptr;
        ModelBone* cannonBone = nullptr;
        ModelBone* hatchBone = nullptr;

        Matrix leftBackWheelTransform;
        Matrix rightBackWheelTransform;
        Matrix leftFrontWheelTransform;
        Matrix rightFrontWheelTransform;
        Matrix leftSteerTransform;
        Matrix rightSteerTransform;
        Matrix turretTransform;
        Matrix cannonTransform;
        Matrix hatchTransform;

        std::vector<Matrix> boneTransforms;

        float wheelRotationValue = 0.0f;
        float steerRotationValue = 0.0f;
        float turretRotationValue = 0.0f;
        float cannonRotationValue = 0.0f;
        float hatchRotationValue = 0.0f;

    public:
        /** @brief Gets the wheel rotation amount. @return The wheel rotation in radians. */
        [[nodiscard]] float getWheelRotationProperty() const;

        /** @brief Sets the wheel rotation amount. @param value The wheel rotation in radians. */
        void setWheelRotationProperty(float value);

        /** @brief Gets the steering rotation amount. @return The steering rotation in radians. */
        [[nodiscard]] float getSteerRotationProperty() const;

        /** @brief Sets the steering rotation amount. @param value The steering rotation in radians. */
        void setSteerRotationProperty(float value);

        /** @brief Gets the turret rotation amount. @return The turret rotation in radians. */
        [[nodiscard]] float getTurretRotationProperty() const;

        /** @brief Sets the turret rotation amount. @param value The turret rotation in radians. */
        void setTurretRotationProperty(float value);

        /** @brief Gets the cannon rotation amount. @return The cannon rotation in radians. */
        [[nodiscard]] float getCannonRotationProperty() const;

        /** @brief Sets the cannon rotation amount. @param value The cannon rotation in radians. */
        void setCannonRotationProperty(float value);

        /** @brief Gets the entry-hatch rotation amount. @return The hatch rotation in radians. */
        [[nodiscard]] float getHatchRotationProperty() const;

        /** @brief Sets the entry-hatch rotation amount. @param value The hatch rotation in radians. */
        void setHatchRotationProperty(float value);

        /** @brief Loads the authentic tank model and caches all animated bones. @param content The game content manager. */
        void Load(ContentManager& content);

        /**
         * @brief Draws the tank using the current animation values.
         * @param world The root world transform.
         * @param view The camera view transform.
         * @param projection The camera projection transform.
         */
        void Draw(const Matrix& world, const Matrix& view, const Matrix& projection);
    };
}
