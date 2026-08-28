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

namespace SimpleAnimation
{
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using Microsoft::Xna::Framework::Graphics::Model;
    using Microsoft::Xna::Framework::Graphics::ModelBone;

    /**
     * @brief Helper class for drawing a tank model with animated wheels and turret.
     */
    class Tank
    {
    private:
        // The XNA framework Model object that we are going to display.
        // XNA's Model is a reference type, so the original's field is null until Load assigns it;
        // std::optional is this port's established stand-in.
        std::optional<Model> tankModel;

        // Shortcut references to the bones that we are going to animate.
        // We could just look these up inside the Draw method, but it is more
        // efficient to do the lookups while loading and cache the results.
        ModelBone* leftBackWheelBone = nullptr;
        ModelBone* rightBackWheelBone = nullptr;
        ModelBone* leftFrontWheelBone = nullptr;
        ModelBone* rightFrontWheelBone = nullptr;
        ModelBone* leftSteerBone = nullptr;
        ModelBone* rightSteerBone = nullptr;
        ModelBone* turretBone = nullptr;
        ModelBone* cannonBone = nullptr;
        ModelBone* hatchBone = nullptr;

        // Store the original transform matrix for each animating bone.
        Matrix leftBackWheelTransform;
        Matrix rightBackWheelTransform;
        Matrix leftFrontWheelTransform;
        Matrix rightFrontWheelTransform;
        Matrix leftSteerTransform;
        Matrix rightSteerTransform;
        Matrix turretTransform;
        Matrix cannonTransform;
        Matrix hatchTransform;

        // Array holding all the bone transform matrices for the entire model.
        // We could just allocate this locally inside the Draw method, but it
        // is more efficient to reuse a single array, as this avoids creating
        // unnecessary garbage.
        std::vector<Matrix> boneTransforms;

        // Current animation positions.
        float wheelRotationValue = 0.0f;
        float steerRotationValue = 0.0f;
        float turretRotationValue = 0.0f;
        float cannonRotationValue = 0.0f;
        float hatchRotationValue = 0.0f;

    public:
        /**
         * @brief Gets the wheel rotation amount.
         * @return The wheel rotation, in radians.
         */
        [[nodiscard]] float getWheelRotationProperty() const { return wheelRotationValue; }

        /**
         * @brief Sets the wheel rotation amount.
         * @param value The wheel rotation, in radians.
         */
        void setWheelRotationProperty(float value) { wheelRotationValue = value; }

        /**
         * @brief Gets the steering rotation amount.
         * @return The steering rotation, in radians.
         */
        [[nodiscard]] float getSteerRotationProperty() const { return steerRotationValue; }

        /**
         * @brief Sets the steering rotation amount.
         * @param value The steering rotation, in radians.
         */
        void setSteerRotationProperty(float value) { steerRotationValue = value; }

        /**
         * @brief Gets the turret rotation amount.
         * @return The turret rotation, in radians.
         */
        [[nodiscard]] float getTurretRotationProperty() const { return turretRotationValue; }

        /**
         * @brief Sets the turret rotation amount.
         * @param value The turret rotation, in radians.
         */
        void setTurretRotationProperty(float value) { turretRotationValue = value; }

        /**
         * @brief Gets the cannon rotation amount.
         * @return The cannon rotation, in radians.
         */
        [[nodiscard]] float getCannonRotationProperty() const { return cannonRotationValue; }

        /**
         * @brief Sets the cannon rotation amount.
         * @param value The cannon rotation, in radians.
         */
        void setCannonRotationProperty(float value) { cannonRotationValue = value; }

        /**
         * @brief Gets the entry hatch rotation amount.
         * @return The hatch rotation, in radians.
         */
        [[nodiscard]] float getHatchRotationProperty() const { return hatchRotationValue; }

        /**
         * @brief Sets the entry hatch rotation amount.
         * @param value The hatch rotation, in radians.
         */
        void setHatchRotationProperty(float value) { hatchRotationValue = value; }

        /**
         * @brief Loads the tank model and looks up the bones it animates.
         * @param content The ContentManager the model is loaded from.
         */
        void Load(ContentManager& content);

        /**
         * @brief Draws the tank model, using the current animation settings.
         *
         * @param world Where to place the tank in the world.
         * @param view The camera's view matrix.
         * @param projection The camera's projection matrix.
         */
        void Draw(const Matrix& world, const Matrix& view, const Matrix& projection);
    };
}
