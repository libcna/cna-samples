// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Tank.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>

#include "HeightMapInfo.hpp"

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"

namespace TanksOnAHeightmap
{
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using Microsoft::Xna::Framework::Graphics::Model;
    using Microsoft::Xna::Framework::Graphics::ModelBone;
    using Microsoft::Xna::Framework::Input::GamePadState;
    using Microsoft::Xna::Framework::Input::KeyboardState;

    /** @brief Controls and draws the animated tank on the terrain surface. */
    class Tank
    {
    private:
        static constexpr float TankVelocity = 2.0f;
        static constexpr float TankWheelRadius = 18.0f;
        static constexpr float TankTurnSpeed = 0.025f;

        Vector3 position;
        float facingDirection = 0.0f;

        std::optional<Model> model;
        Matrix orientation = Matrix::getIdentityProperty();
        Matrix wheelRollMatrix = Matrix::getIdentityProperty();

        ModelBone* leftBackWheelBone = nullptr;
        ModelBone* rightBackWheelBone = nullptr;
        ModelBone* leftFrontWheelBone = nullptr;
        ModelBone* rightFrontWheelBone = nullptr;

        Matrix leftBackWheelTransform;
        Matrix rightBackWheelTransform;
        Matrix leftFrontWheelTransform;
        Matrix rightFrontWheelTransform;

    public:
        /**
         * @brief Gets the tank position used to place the camera.
         * @return The world-space tank position.
         */
        [[nodiscard]] const Vector3& getPositionProperty() const;

        /**
         * @brief Gets the direction the tank faces, in radians.
         * @return The yaw angle used to position and aim the camera.
         */
        [[nodiscard]] float getFacingDirectionProperty() const;

        /**
         * @brief Loads the tank model and caches its four wheel bones.
         * @param content The game's content manager.
         */
        void LoadContent(ContentManager& content);

        /**
         * @brief Applies keyboard/gamepad movement and conforms the tank to the terrain normal.
         *
         * @param currentGamePadState Current player-one gamepad state.
         * @param currentKeyboardState Current keyboard state.
         * @param heightMapInfo Terrain data used to constrain and orient the tank.
         */
        void HandleInput(
            const GamePadState& currentGamePadState,
            const KeyboardState& currentKeyboardState,
            const HeightMapInfo& heightMapInfo);

        /**
         * @brief Draws the tank with animated wheels, default lighting and terrain fog.
         *
         * @param viewMatrix The view transform.
         * @param projectionMatrix The projection transform.
         */
        void Draw(const Matrix& viewMatrix, const Matrix& projectionMatrix);
    };
}
