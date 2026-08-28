// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Spaceship.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>
#include <vector>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"

namespace Graphics3DSample
{
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using Microsoft::Xna::Framework::Graphics::BasicEffect;
    using Microsoft::Xna::Framework::Graphics::Model;

    /**
     * @brief Helper class for drawing a spaceship model with animated wheels and turret.
     */
    class Spaceship
    {
    private:
        // The XNA framework Model object that we are going to display.
        // XNA's Model is a reference type, so the original's field is null until Load assigns it;
        // std::optional is this port's established stand-in.
        std::optional<Model> spaceshipModel;

        // Array holding all the bone transform matrices for the entire model.
        // We could just allocate this locally inside the Draw method, but it
        // is more efficient to reuse a single array, as this avoids creating
        // unnecessary garbage.
        std::vector<Matrix> boneTransforms;

        // Spaceship drawing parameters
        Matrix projection;
        Matrix rotation;
        Matrix view;
        std::vector<bool> lights;
        bool isTextureEnabled = false;
        bool isPerPixelLightingEnabled = false;

    public:
        /**
         * @brief Loads the spaceship model.
         * @param content The ContentManager the model is loaded from.
         */
        void Load(ContentManager& content);

        /**
         * @brief Gets the projection matrix value.
         * @return The projection matrix.
         */
        [[nodiscard]] const Matrix& getProjectionProperty() const { return projection; }

        /**
         * @brief Sets the projection matrix value.
         * @param value The projection matrix.
         */
        void setProjectionProperty(const Matrix& value) { projection = value; }

        /**
         * @brief Gets the rotation matrix value.
         * @return The rotation matrix.
         */
        [[nodiscard]] const Matrix& getRotationProperty() const { return rotation; }

        /**
         * @brief Sets the rotation matrix value.
         * @param value The rotation matrix.
         */
        void setRotationProperty(const Matrix& value) { rotation = value; }

        /**
         * @brief Gets whether the model's texture is applied.
         * @return True when texturing is enabled.
         */
        [[nodiscard]] bool getIsTextureEnabledProperty() const { return isTextureEnabled; }

        /**
         * @brief Sets whether the model's texture is applied.
         * @param value True to enable texturing.
         */
        void setIsTextureEnabledProperty(bool value) { isTextureEnabled = value; }

        /**
         * @brief Gets the view matrix value.
         * @return The view matrix.
         */
        [[nodiscard]] const Matrix& getViewProperty() const { return view; }

        /**
         * @brief Sets the view matrix value.
         * @param value The view matrix.
         */
        void setViewProperty(const Matrix& value) { view = value; }

        /**
         * @brief Gets the lights states.
         * @return One flag per directional light.
         */
        [[nodiscard]] const std::vector<bool>& getLightsProperty() const { return lights; }

        /**
         * @brief Sets the lights states.
         * @param value One flag per directional light.
         */
        void setLightsProperty(const std::vector<bool>& value) { lights = value; }

        /**
         * @brief Gets the per pixel lighting preference.
         * @return True when per-pixel lighting is preferred.
         */
        [[nodiscard]] bool getIsPerPixelLightingEnabledProperty() const { return isPerPixelLightingEnabled; }

        /**
         * @brief Sets the per pixel lighting preference.
         * @param value True to prefer per-pixel lighting.
         */
        void setIsPerPixelLightingEnabledProperty(bool value) { isPerPixelLightingEnabled = value; }

        /** @brief Draws the spaceship model, using the current drawing parameters. */
        void Draw();

    private:
        /**
         * @brief Sets effect's per pixel lighting preference.
         * @param effect The effect to configure.
         */
        void SetEffectPerPixelLightingEnabled(BasicEffect& effect);

        /**
         * @brief Sets effects lighting properties.
         *
         * @param effect The effect to configure.
         * @param lights One flag per directional light.
         */
        void SetEffectLights(BasicEffect& effect, const std::vector<bool>& lights);
    };
}
